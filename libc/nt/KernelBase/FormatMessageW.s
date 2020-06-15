.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FormatMessageW,FormatMessageW,398

	.text.windows
FormatMessage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FormatMessageW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	FormatMessage,globl
	.previous
