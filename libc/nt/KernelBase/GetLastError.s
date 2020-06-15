.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetLastError,GetLastError,581

	.text.windows
GetLastError:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetLastError(%rip)
	leave
	ret
	.endfn	GetLastError,globl
	.previous
