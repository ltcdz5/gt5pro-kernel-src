// SPDX-License-Identifier: GPL-2.0-only
/*
 * OPPO sched_ext / hmbird export hub.
 *
 * "Some variables need to export, if CONFIG_HMBIRD_SCHED not configed,
 *  error will be reported by compiler, These variables must be defined
 *  outside of CONFIG_HMBIRD_SCHED MICRO."
 *
 * Ported from reigadegr/sun_action::patchs/6.1/6.1sched_ext.diff (hub file),
 * cross-checked against the *factory* kernel of this very device:
 *   - types   : /home/builder/abi/btf2/stock.btf  (BTF of images/boot_a.img)
 *   - code    : /home/builder/stock/Image.stock   (raw Image of images/boot_a.img)
 *   - exports : __ksymtab walk of the same Image  -> stock exports exactly the
 *               ten symbols defined below (see lab 2026-10-08/scx-step4b/).
 *
 * These symbols are consumed by the vendor module oplus_bsp_sched_ext.ko.
 *
 * Scope note: this hub only publishes state/objects.  It never registers a
 * sched_ext (BPF struct_ops) scheduler and never flips scx_ops_enable() --
 * registering a scheduler on this SoC hard-hangs the device (see kit
 * CHANGELOG section 3.3).  /sys/kernel/sched_ext/enabled therefore stays 0.
 */
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/atomic.h>
#include <linux/percpu.h>
#include <linux/proc_fs.h>
#include <linux/export.h>
#include <linux/sched.h>
#include <linux/sched/ext.h>
#include <linux/cpumask.h>
#include <linux/slab.h>

int slim_for_app;
EXPORT_SYMBOL_GPL(slim_for_app);

/*
 * BLOCK1/BLOCK2 (step 5b): the hmbird knobs consumed by kernel/sched/ext.c.
 *
 * ext.c is compiled whenever CONFIG_SCHED_CLASS_EXT=y, i.e. also when
 * CONFIG_HMBIRD_SCHED=n -- and in that configuration
 * kernel/sched/hmbird_sched_proc_main.c / slim_sysctl.c (the previous owners
 * of these three) are not built at all.  Same rule as the ten symbols above:
 * "These variables must be defined outside of CONFIG_HMBIRD_SCHED MICRO."
 * The /proc writers in hmbird_sched_proc_main.c reach them through slim.h.
 */
int hmbirdcore_debug;
int partial_enable;

noinline int tracing_mark_write(const char *buf)
{
	trace_printk(buf);
	return 0;
}

struct scx_sched_rq_stats {
	u64		window_start;
	u64		latest_clock;
	u32		prev_window_size;
	u64		task_exec_scale;
	u64		prev_runnable_sum;
	u64		curr_runnable_sum;
};

DEFINE_PER_CPU(struct scx_sched_rq_stats, scx_sched_rq_stats);
EXPORT_SYMBOL_GPL(scx_sched_rq_stats);

atomic64_t scx_irq_work_lastq_ws;
EXPORT_SYMBOL_GPL(scx_irq_work_lastq_ws);

atomic_t ext_module_loaded = ATOMIC_INIT(0);
EXPORT_SYMBOL_GPL(ext_module_loaded);

struct proc_dir_entry *hmbird_dir;
EXPORT_SYMBOL_GPL(hmbird_dir);

atomic_t non_ext_task = ATOMIC_INIT(true);
EXPORT_SYMBOL_GPL(non_ext_task);

/*
 * Upstream keeps this as a static key; the OPPO sched_ext/hmbird stack turns
 * it into a plain atomic_t so that it can be shared with the vendor module.
 */
atomic_t __scx_ops_enabled;
EXPORT_SYMBOL_GPL(__scx_ops_enabled);

/*
 * CPU isolation masks of the OPPO sched_ext scheduler.
 *
 * Layout is byte-for-byte the factory kernel's struct (BTF: "struct
 * scx_iso_masks", size 40 = 5 x sizeof(cpumask_var_t); NR_CPUS=32 and
 * CONFIG_CPUMASK_OFFSTACK=n, so cpumask_var_t is struct cpumask[1] == 8 bytes).
 * The member order matters: oplus_bsp_sched_ext.ko indexes the object by raw
 * offset (cpu_util_policy_store: ex_free@0x00, exclusive@0x08, partial@0x10),
 * so an anonymous 4-member struct would be read off by one member.
 */
struct scx_iso_masks {
	cpumask_var_t	ex_free;
	cpumask_var_t	exclusive;
	cpumask_var_t	partial;
	cpumask_var_t	big;
	cpumask_var_t	little;
};

struct scx_iso_masks iso_masks;
EXPORT_SYMBOL_GPL(iso_masks);

/*
 * Size of the hmbird minidump region.  The factory kernel hands out a
 * kmalloc(4096, GFP_KERNEL) buffer here and reports exactly 0x1000:
 *   Image.stock scx_get_md_info():  mov w9,#0x1000 ; ldr x8,[x8,#3584] ;
 *                                   str x8,[x0] ; str x9,[x1] ; ret
 *   (the ldr reads the buffer pointer from .bss @0x2217e00, which is filled by
 *    the hmbird init: kmalloc_trace(kmalloc_caches[..], 0xdc0, 0x1000)).
 *
 * The consumer (oplus_bsp_sched_ext.ko:hmbird_misc_init) BUG()s on a NULL
 * vaddr, so we publish a real, correctly sized linear-mapped buffer instead of
 * reporting "nothing".  Its *contents* are not produced here: the hmbird core
 * that fills it is not part of this tree (see the boundary note in the kit
 * CHANGELOG).  The buffer is zeroed, so a minidump captures an empty region.
 */
#define HMBIRD_MD_SIZE	4096

static void *hmbird_md_addr;

void scx_get_md_info(unsigned long *vaddr, unsigned long *size)
{
	if (vaddr)
		*vaddr = (unsigned long)hmbird_md_addr;
	if (size)
		*size = HMBIRD_MD_SIZE;
}
EXPORT_SYMBOL_GPL(scx_get_md_info);

/*
 * Factory kernel, verbatim (Image.stock task_is_scx @0x2ecb18):
 *   ldr  x8, [x0, #832]     ; p->sched_class
 *   adrp x9, &ext_sched_class ; add x9, x9, #0x980
 *   cmp  x8, x9 ; cset w0, eq ; ret
 * i.e. exactly the implementation shipped in the OPPO source drop
 * (6.1sched_ext.diff, kernel/sched/ext.c):
 *   "Must with rq lock held."  bool task_is_scx(struct task_struct *p)
 *                              { return p->sched_class == &ext_sched_class; }
 *
 * With scx never enabled no task can be in ext_sched_class, so this returns
 * false for every task -- i.e. identical to the previous stub at run time,
 * but with the real semantics when scx is ever switched on.
 */
struct sched_class;
extern const struct sched_class ext_sched_class;

#ifdef CONFIG_SCHED_CLASS_EXT
bool task_is_scx(struct task_struct *p)
{
	return p->sched_class == &ext_sched_class;
}
#else
bool task_is_scx(struct task_struct *p)
{
	return false;
}
#endif
EXPORT_SYMBOL_GPL(task_is_scx);

/*
 * iso_masks has to be usable as soon as the vendor module runs its init, so
 * populate it at boot.  No CPU is marked "exclusive"/"partial": the vendor
 * governor then simply never isolates a CPU, which is the conservative
 * default.  "big"/"little" are set to "all CPUs" because this tree has no
 * hmbird topology owner; the module shipped with this ROM only ever reads
 * ex_free/exclusive/partial.
 */
static int __init hmbird_export_init(void)
{
	if (!zalloc_cpumask_var(&iso_masks.ex_free, GFP_KERNEL) ||
	    !zalloc_cpumask_var(&iso_masks.exclusive, GFP_KERNEL) ||
	    !zalloc_cpumask_var(&iso_masks.partial, GFP_KERNEL) ||
	    !zalloc_cpumask_var(&iso_masks.big, GFP_KERNEL) ||
	    !zalloc_cpumask_var(&iso_masks.little, GFP_KERNEL)) {
		pr_err("hmbird_export: cannot allocate iso_masks\n");
		return -ENOMEM;
	}

	cpumask_setall(iso_masks.big);
	cpumask_setall(iso_masks.little);

	hmbird_md_addr = kzalloc(HMBIRD_MD_SIZE, GFP_KERNEL);
	if (!hmbird_md_addr)
		pr_warn("hmbird_export: no minidump region; scx_get_md_info() reports vaddr=0\n");

	return 0;
}
arch_initcall(hmbird_export_init);
