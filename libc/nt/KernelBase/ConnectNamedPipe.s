.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ConnectNamedPipe,ConnectNamedPipe,156

	.text.windows
ConnectNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ConnectNamedPipe(%rip),%rax
	jmp	__sysv2nt
	.endfn	ConnectNamedPipe,globl
	.previous
