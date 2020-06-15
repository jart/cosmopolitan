.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_PulseEvent,PulseEvent,1250

	.text.windows
PulseEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_PulseEvent(%rip)
	leave
	ret
	.endfn	PulseEvent,globl
	.previous
