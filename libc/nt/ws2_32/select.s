.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_select,select,18

	.text.windows
__select$nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_select(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__select$nt,globl
	.previous
