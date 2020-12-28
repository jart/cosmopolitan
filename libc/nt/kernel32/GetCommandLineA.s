.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetCommandLineA,GetCommandLineA,0

	.text.windows
GetCommandLineA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCommandLineA(%rip)
	leave
	ret
	.endfn	GetCommandLineA,globl
	.previous
