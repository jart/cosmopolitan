.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_QueryPerformanceCounter,QueryPerformanceCounter,1098

	.text.windows
QueryPerformanceCounter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_QueryPerformanceCounter(%rip)
	leave
	ret
	.endfn	QueryPerformanceCounter,globl
	.previous
