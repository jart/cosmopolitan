.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_setsockopt,setsockopt,21

	.text.windows
__setsockopt$nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_setsockopt(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__setsockopt$nt,globl
	.previous
