.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreateSymbolicLinkW,CreateSymbolicLinkW,223

	.text.windows
CreateSymbolicLink:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateSymbolicLinkW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateSymbolicLink,globl
	.previous
