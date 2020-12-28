.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetStartupInfoW,GetStartupInfoW,0

	.text.windows
GetStartupInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetStartupInfoW(%rip)
	leave
	ret
	.endfn	GetStartupInfo,globl
	.previous
