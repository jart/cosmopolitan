/*bin/echo   ' -*- mode:sh; indent-tabs-mode:nil; tab-width:8; coding:utf-8 -*-│
│vi: set net ft=sh ts=2 sts=2 sw=2 fenc=utf-8                               :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚────────────────────────────────────────────────────────────────'>/dev/null #*/
dir=libc/sysv/machcalls
. libc/sysv/gen.sh

#	NeXSTEP Carnegie Melon Mach Microkernel
#	» so many context switches
#	                                              GNU/Systemd┐
# 	                                                  2.6.18+│
#	                                              Mac OS X┐  │
#	                                                 15.6+│  │
#	                                           FreeBSD┐   │  │
#	                                               12+│ ┌─│──│── XnuClass{1:Mach,2:Unix}
#	                                        OpenBSD┐  │ │ │  │
#	                                           6.4+│  │ │ │  │
#	                                      NetBSD┐  │  │ │ │  │
#	                                        9.1+│  │  │ │ │  │
#	                                           ┌┴┐┌┴┐┌┴┐│┬┴┐┌┴┐
scall	_kernelrpc_mach_vm_allocate_trap         0xfffffffff100afff	globl
scall	_kernelrpc_mach_vm_purgable_control_trap 0xfffffffff100bfff	globl
scall	_kernelrpc_mach_vm_deallocate_trap       0xfffffffff100cfff	globl
scall	_kernelrpc_mach_vm_protect_trap          0xfffffffff100efff	globl
scall	_kernelrpc_mach_vm_map_trap              0xfffffffff100ffff	globl
scall	_kernelrpc_mach_port_allocate_trap       0xfffffffff1010fff	globl
scall	_kernelrpc_mach_port_destroy_trap        0xfffffffff1011fff	globl
scall	_kernelrpc_mach_port_deallocate_trap     0xfffffffff1012fff	globl
scall	_kernelrpc_mach_port_mod_refs_trap       0xfffffffff1013fff	globl
scall	_kernelrpc_mach_port_move_member_trap    0xfffffffff1014fff	globl
scall	_kernelrpc_mach_port_insert_right_trap   0xfffffffff1015fff	globl
scall	_kernelrpc_mach_port_insert_member_trap  0xfffffffff1016fff	globl
scall	_kernelrpc_mach_port_extract_member_trap 0xfffffffff1017fff	globl
scall	_kernelrpc_mach_port_construct_trap      0xfffffffff1018fff	globl
scall	_kernelrpc_mach_port_destruct_trap       0xfffffffff1019fff	globl
scall	mach_reply_port                          0xfffffffff101afff	globl
scall	thread_self_trap                         0xfffffffff101bfff	globl
scall	task_self_trap                           0xfffffffff101cfff	globl
scall	host_self_trap                           0xfffffffff101dfff	globl
scall	mach_msg_trap                            0xfffffffff101ffff	globl
scall	mach_msg_overwrite_trap                  0xfffffffff1020fff	globl
scall	semaphore_signal_trap                    0xfffffffff1021fff	globl
scall	semaphore_signal_all_trap                0xfffffffff1022fff	globl
scall	semaphore_signal_thread_trap             0xfffffffff1023fff	globl
scall	semaphore_wait_trap                      0xfffffffff1024fff	globl
scall	semaphore_wait_signal_trap               0xfffffffff1025fff	globl
scall	semaphore_timedwait_trap                 0xfffffffff1026fff	globl
scall	semaphore_timedwait_signal_trap          0xfffffffff1027fff	globl
scall	_kernelrpc_mach_port_guard_trap          0xfffffffff1029fff	globl
scall	_kernelrpc_mach_port_unguard_trap        0xfffffffff102afff	globl
scall	mach_generate_activity_id                0xfffffffff102bfff	globl
scall	task_name_for_pid                        0xfffffffff102cfff	globl
scall	task_for_pid                             0xfffffffff102dfff	globl
scall	pid_for_task                             0xfffffffff102efff	globl
scall	macx_swapon                              0xfffffffff1030fff	globl
scall	macx_swapoff                             0xfffffffff1031fff	globl
scall	thread_get_special_reply_port            0xfffffffff1032fff	globl
scall	macx_triggers                            0xfffffffff1033fff	globl
scall	macx_backing_store_suspend               0xfffffffff1034fff	globl
scall	macx_backing_store_recovery              0xfffffffff1035fff	globl
scall	pfz_exit                                 0xfffffffff103afff	globl
scall	swtch_pri                                0xfffffffff103bfff	globl
scall	swtch                                    0xfffffffff103cfff	globl
scall	thread_switch                            0xfffffffff103dfff	globl
scall	clock_sleep_trap                         0xfffffffff103efff	globl
scall	host_create_mach_voucher_trap            0xfffffffff1046fff	globl
scall	mach_voucher_extract_attr_recipe_trap    0xfffffffff1048fff	globl
scall	mach_timebase_info_trap                  0xfffffffff1059fff	globl
scall	mach_wait_until_trap                     0xfffffffff105afff	globl
scall	mk_timer_create_trap                     0xfffffffff105bfff	globl
scall	mk_timer_destroy_trap                    0xfffffffff105cfff	globl
scall	mk_timer_arm_trap                        0xfffffffff105dfff	globl
scall	mk_timer_cancel_trap                     0xfffffffff105efff	globl
scall	mk_timer_arm_leeway_trap                 0xfffffffff105ffff	globl
scall	iokit_user_client_trap                   0xfffffffff1064fff	globl
