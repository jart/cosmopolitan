.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_listen,listen,13

	.text.windows
__listen$nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_listen(%rip),%rax
	jmp	__sysv2nt
	.endfn	__listen$nt,globl
	.previous
