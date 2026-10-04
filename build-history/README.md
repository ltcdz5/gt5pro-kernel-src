# 构建历史补丁序列（opt5 → v1.1-opt47）

本目录是 **38 个真实构建提交**的原样补丁（git format-patch 导出），每个补丁头部保留
**原始提交 SHA / 作者 / 日期 / 提交信息原文**，可与工具台账仓库的 CHANGELOG.md「提交」列逐一核对。

## 为什么是补丁，而不是 git 提交历史

本机内核树是**浅克隆**（.git/shallow 有 9 个浅边界提交，含 7a244ff18620 那次 Revert、
ebdd1643cfdb 的 Oplus 同步），把提交直接推上来会被 GitHub 拒绝：

    remote: fatal: did not receive expected object a224a9d8ee062fb81018a44ab1df4964ab379dd8
    error: remote unpack failed: index-pack failed
    ! [remote rejected]  v1.1-opt42 -> history (failed)

而把 33 个提交重建成自洽 lineage 会让 **每一个 SHA 都变**，台账里引用的提交号就对不上了。
⇒ 选择**不篡改 SHA** 的补丁序列作为审计载体。

## 怎么用

- **看某版改了什么**：直接打开对应补丁（文件名前缀 = 提交顺序，与 CHANGELOG 行序一致）
- **复现**：在 6.1.141 OKI 基线或 opt5-state 上 `git am` 这一串补丁
- **核 CVE**：按补丁里的 hunk 与上游提交逐行比对
- **核闸门**：补丁只改函数体的，vmlinux.symvers 应当逐字节不变

## 索引（补丁 ↔ 版本 ↔ 原始提交）

| # | 补丁文件 | 原始提交 | 提交标题 |
|---|---|---|---|
| 1 | 0001-opt7-clean-builder-config_fix-IP6_NF_NAT-gki_defconf.patch | d56788d59a1f | opt7-clean: 拆除 builder 注入的 config_fix(不再谎报 IP6_NF_NAT) + gki_defconfig 去重 + HEADERS_INSTALL 对齐原厂 |
| 2 | 0002-opt9-opt7-clean-Oplus-ebdd1643c-13-12-blk-mq.c-EXPOR.patch | 206914f9cc64 | opt9 = opt7-clean + Oplus ebdd1643c 同步的 13 文件子集(闭包剔除 12 项含 blk-mq.c 的 EXPORT) + 后缀 -opt9 |
| 3 | 0003-opt10-opt9-stable-6.1.142.145-10-.c-opt10.patch | 495c039ac7ea | opt10 = opt9 + stable 6.1.142..145 中自洽落地的 10 个纯 .c 修复 + 后缀 -opt10 |
| 4 | 0004-opt11-opt10-stable-6.1.146.150-6-.c-opt11.patch | 4faf90660e27 | opt11 = opt10 + stable 6.1.146..150 中真参与本机编译的 6 个 .c 修复 + 后缀 -opt11 |
| 5 | 0005-opt12-opt11-stable-6.1.151.188-116-.c-opt12.patch | bbe1b4de1eba | opt12 = opt11 + stable 6.1.151..188 中真参与本机编译的 116 个 .c 修复 + 后缀 -opt12 |
| 6 | 0006-opt12-clean-28-20-.ko-boot_a-8.patch | d9a3e7eab9b6 | opt12-clean: 退掉 28 个不进本机内核镜像的白改文件（20 个编成 .ko 刷 boot_a 带不走 + 8 个本机不编） |
| 7 | 0007-opt13-opt12-d9a3e7eab-UBSAN-INIT_ON_ALLOC_DEFAULT_ON.patch | 4e03a7d25409 | opt13 = opt12(d9a3e7eab) + 减脂三项(UBSAN 全关 / INIT_ON_ALLOC_DEFAULT_ON 关 / KFENCE 采样间隔 0)，后缀 -opt13；config 差 18 行全部属这三项及其依赖，导出符号基准 15388 个不变 |
| 8 | 0008-opt14-opt13-4e03a7d25-ACK-android14-6.1-lts-68-mm-33.patch | bc26f54a57db | opt14 = opt13(4e03a7d25) + ACK android14-6.1-lts 68 条提交(mm 33 / f2fs 13 / sched 4 / block 4 / erofs 2 / 头 10 等)，取自权威源 android.googlesource.com（此前误用冻结的 GitHub 镜像得出'上游封顶'） |
| 9 | 0009-opt14-3-5b2aec77f922-53a8b297ba46-hugetlb-struct-pag.patch | 93a1a8f94871 | opt14 退料: 退掉 3 条假设前置不在本基线的补丁 —— 5b2aec77f922/53a8b297ba46(hugetlb 用 struct page.pt_share_count, 本树无此字段) + c21a9e0e4ef4(block/fops.c 调 kiocb_invalidate_pages 等, 本树无此助手) |
| 10 | 0010-opt14-opt13-opt14-scripts-setlocalversion.patch | 88488d6a5c69 | opt14: 版本后缀 opt13 -> opt14 (scripts/setlocalversion) |
| 11 | 0011-opt14-trim-23-.o.cmd-DAMON-kunit-11-secretmem-kmsan-.patch | 05480beb0c27 | opt14-trim: 退掉 23 条不进本机镜像的白改(判据=编译器 .o.cmd 依赖集; 含 DAMON kunit 测试头 11 条 + secretmem/kmsan/kasan-init/hugetlb/memory-failure/rnbd/damon 等)。使改动集与效果集对齐。 |
| 12 | 0012-opt15-ACK-138-166-28-modversions-CRC-5.patch | 234e0260ea0c | opt15: ACK 第二批 138 条(166 中退 28 条 modversions CRC 风险 + 5 条缺前置) |
| 13 | 0013-opt15-P5-P2-P4-config-gki_defconfig-PANIC_TIMEOUT-1-.patch | 1df7a34f6453 | opt15-P5: 把 P2/P4 的 config 增量录进 gki_defconfig, 使构建可复现; PANIC_TIMEOUT -1 -> 30 |
| 14 | 0014-opt15-P11-round2-104-CONFLICT-6.patch | e55a82a0cee3 | opt15-P11: 从 round2 遗留的 104 条 CONFLICT 中捞出 6 条自洽安全修复 |
| 15 | 0015-opt15-P13-stable-6.1.142.188-6-.c.patch | d4fe9dd3640e | opt15-P13: 补上 stable 6.1.142..188 中本机参与编译却被跳过的 6 条纯 .c |
| 16 | 0016-opt15-P16-P13-proc-loadavg-loadavg.c-int-cast.patch | a54a45ae5455 | opt15-P16: 修掉 P13 引入的 /proc/loadavg 爆表 —— 退回 loadavg.c 的 (int) cast |
| 17 | 0017-opt15-P19-ACK-round3-android14-6.1-104-26.patch | 14f326fc160d | opt15-P19: ACK round3 —— 从 android14-6.1 收 104 个缺失文件块 (26 个提交的真修复) |
| 18 | 0018-opt15-P20-945be0af8244-posix-cpu-timers-UAF.patch | d8e827ea8f42 | opt15-P20: 补上 945be0af8244 posix-cpu-timers UAF 修复的【生产者半边】 |
| 19 | 0019-opt15-P21-SSG-Samsung-Generic-I-O-scheduler.patch | e17ad23693ab | opt15-P21: 启用 SSG 电梯 (Samsung Generic I/O scheduler) |
| 20 | 0020-opt15-P25-ACK-round4-2026-05-20.09-29.patch | c2a84f3a54c0 | opt15-P25: ACK round4 —— 老窗口(2026-05-20..09-29) 的自洽子集 |
| 21 | 0021-opt15-P27-ACK-round4-11.patch | 3448adab80a6 | opt15-P27: ACK round4 头文件修复批 —— 11 个不碰中枢结构体的提交 |
| 22 | 0022-opt15-P28-ACK-round4-53.patch | b45c37a0c6b4 | opt15-P28: ACK round4 冲突块落地 (53 文件) |
| 23 | 0023-opt15-P30-f2fs-dic-workqueue-UAF.patch | a79799cda102 | opt15-P30: f2fs 修 dic 在 workqueue 晚释放路径上的 UAF |
| 24 | 0024-opt15-P31-f2fs-dic_layout-bootconfig-qcom_geni.patch | b96307adfd22 | opt15-P31: f2fs 解压路径改用 dic_layout + bootconfig 三修复 + qcom_geni 串口 |
| 25 | 0025-opt15-P32-f2fs-SSG.patch | e1b638d4de99 | opt15-P32: f2fs 压缩越界修复 + SSG 两处缺陷 |
| 26 | 0026-opt15-P35-version1-6.1.141-android14-11-o-ltcdz5-ver.patch | cfd8e65c0a28 | opt15-P35 = version1: 版本号规范切换到 6.1.141-android14-11-o-ltcdz5-version1-opt35 |
| 27 | 0027-version1-opt36-UFS-CVE-2026-43471.patch | febd4235f865 | version1-opt36: UFS 三处修复（含 CVE-2026-43471）+ 第一个新版本号规范的版本 |
| 28 | 0028-version1-opt37-config-UBSAN-INIT_ON_ALLOC-INIT_STACK.patch | 48e095183986 | version1-opt37: 四项 config 减法（UBSAN / INIT_ON_ALLOC / INIT_STACK / ZRAM+RCU_NOCB） |
| 29 | 0029-v1.0-opt38-netfilter-CVE.patch | 5a22d459255f | v1.0-opt38: netfilter 四项修复（三个已公开 CVE 的修复未进树）+ 版本号换新规范 |
| 30 | 0030-v1.0-opt39-ext4-jbd2-CVE-2025-38337-.c-CRC.patch | c6f3611bb39d | v1.0-opt39: ext4/jbd2 三项修复（含 CVE-2025-38337）+ 首次验证"纯 .c 不动 CRC" |
| 31 | 0031-v1.1-opt40-CVE-2026-31446-ext4-sysfs-UAF.patch | a9d0d61fbf68 | v1.1-opt40: CVE-2026-31446 —— ext4 sysfs UAF（竞态型）+ 首次通过"改结构体"的闸门 |
| 32 | 0032-v1.1-opt41-AF_PACKET-cmsg-1ee90b77b727-ad9a0374ee6d.patch | e5f8f1aa13a8 | v1.1-opt41: 修 AF_PACKET 时间戳 cmsg 越界读（上游 1ee90b77b727 / 账本 ad9a0374ee6d） |
| 33 | 0033-v1.1-opt42-USB-gadget-bRequestType-LZ4-armv8-Permtab.patch | 77aa56a8024c | v1.1-opt42: USB gadget bRequestType 位域误判 + LZ4 armv8 Permtable 越界读 |
| 34 | 0034-v1.1-opt45-T0-C-sched_ext-scx.patch | 83f9166efbd4 | v1.1-opt45 (T0/路线C)：拒绝启用 sched_ext —— 让 scx 干净失败，不再硬挂死整机 |
| 35 | 0035-v1.1-opt45-2-sched_ext_ops-BPF-struct_ops-BPF.patch | dfea5e50fd23 | v1.1-opt45（探针2）：把 sched_ext_ops 从 BPF struct_ops 类型表里摘掉 —— 在真正挂死的那一层（BPF 加载）拦截 |
| 36 | 0036-opt45-61-.rej-.orig-LZ4-Zstd-gitlink-Baseband-guard-.patch | 846c9c1ba805 | opt45 交付前清理：删除 61 个 .rej/.orig 残留（LZ4/Zstd 补丁产物）+ 摘下悬空 gitlink Baseband-guard（160000，无 .gitmodules） |
| 37 | 0037-opt45-Baseband-guard-gitlink.patch | 6d61a69692ef | opt45：把 Baseband-guard 从【悬空 gitlink】修成【合法子模块引用】 |
| 38 | 0038-v1.1-opt47-i2c-ACK-10-02.patch | 5ddf8408b29a | v1.1-opt47：i2c 适配器注册竞态 + 失败路径补漏（取自 ACK 10-02 两条） |
