.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FormatMessageA,FormatMessageA,397

	.text.windows
FormatMessageA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FormatMessageA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	FormatMessageA,globl
	.previous
