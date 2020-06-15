.include "o/libc/nt/ntdllimport.inc"
.ntimp	LdrUnloadDll

	.text.windows
LdrUnloadDll:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_LdrUnloadDll(%rip)
	leave
	ret
	.endfn	LdrUnloadDll,globl
	.previous
