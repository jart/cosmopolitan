.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetWindowsDirectoryA,GetWindowsDirectoryA,0

	.text.windows
GetWindowsDirectoryA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindowsDirectoryA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetWindowsDirectoryA,globl
	.previous
