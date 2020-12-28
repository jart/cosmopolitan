.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetCommandLineW,GetCommandLineW,0

	.text.windows
GetCommandLine:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCommandLineW(%rip)
	leave
	ret
	.endfn	GetCommandLine,globl
	.previous
