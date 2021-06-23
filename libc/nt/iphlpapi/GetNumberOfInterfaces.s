.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_GetNumberOfInterfaces,GetNumberOfInterfaces,0

	.text.windows
GetNumberOfInterfaces:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetNumberOfInterfaces(%rip)
	leave
	ret
	.endfn	GetNumberOfInterfaces,globl
	.previous
