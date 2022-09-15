.include "o/libc/nt/codegen.inc"
.imp	API-MS-Win-Core-Synch-l1-2-0,__imp_WakeByAddressAll,WakeByAddressAll,113

	.text.windows
WakeByAddressAll:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WakeByAddressAll(%rip)
	leave
	ret
	.endfn	WakeByAddressAll,globl
	.previous
