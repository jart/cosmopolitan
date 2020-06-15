.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetCurrentProcess,GetCurrentProcess,500

	.text.windows
GetCurrentProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCurrentProcess(%rip)
	leave
	ret
	.endfn	GetCurrentProcess,globl
	.previous
