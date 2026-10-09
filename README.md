# gt5pro-kernel-src · 自编内核源码

**一句话**：真我 GT5 Pro（RMX3888 / SM8650 / ColorOS 16）自编内核的**完整源码**，GPL-2.0。

> 作者：**github@ltcdz5** ／ **酷安@天玑1100逆天功耗** ｜ 由 DeepSeek 协助整理

## 这是什么

- 这里放的是**内核源码本身**，不是刷机包。要刷机的镜像与模块在下面两个仓库里。
- 公开它就是**履行 GPL 的对应源码义务**，也给想自己编译的人一个起点。

## 基线要求

- 内核基线必须是 **OPPO 系（OKI）** 命名，例如 `6.1.141-android14-11-o-...`
- 设备树里的 hmbird 类型为 `HMBIRD_OGKI`（SM8650 属于这一档）

## 怎么编译

```bash
make -j16 LLVM=1 ARCH=arm64 \
  CROSS_COMPILE=aarch64-linux-gnu- CROSS_COMPILE_ARM32=arm-linux-gnueabihf- \
  CC="ccache clang" LD=ld.lld HOSTCC=clang HOSTLD=ld.lld \
  O=out KCFLAGS+=-O2 KCFLAGS+=-Wno-error gki_defconfig all
```

出片后请到工具箱仓库跑三道关卡（厂商模块 / 导出遮蔽 / 拒载）。

## 改了什么、不含什么

- **改过的**：设备树兼容层、构建配置、版本串标识、风驰（scx/hmbird）接入层等
- **不含的**：任何厂商二进制；第三方代码只作引用与致谢（来源逐条写在 `NOTICE.md`）

> 说明：本仓库的 `README.md` 已被这份项目说明替换；内核原本的 README 见顶层 `README` 文件。

## 相关仓库

- 工具箱与全部过程档案：**gt5pro-kernel-kit**
- 风驰项目说明与模块下载：**gt5pro-hmbird**

## 许可

GPL-2.0