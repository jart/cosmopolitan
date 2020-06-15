.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FlushViewOfFile,FlushViewOfFile,392

	.text.windows
FlushViewOfFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FlushViewOfFile(%rip),%rax
	jmp	__sysv2nt
	.endfn	FlushViewOfFile,globl
	.previous
