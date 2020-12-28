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
#	» so many context switches                    GNU/Systemd┐
#	                                             Mac OS X┐   │
#	                                          FreeBSD┐   │   │
#	                                      OpenBSD┐   │ ┌─│───│── XnuClass{1:Mach,2:Unix}
#	                                           ┌─┴┐┌─┴┐│┌┴┐┌─┴┐
scall	_kernelrpc_mach_vm_allocate_trap         0xffffffff100affff	globl
scall	_kernelrpc_mach_vm_purgable_control_trap 0xffffffff100bffff	globl
scall	_kernelrpc_mach_vm_deallocate_trap       0xffffffff100cffff	globl
scall	_kernelrpc_mach_vm_protect_trap          0xffffffff100effff	globl
scall	_kernelrpc_mach_vm_map_trap              0xffffffff100fffff	globl
scall	_kernelrpc_mach_port_allocate_trap       0xffffffff1010ffff	globl
scall	_kernelrpc_mach_port_destroy_trap        0xffffffff1011ffff	globl
scall	_kernelrpc_mach_port_deallocate_trap     0xffffffff1012ffff	globl
scall	_kernelrpc_mach_port_mod_refs_trap       0xffffffff1013ffff	globl
scall	_kernelrpc_mach_port_move_member_trap    0xffffffff1014ffff	globl
scall	_kernelrpc_mach_port_insert_right_trap   0xffffffff1015ffff	globl
scall	_kernelrpc_mach_port_insert_member_trap  0xffffffff1016ffff	globl
scall	_kernelrpc_mach_port_extract_member_trap 0xffffffff1017ffff	globl
scall	_kernelrpc_mach_port_construct_trap      0xffffffff1018ffff	globl
scall	_kernelrpc_mach_port_destruct_trap       0xffffffff1019ffff	globl
scall	mach_reply_port                          0xffffffff101affff	globl
scall	thread_self_trap                         0xffffffff101bffff	globl
scall	task_self_trap                           0xffffffff101cffff	globl
scall	host_self_trap                           0xffffffff101dffff	globl
scall	mach_msg_trap                            0xffffffff101fffff	globl
scall	mach_msg_overwrite_trap                  0xffffffff1020ffff	globl
scall	semaphore_signal_trap                    0xffffffff1021ffff	globl
scall	semaphore_signal_all_trap                0xffffffff1022ffff	globl
scall	semaphore_signal_thread_trap             0xffffffff1023ffff	globl
scall	semaphore_wait_trap                      0xffffffff1024ffff	globl
scall	semaphore_wait_signal_trap               0xffffffff1025ffff	globl
scall	semaphore_timedwait_trap                 0xffffffff1026ffff	globl
scall	semaphore_timedwait_signal_trap          0xffffffff1027ffff	globl
scall	_kernelrpc_mach_port_guard_trap          0xffffffff1029ffff	globl
scall	_kernelrpc_mach_port_unguard_trap        0xffffffff102affff	globl
scall	mach_generate_activity_id                0xffffffff102bffff	globl
scall	task_name_for_pid                        0xffffffff102cffff	globl
scall	task_for_pid                             0xffffffff102dffff	globl
scall	pid_for_task                             0xffffffff102effff	globl
scall	macx_swapon                              0xffffffff1030ffff	globl
scall	macx_swapoff                             0xffffffff1031ffff	globl
scall	thread_get_special_reply_port            0xffffffff1032ffff	globl
scall	macx_triggers                            0xffffffff1033ffff	globl
scall	macx_backing_store_suspend               0xffffffff1034ffff	globl
scall	macx_backing_store_recovery              0xffffffff1035ffff	globl
scall	pfz_exit                                 0xffffffff103affff	globl
scall	swtch_pri                                0xffffffff103bffff	globl
scall	swtch                                    0xffffffff103cffff	globl
scall	thread_switch                            0xffffffff103dffff	globl
scall	clock_sleep_trap                         0xffffffff103effff	globl
scall	host_create_mach_voucher_trap            0xffffffff1046ffff	globl
scall	mach_voucher_extract_attr_recipe_trap    0xffffffff1048ffff	globl
scall	mach_timebase_info_trap                  0xffffffff1059ffff	globl
scall	mach_wait_until_trap                     0xffffffff105affff	globl
scall	mk_timer_create_trap                     0xffffffff105bffff	globl
scall	mk_timer_destroy_trap                    0xffffffff105cffff	globl
scall	mk_timer_arm_trap                        0xffffffff105dffff	globl
scall	mk_timer_cancel_trap                     0xffffffff105effff	globl
scall	mk_timer_arm_leeway_trap                 0xffffffff105fffff	globl
scall	iokit_user_client_trap                   0xffffffff1064ffff	globl
