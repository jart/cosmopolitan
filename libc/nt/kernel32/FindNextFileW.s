.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FindNextFileW,FindNextFileW,0

	.text.windows
FindNextFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindNextFileW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindNextFile,globl
	.previous
