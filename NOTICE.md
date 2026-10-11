# NOTICE · 来源与致谢

本仓库是 真我 GT5 Pro（RMX3888 / SM8650 / ColorOS 16，6.1.141 OKI 基线）自编内核的源码树，以 GPL-2.0 发布。

整理：github@ltcdz5 / 酷安@天玑1100逆天功耗。内容由 DeepSeek 协助整理。

## 一、本仓库自身的改动

设备树 / 构建配置 / 版本串标识等工程性改动，以及 `kernel/sched/hmbird_export.c`（导出兼容层，本项目自行编写）。

## 二、沿用或改写的第三方来源（在此说明并致谢）

| 文件 | 来源 |
| --- | --- |
| `kernel/sched/ext.c` · `ext.h` · `build_policy.c` · `Makefile` · `include/linux/sched/ext.h` · `kernel/bpf/bpf_struct_ops_types.h` · `kernel/Kconfig.preempt` · `include/trace/hooks/sched.h` · `kernel/sched/debug.c` · `vendor_hooks.c` | sched_ext 与调度接入相关的公开实现，以及本项目的接入改动 |

其余改动（F2FS、arm64、i2c、rpmsg、fwnode 等）来自 AOSP / GKI 官方源码与厂商公开的 GPL 内核源码。

## 三、已从本仓库移除的文件

下列文件的许可状态不明确，**不在本仓库中提供**，仅在此说明其存在与获取方式：

| 已移除的文件 | 说明 |
| --- | --- |
| `kernel/sched/slim.h` | slim / 风驰 接口声明 |
| `kernel/sched/slim_sysctl.c` | /proc/slim_sched 节点 |
| `kernel/sched/hmbird_sched_proc_main.c` | /proc/hmbird_sched 节点主体 |
| `kernel/sched/hmbird_sched_proc.h` | 上述两个文件所用的宏头 |
| `kernel/oplus_cpu/sched_ext/`（BUILD.bazel / main.c / hmbird_sched_proc.h / oplus_local_modules.bzl） | 同一家族的另一份副本，未被任何 Makefile 编译 |

这些内容属于社区公开的 hmbird / 风驰 调度家族补丁（OP-PAD-3-SM8750 系列）。需要它们的读者请自行从对应上游取得；若相关权利人认为本仓库的其它内容不妥，请联系后删除或调整。

为保持源码树可构建，`kernel/sched/build_policy.c` 中 include 上述两个 .c 的段落已一并移除；`kernel/sched/ext.c` 改为就地声明它实际用到的两个符号（`partial_enable`、`__scx_ops_enabled`，二者仍由 `kernel/sched/hmbird_export.c` 定义）；`arch/arm64/configs/gki_defconfig` 中 `CONFIG_HMBIRD_SCHED` 已关闭。

## 四、参考过但未包含其代码的项目

ferstar 系列、reigadegr/sun_action、reigadegr/hmbird_controller、WildKernels/kernel_patches、wanwei1028/oneplus_hmbird_fix、murongruyan/cezai-hmbird-ko、TheVoyager0777/Platform_Phantom、xhai-git/SCRC —— 仅作思路参考与致谢，本仓库不含其代码。
