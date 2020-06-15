.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_getsockopt,getsockopt,7

	.text.windows
__getsockopt$nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_getsockopt(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__getsockopt$nt,globl
	.previous
