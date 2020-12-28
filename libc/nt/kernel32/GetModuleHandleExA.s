.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetModuleHandleExA,GetModuleHandleExA,0

	.text.windows
GetModuleHandleExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetModuleHandleExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetModuleHandleExA,globl
	.previous
