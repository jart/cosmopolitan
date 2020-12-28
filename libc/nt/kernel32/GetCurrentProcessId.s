.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetCurrentProcessId,GetCurrentProcessId,0

	.text.windows
GetCurrentProcessId:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCurrentProcessId(%rip)
	leave
	ret
	.endfn	GetCurrentProcessId,globl
	.previous
