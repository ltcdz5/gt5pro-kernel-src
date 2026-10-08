<!-- badges -->
[![Device](https://img.shields.io/badge/Device-Realme%20GT5%20Pro%20(RMX3888)-orange.svg)]()
[![SoC](https://img.shields.io/badge/SoC-Snapdragon%208%20Gen%203%20(SM8650)-0a7bbb.svg)]()
[![Kernel](https://img.shields.io/badge/Kernel-6.1.141%20OKI-f6a500.svg)]()
[![License](https://img.shields.io/badge/License-GPL--2.0-blue.svg)](LICENSE)
[![AK3](https://img.shields.io/badge/AnyKernel3-Ready-3ddc84.svg)]()

# GT5 Pro 自编内核（RMX3888 / SM8650 / 6.1.141 OKI）

> 真我 GT5 Pro（pineapple / RMX3888，Snapdragon 8 Gen 3）的自编内核**源码快照**仓库。
> 基于 OPPO 官方开源 **6.1.141 OKI** 树 + **ACK（android14-6.1）回移与修复**，实机日常使用。

## 现役版本

| 项 | 值 |
|---|---|
| 版本串 | `6.1.141-android14-11-o-ltcdz5-v1.1-opt49` |
| 现役镜像 | `boot-v1.1-opt49-crc-repacked.img`（md5 `c40ee988904f2ea29720b0100b0ad124`）|
| **AK3 包** | `GT5Pro-RMX3888-v1.1-opt49-crc-AK3.zip`（md5 `13966f4df5e14c6289b4aff60b4380d9`，**含附加模块自动安装**）|
| 源码快照 | 分支 `opt48`（opt49 的快照待观察期结束后推）|
| 回退首选 | `boot-v1.1-opt49-repacked.img` / `boot-v1.1-opt48-repacked.img` |
| 台账（权威） | 工具仓库 [gt5pro-kernel-kit](https://github.com/ltcdz5/gt5pro-kernel-kit) 的 `CHANGELOG.md` |

## 特性

- 🔧 **ACK 回移**：android14-6.1 LTS 安全修复（f2fs 死锁、i2c 适配器注册竞态、rpmsg char UAF、arm64 fault、pKVM pvmfw …）
- 🗜️ **存储**：f2fs 修复 + 合并 IPU 写提交补漏；zstd / lz4
- 🌐 **网络**：BBR / BRUTAL / VEGAS 等拥塞控制、fq / fq_codel / cake、WireGuard、TPROXY、DNS 解析器
- 🧠 **内存**：MGLRU（多代 LRU）+ PSI + zram/hybridswap 兼容
- 🎛️ **调度**：`sched_ext` 已编入、WALT/uag 原厂调度保留（**不做** BORE/BMQ 替换）
- 📦 **文件系统扩展（opt49）**：NTFS3、SQUASHFS、CIFS（内置）
- 🔊 **蓝牙修复（opt49-crc）**：定点对齐 `sk_filter_trim_cap` 的 modversions CRC，厂商 `bluetooth.ko` 全栈恢复
- ⛔ **不做**：内置 Root（KernelSU/SUSFS）、超频、调度替换

## 刷写

### 方式 A · AnyKernel3（推荐）
下载 `GT5Pro-RMX3888-v1.1-opt49-crc-AK3.zip`，在 **KernelSU / Magisk 管理器**里直接刷：
- 自动把 `Image` 写进 boot 分区（保留 ramdisk / root）
- **自动安装附加模块**：`horae_once`（相机对焦用的 horae 常驻）、`quiet_logs`（压制原厂 HAL 日志刷屏）

### 方式 B · fastboot（只刷 boot_a）
```sh
fastboot flash boot_a boot-v1.1-opt49-crc-repacked.img
fastboot set_active a        # 必须：fastboot flash 会切到 b 槽
fastboot reboot
```
⚠️ 只刷 `boot_a`；刷完确认 `getprop ro.boot.slot_suffix` = `_a`。

## 构建

```sh
make -j$(nproc) LLVM=1 ARCH=arm64 \
  CROSS_COMPILE=aarch64-linux-gnu- CROSS_COMPILE_ARM32=arm-linux-gnueabihf- \
  CC="ccache clang" LD=ld.lld HOSTCC=clang HOSTLD=ld.lld O=out \
  KCFLAGS+=-O2 KCFLAGS+=-Wno-error gki_defconfig all
```
> 构建后必须跑一次 `tools/patch_crc_sk_filter_trim_cap.py`（工具仓库内），否则厂商蓝牙模块会因 CRC 不符被拒载。

## 分支

| 分支 | 内容 |
|---|---|
| `main` | 说明与当前指向 |
| `opt48` | v1.1-opt48 源码快照（现役源码基线）|
| `opt47` / `opt42` | 历史快照 |

## 归属与免责

- 许可 **GPL-2.0**；内核版权归各自作者（kernel.org、Qualcomm、OPPO/realme 开源）
- AnyKernel3 模板：**osm0sis** @ xda-developers；社区参考：**cctv18**/oppo_oplus_realme_sm8650、OnePlusOSS、LineageOS
- 刷机有风险，**责任自负**；本项目为个人自用与学习。

*最近更新：2026-10-08*
