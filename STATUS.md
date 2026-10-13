# 当前状态（STATUS）

> 本文件是**当前版本口径**；`README.md` / `PROJECT.md` 讲的是「这个树是什么、怎么编译」。

## 现役版本

| 项 | 值 |
|---|---|
| 内核版本串 | `6.1.141-android14-11-o-ltcdz5-v1.1-opt162` |
| 用户态方案 | 风驰守护模块 `fengchi-boot` v2.7（versionCode 18） |
| 上一版公开快照 | tag `v1.1-opt60` |

## 源码快照为什么不跟着更新

`opt61` 起，内核里引入了社区 `OP-PAD-3-SM8750` 家族的 **hmbird 调度类**（`kernel/sched/hmbird/`，12 个文件）。该家族直接依赖 `slim.h` / `slim_sysctl.c`（20 个 `slim_*` 符号的定义处），而后两份文件不在本仓库中提供（见 `NOTICE.md` §三）。

⇒ **要么连 slim 单元一起发布，要么把 hmbird 家族一并排除**；本仓库目前选择后者，因此**不提供 opt162 的完整快照**。这不是遗漏，是明确取舍。

（备注：事后逐文件核对发现，这些文件的头部带 `SPDX-License-Identifier: GPL-2.0` 标识。本仓库仍然选择不提供，原因是**来源可追溯性**尚未确认，而不是许可本身有问题。）

## 需要 hmbird 调度类的读者

请自行从上游取得社区补丁（`OP-PAD-3-SM8750` 系列）；本仓库提供其余全部改动与构建脚本。

## 相关仓库

| 仓库 | 作用 |
|---|---|
| gt5pro-kernel-kit | 工具箱与全部过程档案（构建配方、四道 CRC 门禁、CHANGELOG） |
| gt5pro-hmbird | 风驰（scx / hmbird）移植记录与用户态方案 |
