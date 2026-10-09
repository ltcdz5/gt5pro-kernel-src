# 本项目说明（PROJECT）

本仓库是 **真我 GT5 Pro（RMX3888 / SM8650 / ColorOS 16，6.1.141 OKI 基线）** 自编内核的源码树。

> 作者：**github@ltcdz5** ／ **酷安@天玑1100逆天功耗** ｜ 内容由 **DeepSeek** 协助整理

顶层的 `README` 与 `README.md` 是 **AOSP/GKI 内核自带**的，请以本文件为准了解本项目的改动。

## 基线

- 基线来自 AOSP common 内核（`android14-6.1` 系）与机型厂商提供的公开源码（remote `origin`）
- 设备要求：内核基线必须是 **OPPO 系（OKI）** 命名，如 `6.1.141-android14-11-o-...`；设备树 hmbird 类型为 `HMBIRD_OGKI`

## 本仓库的改动（只列自己的）

- 设备树 `version_type` 兼容层（缺失时安全降级，不改变任何默认行为）
- 版本串标识：`scripts/setlocalversion` 中的短署名标签
- 构建配置片段与内核补丁：`001-lz4.patch`、`002-zstd.patch` 等

## 不含什么（重要）

- 本仓库**不包含任何厂商二进制、也不包含无明确许可证的第三方代码**
- 与厂商模块相关的符号/导出对齐，只以**数字与结论**的形式记录在工具仓库 `gt5pro-kernel-kit`

## 怎么构建

~~~bash
make -j16 LLVM=1 ARCH=arm64 \
  CROSS_COMPILE=aarch64-linux-gnu- CROSS_COMPILE_ARM32=arm-linux-gnueabihf- \
  CC="ccache clang" LD=ld.lld HOSTCC=clang HOSTLD=ld.lld \
  O=out KCFLAGS+=-O2 KCFLAGS+=-Wno-error gki_defconfig all
~~~

出片后请到工具仓库跑三道关卡（厂商模块审计 / 导出遮蔽 / 拒载检查）。

## 关联仓库

| 仓库 | 作用 |
|---|---|
| gt5pro-kernel-kit | 工具箱与全部过程档案（三道关卡、CHANGELOG、风驰全过程）|
| gt5pro-hmbird | 风驰（scx/hmbird）移植记录与用户态方案说明 |

## 许可

GPL-2.0（与内核一致）。个人自编自用，仅供交流学习。