.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegCloseKey,RegCloseKey,0

	.text.windows
RegCloseKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegCloseKey(%rip)
	leave
	ret
	.endfn	RegCloseKey,globl
	.previous
