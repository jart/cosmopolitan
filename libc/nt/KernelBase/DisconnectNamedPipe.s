.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_DisconnectNamedPipe,DisconnectNamedPipe,270

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
