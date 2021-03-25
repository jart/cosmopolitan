.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_DeregisterEventSource,DeregisterEventSource,1239

	.text.windows
DeregisterEventSource:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DeregisterEventSource(%rip)
	leave
	ret
	.endfn	DeregisterEventSource,globl
	.previous
