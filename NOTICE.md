# NOTICE · 来源与致谢

本仓库是 **真我 GT5 Pro（RMX3888 / SM8650，6.1.141 OKI 基线）** 的内核源码树，许可证为 **GPL-2.0**（见 LICENSE）。

> 整理者：**github@ltcdz5** ／ **酷安@天玑1100逆天功耗** ｜ 内容由 **DeepSeek** 协助整理

## 一、本仓库自身的改动

- 设备树 / 构建配置 / 版本串标识等工程性改动
- kernel/sched/hmbird_export.c —— 导出兼容层，本项目自行编写

## 二、沿用或改写的第三方来源（在此说明并致谢）

| 文件 | 来源 |
|---|---|
| kernel/sched/hmbird_sched_proc_main.c · hmbird_sched_proc.h | 社区公开的 hmbird / 风驰 调度家族补丁（OP-PAD-3-SM8750 系列）|
| kernel/sched/slim.h · slim_sysctl.c | 同上（slim / 风驰 相关接口）|
| kernel/sched/ext.c · ext.h · build_policy.c · Makefile · include/linux/sched/ext.h · kernel/bpf/bpf_struct_ops_types.h · kernel/Kconfig.preempt · include/trace/hooks/sched.h · kernel/sched/debug.c · vendor_hooks.c | sched_ext 与调度接入相关的公开实现，以及本项目的接入改动 |

**说明**：上述来源的许可状态并不完全明确。本项目在此**说明引用并致谢**；若相关权利人认为不妥，请联系后删除或调整。

其余改动（F2FS、arm64、i2c、rpmsg、fwnode 等）来自 **AOSP / GKI 官方源码**与厂商公开的 GPL 内核源码。

## 三、参考过但未包含其代码的项目

ferstar 系列、reigadegr/sun_action、reigadegr/hmbird_controller、WildKernels/kernel_patches、wanwei1028/oneplus_hmbird_fix、murongruyan/cezai-hmbird-ko、TheVoyager0777/Platform_Phantom、xhai-git/SCRC 等 —— 仅作思路参考与致谢，本仓库不含其代码。
