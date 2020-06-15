.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegEnableReflectionKey,RegEnableReflectionKey,1632

	.text.windows
RegEnableReflectionKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegEnableReflectionKey(%rip)
	leave
	ret
	.endfn	RegEnableReflectionKey,globl
	.previous
