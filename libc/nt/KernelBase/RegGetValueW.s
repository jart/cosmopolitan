.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegGetValueW,RegGetValueW,1345

	.text.windows
RegGetValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegGetValueW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegGetValue,globl
	.previous
