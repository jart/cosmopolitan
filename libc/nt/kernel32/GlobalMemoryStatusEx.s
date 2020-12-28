.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GlobalMemoryStatusEx,GlobalMemoryStatusEx,0

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
