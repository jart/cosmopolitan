.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReleaseMutex,ReleaseMutex,1391

	.text.windows
ReleaseMutex:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ReleaseMutex(%rip)
	leave
	ret
	.endfn	ReleaseMutex,globl
	.previous
