.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetLastError,SetLastError,1336

	.text.windows
SetLastError:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetLastError(%rip)
	leave
	ret
	.endfn	SetLastError,globl
	.previous
