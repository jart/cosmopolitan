.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegFlushKey,RegFlushKey,0

	.text.windows
RegFlushKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegFlushKey(%rip)
	leave
	ret
	.endfn	RegFlushKey,globl
	.previous
