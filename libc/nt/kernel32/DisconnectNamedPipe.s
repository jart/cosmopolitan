.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_DisconnectNamedPipe,DisconnectNamedPipe,0

	.text.windows
DisconnectNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DisconnectNamedPipe(%rip)
	leave
	ret
	.endfn	DisconnectNamedPipe,globl
	.previous
