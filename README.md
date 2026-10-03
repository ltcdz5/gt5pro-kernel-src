<!-- badges -->
[![License](https://img.shields.io/badge/License-GPL--2.0-blue.svg)](LICENSE)
[![Device](https://img.shields.io/badge/Device-Realme%20GT5%20Pro%20(RMX3888)-orange.svg)](https://github.com/ltcdz5/gt5pro-kernel-src)
[![SoC](https://img.shields.io/badge/SoC-Snapdragon%208%20Gen%203%20(SM8650)-0a7bbb.svg)]()
[![Kernel](https://img.shields.io/badge/Kernel-Linux%206.1.141%20OKI-f6a500.svg)]()
[![Android](https://img.shields.io/badge/Android-16%20(ColorOS%2FRUI)-3ddc84.svg)]()
[![Status](https://img.shields.io/badge/Status-Open%20Source%20%C2%B7%20Public-2ea44f.svg)]()

# GT5 Pro 自编内核源码树

**真我 GT5 Pro（RMX3888 · SM8650 "pineapple" · Android 16 / ColorOS）** 的自编内核源码树。

> **开源状态**：本仓库以 **GPL-2.0** 开源（许可证全文见 [`LICENSE`](LICENSE)，内核原有声明见 [`COPYING`](COPYING)）。
> **配套仓库**：[`ltcdz5/gt5pro-kernel-kit`](https://github.com/ltcdz5/gt5pro-kernel-kit) —— 构建脚本、双闸门、完整工作台账。

---

## ⚠️ 复刻 / 衍生 / 借鉴来源（合规标注）

**本仓库是复刻/衍生项目，不是原创内核。** 各层来源与许可如下，**使用时务必遵守对应许可**：

| 层级 | 来源 | 许可 | 说明 |
|---|---|---|---|
| GKI 基底 | **AOSP `kernel/common`**（`android14-6.1`）<br>`https://android.googlesource.com/kernel/common` | GPL-2.0 | Google 通用内核（GKI） |
| 厂商源码 | **OPPO / oplus 官方开源** `android_kernel_common_oneplus_sm8650` | GPL-2.0 | 本仓库的**直接基座**（一加/OPPO/真我 SM8650 共用树） |
| 流程参考 | **`cctv18/oppo_oplus_realme_sm8650`** | GPL-2.0 | 同设备家族自动化编译项目；版本命名与构建流程有所*借鉴* |
| Root / 隐藏 | `tiann/KernelSU`、`ReSukiSU`、`SukiSU-Ultra`、`ShirkNeko/susfs4ksu` | GPL-2.0 | 以 LKM 形式集成，*引用*其内核补丁 |
| 内核补丁 | `ferstar`（lz4 1.10.0 / zstd 1.5.7，移植 by `Xiaomichael`） | 见上游 | *引用*其补丁 |
| IO 调度器 | 三星 SSG（社区整理移植） | GPL-2.0 | *借鉴* |
| 调度扩展 | **LunarKernel（LSE）** | 见上游 | *借鉴*其 slim_walt 模块化路线（未含其代码） |

> 逐项完整标注见 [`NOTICE.md`](NOTICE.md)。
> **本仓库未包含任何厂商闭源 blob**；对厂商源码的全部修改均以 GPL-2.0 公开。

---

## 本仓库相对上游的改动

在厂商源码之上应用的内核补丁与配置取舍（缺陷修复、CVE 回移、config 精简等），
**每一版的改动、验证过程与结论**都记录在配套仓库的台账里：

- 构建与闸门脚本、坑清单：`gt5pro-kernel-kit`
- 每版上机核验报告：`gt5pro-kernel-kit/v1.1-opt*-上机核验-*.md`

**版本命名**：`6.1.141-android14-11-o-ltcdz5-v<族>.<次>-opt<构建序>`
（例：`6.1.141-android14-11-o-ltcdz5-v1.1-opt42`）

## 刷写须知（重要）

本机为 **A/B 虚拟分区 + 解锁 BL**，实践约定：

- **只刷 `boot_a`**。永不触碰 `init_boot` / `devinfo` / `abl` / `xbl` / `vbmeta` / `super` / `userdata` / `boot_b`。
- `fastboot flash` 会改变活动槽位 ⇒ **必须紧跟 `fastboot set_active a`**。
- 自编内核的 vbmeta 未重签 ⇒ 仅在**解锁 BL** 的设备上可用。

## 上游提交规范

AOSP Android Common Kernel 原有的提交说明文档保留在
[`README.android-common-kernel.md`](README.android-common-kernel.md)（未做改动）。

## 免责声明

仅供**研究与个人设备实验**。刷写自编内核可能导致设备无法开机、数据丢失或失去保修，**风险自负**。
