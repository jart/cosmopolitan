.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FindNextFileW,FindNextFileW,376

	.text.windows
FindNextFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindNextFileW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindNextFile,globl
	.previous
