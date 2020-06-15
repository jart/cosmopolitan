.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegDisablePredefinedCache,RegDisablePredefinedCache,1629

	.text.windows
RegDisablePredefinedCache:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegDisablePredefinedCache(%rip)
	leave
	ret
	.endfn	RegDisablePredefinedCache,globl
	.previous
