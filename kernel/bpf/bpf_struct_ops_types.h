/* SPDX-License-Identifier: GPL-2.0 */
/* internal file - do not include directly */

#ifdef CONFIG_BPF_JIT
#ifdef CONFIG_NET
BPF_STRUCT_OPS_TYPE(bpf_dummy_ops)
#endif
#ifdef CONFIG_INET
#include <net/tcp.h>
BPF_STRUCT_OPS_TYPE(tcp_congestion_ops)
#endif
/*
 * opt45 (probe 2): sched_ext_ops is deliberately NOT registered as a BPF
 * struct_ops type on this tree.
 *
 * Loading ANY struct_ops object of this type hard-hangs the machine, and the
 * hang happens in the BPF load path -- scx_ops_enable() is never even reached
 * (verified: bare bpftool prog loadall, i.e. without registration, also hangs).
 * With the type unregistered such loads fail with a clean -ENOENT/-EINVAL
 * instead of freezing the device.
 *
 * No exported symbol changes: this tree exports 0 scx_* symbols, and
 * bpf_sched_ext_ops itself is not exported, so the vendor module CRC
 * contracts are untouched.
 */
#endif
