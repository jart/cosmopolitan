.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegGetKeySecurity,RegGetKeySecurity,1343

	.text.windows
RegGetKeySecurity:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegGetKeySecurity(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegGetKeySecurity,globl
	.previous
