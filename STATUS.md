# 当前状态（STATUS）

> 本文件是**当前版本口径**；`README.md` / `PROJECT.md` 讲的是「这个树是什么、怎么编译」。

## 现役版本

| 项 | 值 |
|---|---|
| 内核版本串 | `6.1.141-android14-11-o-ltcdz5-v1.1-opt162` |
| 用户态方案 | 风驰守护模块 `fengchi-boot` v2.7（versionCode 18） |
| 上一版公开快照 | tag `v1.1-opt60` |

## 本快照包含什么、不包含什么

`main` 现在跟进到 **opt162**，包含 `opt61`→`opt162` 的全部改动，**除**下列不随本仓库发布的单元（理由见 `NOTICE.md` §三）：

| 未包含 | 说明 |
|---|---|
| `kernel/sched/hmbird/`（12 文件） | hmbird 调度类实现本身 |
| `kernel/sched/slim.h` / `slim_sysctl.c` | 20 个 `slim_*` 符号的定义处 |
| `kernel/sched/hmbird_sched_proc_main.c` / `.h` | /proc/hmbird_sched 节点主体 |
| `kernel/oplus_cpu/sched_ext/` | 同一家族的另一份副本 |

与这些单元对接的**钩子仍在树里**，全部由 `CONFIG_HMBIRD_SCHED_CORE` 保护、默认 N，因此**不参与编译**；`gki_defconfig` 里 `CONFIG_HMBIRD_SCHED` 也是关闭的。这样本仓库既给出可构建的当前源码，又不包含上述单元。

> 该选项在本仓库中**不可启用**（家族文件未提供），保留它只是为了上述 `#ifdef` 有意义。

## 需要 hmbird 调度类的读者：缺什么、怎么取

### 缺的文件（共 30 个，分两类）

**甲类：19 个，来自上游补丁**（就是 `opt61` 引入的那批）

| 路径 | 数量 |
|---|---|
| `kernel/sched/hmbird/` 中除 `Makefile` / `hmbird_all.c` 外的 11 个文件 | 11 |
| `kernel/sched/hmbird.h` | 1 |
| `include/linux/sched/`：`hmbird.h`、`hmbird_proc_val.h`、`hmbird_version.h`、`sa_common.h`、`sa_common_struct.h`、`sa_oemdata.h`、`sched_ext.h` | 7 |

**乙类：11 个，本仓库自有移植件，上游补丁里没有**

| 路径 | 说明 |
|---|---|
| `kernel/sched/slim.h`、`kernel/sched/slim_sysctl.c` | 20 个 `slim_*` 符号的定义处（`kernel/sched/hmbird/slim.h` 是上游那份，与这两份不同） |
| `kernel/sched/hmbird_sched_proc_main.c`、`kernel/sched/hmbird_sched_proc.h` | `/proc/hmbird_sched` 节点主体 |
| `kernel/sched/hmbird/Makefile`、`kernel/sched/hmbird/hmbird_all.c` | 把家族内联成单个编译单元的构建胶水 |
| `kernel/oplus_cpu/sched_ext/`：`BUILD.bazel`、`hmbird_sched_proc.h`、`main.c`、`oplus_local_modules.bzl` | 同家族的另一份副本（本仓库里无人编译） |
| `.gitignore` | 本仓库自有 |

> ⚠️ **乙类不是可选项**：`kernel/sched/hmbird/hmbird.c:17` / `hmbird_misc.c:14` / `hmbird_sched_proc.c:15` 直接 `#include "slim.h"`，并引用 20 个 `slim_*` 符号，定义在 `kernel/sched/slim_sysctl.c`。只补甲类、不补乙类**无法构建**。这正是本仓库把甲、乙两类一并排除的原因（二者同进同退）。

### 上游补丁（甲类 19 个文件的唯一来源）

| 项 | 值 |
|---|---|
| 仓库 | [`WildKernels/kernel_patches`](https://github.com/WildKernels/kernel_patches) |
| 文件 | `oneplus/hmbird/fengchi_OP-PAD-3-SM8750_A15.patch` |
| 大小 | 255,895 字节 |
| 本仓库所用版本的 git blob SHA-1 | `dcf80f96f244e3c4ce4a9e84035ef6b1b6a503b9` |
| 直接下载 | <https://raw.githubusercontent.com/WildKernels/kernel_patches/main/oneplus/hmbird/fengchi_OP-PAD-3-SM8750_A15.patch> |
| 文件头许可 | `SPDX-License-Identifier: GPL-2.0` / `GPL-2.0-only`，版权行 `Copyright (C) 2024 Oplus.` 等 |

```bash
curl -L -o fengchi.patch \
  https://raw.githubusercontent.com/WildKernels/kernel_patches/main/oneplus/hmbird/fengchi_OP-PAD-3-SM8750_A15.patch
# 核对（可选，确认拿到的是本仓库所用那一版）
git hash-object fengchi.patch
# 期望 dcf80f96f244e3c4ce4a9e84035ef6b1b6a503b9
```

### 怎么用

1. 取本仓库（已含 `opt61`→`opt162` 的其余全部改动）。
2. 从上面的补丁里抽出甲类 19 个文件（补丁正文里它们都是 `+` 新增文件）。
3. 自行重建乙类 11 个对接件（本仓库不提供）。
4. 打开 `CONFIG_HMBIRD_SCHED` / `CONFIG_HMBIRD_SCHED_CORE` 再构建。

> 上游补丁另外还改了 17 个已有文件（`core.c`、`sched.h`、`fork.c`、`build_policy.c`、`gki_defconfig`、`vmlinux.lds.h`、`include/linux/sched.h`、`include/uapi/linux/sched.h`、`Kconfig.preempt` 等）。本仓库里这些改动**已经包含**（它们就在 `opt61`→`opt162` 的增量里），不需要再打一遍。

## 相关仓库

| 仓库 | 作用 |
|---|---|
| gt5pro-kernel-kit | 工具箱与全部过程档案（构建配方、四道 CRC 门禁、CHANGELOG） |
| gt5pro-hmbird | 风驰（scx / hmbird）移植记录与用户态方案 |
