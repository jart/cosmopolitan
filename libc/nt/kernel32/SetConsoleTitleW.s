.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetConsoleTitleW,SetConsoleTitleW,0

	.text.windows
SetConsoleTitle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetConsoleTitleW(%rip)
	leave
	ret
	.endfn	SetConsoleTitle,globl
	.previous
