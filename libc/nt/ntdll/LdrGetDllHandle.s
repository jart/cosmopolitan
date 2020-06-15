.include "o/libc/nt/ntdllimport.inc"
.ntimp	LdrGetDllHandle

	.text.windows
LdrGetDllHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LdrGetDllHandle(%rip),%rax
	jmp	__sysv2nt
	.endfn	LdrGetDllHandle,globl
	.previous
