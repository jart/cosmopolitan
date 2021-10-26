.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_PeekMessageW,PeekMessageW,2201

	.text.windows
PeekMessage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PeekMessageW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	PeekMessage,globl
	.previous
