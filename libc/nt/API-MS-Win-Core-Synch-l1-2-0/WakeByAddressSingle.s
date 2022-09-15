.include "o/libc/nt/codegen.inc"
.imp	API-MS-Win-Core-Synch-l1-2-0,__imp_WakeByAddressSingle,WakeByAddressSingle,116

	.text.windows
WakeByAddressSingle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WakeByAddressSingle(%rip)
	leave
	ret
	.endfn	WakeByAddressSingle,globl
	.previous
