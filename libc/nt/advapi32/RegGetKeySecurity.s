.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegGetKeySecurity,RegGetKeySecurity,0

	.text.windows
RegGetKeySecurity:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegGetKeySecurity(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegGetKeySecurity,globl
	.previous
