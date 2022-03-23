.include "o/libc/nt/codegen.inc"
.imp	pdh,__imp_PdhOpenQueryW,PdhOpenQueryW,0

	.text.windows
PdhOpenQuery:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PdhOpenQueryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	PdhOpenQuery,globl
	.previous
