.include "o/libc/nt/codegen.inc"
.imp	MsWSock,__imp_DisconnectEx,DisconnectEx,0

	.text.windows
DisconnectEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DisconnectEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	DisconnectEx,globl
	.previous
