# gt5pro-kernel-src · 自编内核源码

**一句话**：真我 GT5 Pro（RMX3888 / SM8650 / ColorOS 16）自编内核的**完整源码**，GPL-2.0。

> 作者：**github@ltcdz5** ／ **酷安@天玑1100逆天功耗** ｜ 由 DeepSeek 协助整理

## ★本仓库当前状态（2026-10-17）★

| 项 | 值 |
|---|---|
| 默认分支 `main` | `3421d434359288b36bd289057e2dbbad1de192b3` —— **opt162 的减法快照** |
| `history` 分支 | `6f3c94da465d5b46913746545acdfb8b5e065e56`（逐版补丁序列） |
| 唯一 tag | `v1.1-opt60` |
| 器件现役内核 | `6.1.141-android14-11-o-ltcdz5-v1.1-opt168` |

★★重要：**本仓库自 2026-10-17 起只做文字说明，不再推送新的源码快照。**★★
`main` 停在 opt162、而器件现役是 opt168，**这里看到的内核源码比实际在跑的落后若干版**。
要看某个版本的确切源码，以 `history` 分支的补丁序列为准（保留原始 SHA，可逐条核对、可 `git am` 复现）。

★另：内核态 hmbird 调度类 / 风驰游戏调度这条方向**自 2026-10-17 暂缓**
（放弃原因：工程量大、难度高）。收尾结论与全部实测证据见 **gt5pro-hmbird** 的
`docs/opt163-opt172-收尾结论与暂缓-20261017.md`。★

## 这是什么

- 这里放的是**内核源码本身**，不是刷机包。要刷机的镜像与模块在下面两个仓库里。
- 公开它是**履行 GPL 的对应源码义务**，也给想自己编译的人一个起点。

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

★**编出 `Image` 还不能刷**★ —— 出片链路里还有一步**CRC 定点覆写**：
`genksyms` 对具名结构体变量只哈希类型名，因此 `iso_masks` / `task_is_scx` / `sk_filter_trim_cap`
三个符号必须**覆写成厂商 `.ko` 的 `__versions` 期望值**（工具 `patch_crc_targeted.py`）。
★漏跑时构建 `rc=0`、无任何报错，唯一症状是设备侧 `oplus_bsp_sched_ext.ko` 静默拒载（`scx` 消失）。★
完整出片链路与四道 CRC 门禁见 **gt5pro-kernel-kit**（README 与 `tools/`）。

## 改了什么、不含什么

- **改过的**：设备树兼容层、构建配置、版本串标识、风驰（scx/hmbird）接入层等
- **不含的**：任何厂商二进制；第三方代码只作引用与致谢（来源逐条写在 `NOTICE.md`）

> 说明：本仓库的 `README.md` 已被这份项目说明替换；内核原本的 README 见顶层 `README` 文件。

## 相关仓库

- 工具箱与全部过程档案：**gt5pro-kernel-kit**
- 风驰项目说明与模块下载：**gt5pro-hmbird**

## 许可

GPL-2.0