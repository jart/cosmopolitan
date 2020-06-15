.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_PeekNamedPipe,PeekNamedPipe,1205

	.text.windows
PeekNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PeekNamedPipe(%rip),%rax
	jmp	__sysv2nt6
	.endfn	PeekNamedPipe,globl
	.previous
