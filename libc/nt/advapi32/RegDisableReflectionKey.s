.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegDisableReflectionKey,RegDisableReflectionKey,1631

	.text.windows
RegDisableReflectionKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegDisableReflectionKey(%rip)
	leave
	ret
	.endfn	RegDisableReflectionKey,globl
	.previous
