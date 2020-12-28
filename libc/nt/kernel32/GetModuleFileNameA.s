.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetModuleFileNameA,GetModuleFileNameA,0

	.text.windows
GetModuleFileNameA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetModuleFileNameA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetModuleFileNameA,globl
	.previous
