.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_DebugActiveProcessStop,DebugActiveProcessStop,240

	.text.windows
DebugActiveProcessStop:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DebugActiveProcessStop(%rip)
	leave
	ret
	.endfn	DebugActiveProcessStop,globl
	.previous
