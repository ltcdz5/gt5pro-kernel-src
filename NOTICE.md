# 来源与许可标注（NOTICE）

## 许可
本仓库是 **Linux 内核源码**（GPL-2.0），许可证全文见仓库内 `COPYING`。
对源码的任何修改与再分发，均遵守 **GPL-2.0**。

## 上游来源（复刻/衍生说明）
| 层级 | 来源 | 许可 |
|---|---|---|
| GKI 基底 | **AOSP `kernel/common`**（`android14-6.1`，Google）<br>`https://android.googlesource.com/kernel/common`（镜像 `github.com/aosp-mirror/kernel_common`） | GPL-2.0 |
| 厂商源码 | **OPPO/oplus 官方开源** `android_kernel_common_oneplus_sm8650`（一加/OPPO/真我 SM8650 共用） | GPL-2.0 |
| 参考项目 | 社区项目 `cctv18/oppo_oplus_realme_sm8650`（构建脚本与版本命名参考） | GPL-2.0 |

## 本仓库的自有改动
在厂商源码之上应用的内核补丁与配置调整（修复、CVE 回移、config 取舍等），
逐版记录在配套仓库 **`gt5pro-kernel-kit`** 的台账文档中（含每版改动、验证结果与结论）。

## 致谢与借鉴
- Root 方案：`tiann/KernelSU`、`ReSukiSU/ReSukiSU`、`SukiSU-Ultra`
- SUSFS：`ShirkNeko/susfs4ksu`
- lz4 1.10.0 / zstd 1.5.7 补丁：`ferstar`（移植 `Xiaomichael`）
- 三星 SSG IO 调度器：社区整理
- 内核补丁筛选方法：上游 `stable-queue` / ACK 台账

## 免责声明
仅供研究与个人设备实验。刷写自编内核可能导致设备无法开机、数据丢失或失去保修，风险自负。
