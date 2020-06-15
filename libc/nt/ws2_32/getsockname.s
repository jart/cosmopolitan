.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_getsockname,getsockname,6

	.text.windows
__getsockname$nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_getsockname(%rip),%rax
	jmp	__sysv2nt
	.endfn	__getsockname$nt,globl
	.previous
