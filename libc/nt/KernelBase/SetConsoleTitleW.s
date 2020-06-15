.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetConsoleTitleW,SetConsoleTitleW,1489

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
