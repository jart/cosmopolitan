.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegFlushKey,RegFlushKey,1342

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
