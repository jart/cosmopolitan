.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetLargestConsoleWindowSize,GetLargestConsoleWindowSize,0

	.text.windows
GetLargestConsoleWindowSize:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetLargestConsoleWindowSize(%rip)
	leave
	ret
	.endfn	GetLargestConsoleWindowSize,globl
	.previous
