.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetWindowsDirectoryW,GetWindowsDirectoryW,0

	.text.windows
GetWindowsDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindowsDirectoryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetWindowsDirectory,globl
	.previous
