.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_DebugActiveProcess,DebugActiveProcess,0

	.text.windows
DebugActiveProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DebugActiveProcess(%rip)
	leave
	ret
	.endfn	DebugActiveProcess,globl
	.previous
