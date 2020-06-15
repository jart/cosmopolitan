.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GlobalMemoryStatusEx,GlobalMemoryStatusEx,822

	.text.windows
GlobalMemoryStatusEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GlobalMemoryStatusEx(%rip)
	leave
	ret
	.endfn	GlobalMemoryStatusEx,globl
	.previous
