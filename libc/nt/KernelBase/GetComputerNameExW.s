.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetComputerNameExW,GetComputerNameExW,449

	.text.windows
GetComputerNameEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetComputerNameExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetComputerNameEx,globl
	.previous
