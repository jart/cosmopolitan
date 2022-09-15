.include "o/libc/nt/codegen.inc"
.imp	API-MS-Win-Core-Synch-l1-2-0,__imp_WaitOnAddress,WaitOnAddress,111

	.text.windows
WaitOnAddress:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitOnAddress(%rip),%rax
	jmp	__sysv2nt
	.endfn	WaitOnAddress,globl
	.previous
