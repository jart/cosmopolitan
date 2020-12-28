.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateSymbolicLinkA,CreateSymbolicLinkA,0

	.text.windows
CreateSymbolicLinkA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateSymbolicLinkA(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateSymbolicLinkA,globl
	.previous
