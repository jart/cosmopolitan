.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CloseHandle,CloseHandle,134

	.text.windows
CloseHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CloseHandle(%rip)
	leave
	ret
	.endfn	CloseHandle,globl
	.previous
