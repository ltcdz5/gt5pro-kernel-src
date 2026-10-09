// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2024 Oplus. All rights reserved.
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cpufreq.h>
#include <linux/of.h>
#include "hmbird_sched_proc.h"
/*
 * BLOCK3 deviation: the factory kernel/sched/ext.h carries an include guard
 * (#ifndef __SCX_EXT_H); ours is the unguarded upstream one and is meant to
 * be pulled in exactly once, by ext.c.  build_policy.c #include's this file
 * right after ext.c, so everything ext.h provides (enum scx_exit_type, the
 * file-static scx_exit_type / scx_nr_rejected objects) is already in scope --
 * including it a second time only produces "redefinition of scx_wake_flags"
 * errors.  The real fix (guard ext.h) belongs to the ext.c/ext.h block.
 */
#include "slim.h"

#define HMBIRD_SCHED_PROC_DIR "hmbird_sched"
#define HMBIRD_PROC_PERMISSION  0666

int scx_enable;
int cpuctrl_high_ratio = 55;
int cpuctrl_low_ratio = 40;
int slim_stats;
int misfit_ds = 90;
/*
 * BLOCK7 de-dup (2/3): highres_tick_ctrl / highres_tick_ctrl_dbg.
 * Owned by kernel/sched/slim_sysctl.c (that is where they are initialised,
 * to true) and declared extern in kernel/sched/slim.h.  Do not define them
 * a second time.
 */
int sched_ravg_window_frame_per_sec = 125;
int parctrl_high_ratio = 55;
int parctrl_low_ratio = 40;
int parctrl_high_ratio_l = 65;
int parctrl_low_ratio_l = 50;
int isoctrl_high_ratio = 75;
int isoctrl_low_ratio = 60;
int isolate_ctrl;
int iso_free_rescue;
int heartbeat;
int heartbeat_enable;
int watchdog_enable;
int save_gov;

char saved_gov[NR_CPUS][16];

/*
 * ---- DT version_type compatibility layer (step 5d-4) ----
 *
 * The factory device tree declares the hmbird type at
 *   /proc/device-tree/soc/oplus,hmbird/version_type/type
 * as a string ("HMBIRD_OGKI" on SM8650).  The factory vendor module
 * compares it against "HMBIRD_GKI"; when the kernel's expected type and
 * the DT's do not agree the device does not come up -- that is the
 * mechanism behind the opt43 whole-device hang followed by a PMIC reset.
 *
 * This layer reads the same node, but it is strictly defensive:
 *   - a missing node, a missing property or an unreadable value is NOT a
 *     failure path: we log once, keep HMBIRD_DT_TYPE_UNKNOWN and carry on;
 *   - nothing here panics, BUGs, or fails the init/probe path;
 *   - the value only ever *narrows* what the in-kernel hmbird path is
 *     willing to do on its own; it never force-enables anything.
 *
 * No EXPORT_SYMBOL here: hmbird is built into this image, and the export
 * set has to stay at its 15,489 names.
 */
#define HMBIRD_DT_TYPE_UNKNOWN	"UNKNOWN"
#define HMBIRD_DT_TYPE_GKI	"HMBIRD_GKI"
#define HMBIRD_DT_TYPE_OGKI	"HMBIRD_OGKI"

static char hmbird_dt_version_type[32] = HMBIRD_DT_TYPE_UNKNOWN;

/* Mirrors is_hmbird_gki_type() from the reference compat patch. */
bool is_hmbird_gki_type(void)
{
	return !strcmp(hmbird_dt_version_type, HMBIRD_DT_TYPE_GKI);
}

/* SM8650's correct type, per the vendor SoC table. */
bool is_hmbird_ogki_type(void)
{
	return !strcmp(hmbird_dt_version_type, HMBIRD_DT_TYPE_OGKI);
}

/*
 * Safe-degrade decision: only an exact HMBIRD_OGKI match means "the DT and
 * this kernel agree".  Node absent / property absent / unreadable / any
 * other string all return false, so the in-kernel hmbird path stays idle
 * unless userspace asks for it through /proc/hmbird_sched/scx_enable.
 */
bool hmbird_dt_type_matches(void)
{
	return is_hmbird_ogki_type();
}

const char *hmbird_dt_version_type_name(void)
{
	return hmbird_dt_version_type;
}

static int hmbird_read_version_type(void)
{
	struct device_node *node, *vt;
	const char *type_str = NULL;
	int ret;

	node = of_find_node_by_path("/soc/oplus,hmbird");
	if (!node) {
		pr_info("hmbird_sched: no /soc/oplus,hmbird node in DT; version_type stays %s (safe degrade)\n",
			HMBIRD_DT_TYPE_UNKNOWN);
		return -ENOENT;
	}

	vt = of_get_child_by_name(node, "version_type");
	if (vt) {
		ret = of_property_read_string(vt, "type", &type_str);
		of_node_put(vt);
	} else {
		/* tolerate a flat layout: the property directly on oplus,hmbird */
		ret = of_property_read_string(node, "version_type", &type_str);
	}
	of_node_put(node);

	if (ret || !type_str) {
		pr_info("hmbird_sched: DT version_type/type missing or unreadable (%d); stays %s (safe degrade)\n",
			ret, HMBIRD_DT_TYPE_UNKNOWN);
		return ret ? ret : -EINVAL;
	}

	strscpy(hmbird_dt_version_type, type_str, sizeof(hmbird_dt_version_type));
	pr_info("hmbird_sched: DT version_type = %s (gki=%d ogki=%d)\n",
		hmbird_dt_version_type, is_hmbird_gki_type(), is_hmbird_ogki_type());
	return 0;
}

/*
 * BLOCK7 de-dup (1/3): slim_for_app.
 * kernel/sched/hmbird_export.c defines and EXPORT_SYMBOL_GPL()'s it, and the
 * factory file defined it a second time.  One definition only.
 */
extern int slim_for_app;

/*
 * BLOCK7 de-dup (3/3): hmbird_dir.
 * The factory file declared a function-local of this name, which shadowed the
 * global struct proc_dir_entry *hmbird_dir that kernel/sched/hmbird_export.c
 * defines and EXPORT_SYMBOL_GPL()'s.  hmbird_export.c is authoritative, so use
 * the export.
 *
 * This is not cosmetic: oplus_bsp_sched_ext.ko does NOT create
 * /proc/hmbird_sched itself (the literal is absent from its .ko -- only
 * "slim_walt", "slim_freq_gov" and "scx_shadow_tick_enable" are there), it
 * imports hmbird_dir and hangs its own entries under it.  The kernel therefore
 * has to publish the real /proc/hmbird_sched entry through that global, which
 * is what hmbird_proc_init() below does.
 */
extern struct proc_dir_entry *hmbird_dir;

static int set_proc_buf_val(struct file *file, const char __user *buf, size_t count, int *val)
{
	char kbuf[5] = {0};
	int err;

	if (count >= 5)
		return -EFAULT;

	if (copy_from_user(kbuf, buf, count)) {
		pr_err("hmbird_sched : Failed to copy_from_user\n");
		return -EFAULT;
	}

	err = kstrtoint(strstrip(kbuf), 0, val);
	if (err < 0) {
		pr_err("hmbird_sched: Failed to exec kstrtoint\n");
		return -EFAULT;
	}

	return 0;
}

/* common ops begin */
static ssize_t hmbird_common_write(struct file *file,
				   const char __user *buf,
				   size_t count, loff_t *ppos)
{
	int *pval = (int *)pde_data(file_inode(file));

	if (set_proc_buf_val(file, buf, count, pval))
		return -EFAULT;

	return count;
}

static int hmbird_common_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%d\n", *(int *) m->private);
	return 0;
}

static int hmbird_common_open(struct inode *inode, struct file *file)
{
	return single_open(file, hmbird_common_show, pde_data(inode));
}
HMBIRD_PROC_OPS(hmbird_common, hmbird_common_open, hmbird_common_write);
/* common ops end */

/* scx_enable ops begin */
static ssize_t scx_enable_proc_write(struct file *file, const char __user *buf,
								size_t count, loff_t *ppos)
{
	int *pval = (int *)pde_data(file_inode(file));

	if (set_proc_buf_val(file, buf, count, pval))
		return -EFAULT;

	return count;
}
HMBIRD_PROC_OPS(scx_enable, hmbird_common_open, scx_enable_proc_write);
/* scx_enable ops end */

/* hmbird_stats ops begin */
#define MAX_STATS_BUF	(2000)
static int hmbird_stats_proc_show(struct seq_file *m, void *v)
{
    	seq_puts(m, "global stat:0, 0\n");
    	seq_puts(m, "cpu_allow_fail:0, 0\n");
    	seq_puts(m, "rt_cnt:0, 0\n");
    	seq_puts(m, "key_task_cnt:0, 0\n");
    	seq_puts(m, "switch_idx:0, 0\n");
    	seq_puts(m, "timeout_cnt:0, 0\n");
    	seq_puts(m, "total_dsp_cnt:0, 0\n");
    	seq_puts(m, "move_rq_cnt:0, 0\n");
	seq_puts(m, "select_cpu:0, 0\n");
    
    	// Output gdsq_cnt array
    	for (int i = 0; i < 10; i++) {
        	seq_printf(m, "gdsq_cnt[%d]:0, 0\n", i);
    	}
    
    	seq_puts(m, "err_idx:0, 0, 0, 0, 0\n");
    
    	// 扩展 pcp_timeout_cnt 输出（增加索引6-7）
    	for (int i = 0; i < 8; i++) {  // 从 6 扩大到 8
        	seq_printf(m, "pcp_timeout_cnt[%d]:0\n", i);
    	}
    
    	// 添加缺失的 pcp_ldsq_cnt 数组输出（索引0-7）
    	for (int i = 0; i < 8; i++) {
        	seq_printf(m, "pcp_ldsq_cnt[%d]:0, 0\n", i);
    	}
    
    	// 添加缺失的 pcp_enql_cnt 数组输出（索引0-7）
    	for (int i = 0; i < 8; i++) {
        	seq_printf(m, "pcp_enql_cnt[%d]:0\n", i);
    	}
    
    	// 原有的调度器状态变量输出
    	seq_printf(m, "SCX Enabled: %d\n", scx_enable);
    	seq_printf(m, "Partial Enable: %d\n", partial_enable);
    	seq_printf(m, "Slim Stats: %d\n", slim_stats);
    	seq_printf(m, "Heartbeat: %d\n", heartbeat);
    	seq_printf(m, "Misfit DS: %d\n", misfit_ds);
    	seq_printf(m, "Highres Tick Ctrl: %u\n", highres_tick_ctrl);
    	seq_printf(m, "Watchdog Enable: %d\n", watchdog_enable);
    
    	// SCX 特定统计信息
    	seq_printf(m, "SCX Exit Type: %d\n", atomic_read(&scx_exit_type));
    	seq_printf(m, "SCX Rejected Tasks: %lld\n", atomic64_read(&scx_nr_rejected));
    
    	// 调度频率信息
    	seq_printf(m, "Sched Ravg Window Frame Per Sec: %d\n", 
               sched_ravg_window_frame_per_sec);

	seq_printf(m, "DT Version Type: %s (match=%d)\n",
		hmbird_dt_version_type_name(), hmbird_dt_type_matches());
    
    	return 0;
 }

static int hmbird_stats_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, hmbird_stats_proc_show, inode);
}
HMBIRD_PROC_OPS(hmbird_stats, hmbird_stats_proc_open, NULL);
/* hmbird_stats ops end */

static ssize_t save_gov_str(struct file *file, const char __user *buf,
					size_t count, loff_t *ppos)
{
	int cpu;
	struct cpufreq_policy *policy;

	for_each_present_cpu(cpu) {
		policy = cpufreq_cpu_get(cpu);
		if (cpu != policy->cpu)
			continue;
	}
	return count;
}
HMBIRD_PROC_OPS(save_gov, hmbird_common_open, save_gov_str);

static int hmbird_proc_init(void)
{

	/* mkdir /proc/hmbird_sched */
	hmbird_dir = proc_mkdir(HMBIRD_SCHED_PROC_DIR, NULL);
	if (!hmbird_dir) {
		pr_err("Error creating proc directory %s\n", HMBIRD_SCHED_PROC_DIR);
		return -ENOMEM;
	}

	/* /proc/hmbird_sched--begin */
	HMBIRD_CREATE_PROC_ENTRY_DATA("scx_enable", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&scx_enable_proc_ops,
					&scx_enable);

	HMBIRD_CREATE_PROC_ENTRY_DATA("partial_ctrl", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&partial_enable);

	HMBIRD_CREATE_PROC_ENTRY_DATA("cpuctrl_high", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&cpuctrl_high_ratio);

	HMBIRD_CREATE_PROC_ENTRY_DATA("cpuctrl_low", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&cpuctrl_low_ratio);

	HMBIRD_CREATE_PROC_ENTRY_DATA("slim_stats", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&slim_stats);

	HMBIRD_CREATE_PROC_ENTRY_DATA("hmbirdcore_debug", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&hmbirdcore_debug);

	HMBIRD_CREATE_PROC_ENTRY_DATA("slim_for_app", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&slim_for_app);

	HMBIRD_CREATE_PROC_ENTRY_DATA("misfit_ds", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&misfit_ds);

	HMBIRD_CREATE_PROC_ENTRY_DATA("save_gov", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&save_gov_proc_ops,
					&save_gov);

	HMBIRD_CREATE_PROC_ENTRY_DATA("heartbeat", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&heartbeat);

	HMBIRD_CREATE_PROC_ENTRY_DATA("heartbeat_enable", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&heartbeat_enable);

	HMBIRD_CREATE_PROC_ENTRY_DATA("watchdog_enable", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&watchdog_enable);

	HMBIRD_CREATE_PROC_ENTRY_DATA("isolate_ctrl", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&isolate_ctrl);

	HMBIRD_CREATE_PROC_ENTRY_DATA("parctrl_high_ratio", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&parctrl_high_ratio);

	HMBIRD_CREATE_PROC_ENTRY_DATA("parctrl_low_ratio", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&parctrl_low_ratio);

	HMBIRD_CREATE_PROC_ENTRY_DATA("isoctrl_high_ratio", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&isoctrl_high_ratio);

	HMBIRD_CREATE_PROC_ENTRY_DATA("isoctrl_low_ratio", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&isoctrl_low_ratio);

	HMBIRD_CREATE_PROC_ENTRY_DATA("iso_free_rescue", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&iso_free_rescue);

	HMBIRD_CREATE_PROC_ENTRY_DATA("parctrl_high_ratio_l", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&parctrl_high_ratio_l);

	HMBIRD_CREATE_PROC_ENTRY_DATA("parctrl_low_ratio_l", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_common_proc_ops,
					&parctrl_low_ratio_l);

	HMBIRD_CREATE_PROC_ENTRY("hmbird_stats", HMBIRD_PROC_PERMISSION,
					hmbird_dir,
					&hmbird_stats_proc_ops);
	/* /proc/hmbird_sched--end */


	return 0;
}

static int __init hmbird_common_init(void)
{
	int ret;

	ret = hmbird_read_version_type();
	if (ret)
		pr_warn("hmbird_sched: DT version_type unavailable; in-kernel hmbird stays inactive unless enabled from userspace\n");

	return hmbird_proc_init();
}

static void __exit hmbird_common_exit(void)
{
}

module_init(hmbird_common_init);
module_exit(hmbird_common_exit);
MODULE_LICENSE("GPL v2");

