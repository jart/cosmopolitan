.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFullPathNameW,GetFullPathNameW,0

	.text.windows
GetFullPathName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFullPathNameW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFullPathName,globl
	.previous
