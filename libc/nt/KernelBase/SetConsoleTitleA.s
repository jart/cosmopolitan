.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetConsoleTitleA,SetConsoleTitleA,1488

	.text.windows
SetConsoleTitleA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetConsoleTitleA(%rip)
	leave
	ret
	.endfn	SetConsoleTitleA,globl
	.previous
