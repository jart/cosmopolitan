.include "o/libc/nt/ntdllimport.inc"
.ntimp	LdrLoadDll

	.text.windows
LdrLoadDll:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LdrLoadDll(%rip),%rax
	jmp	__sysv2nt
	.endfn	LdrLoadDll,globl
	.previous
