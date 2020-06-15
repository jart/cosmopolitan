.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetLargestConsoleWindowSize,GetLargestConsoleWindowSize,580

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
