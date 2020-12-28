.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetConsoleSelectionInfo,GetConsoleSelectionInfo,0

	.text.windows
GetConsoleSelectionInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetConsoleSelectionInfo(%rip)
	leave
	ret
	.endfn	GetConsoleSelectionInfo,globl
	.previous
