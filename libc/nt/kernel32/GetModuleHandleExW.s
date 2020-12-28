.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetModuleHandleExW,GetModuleHandleExW,0

	.text.windows
GetModuleHandleEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetModuleHandleExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetModuleHandleEx,globl
	.previous
