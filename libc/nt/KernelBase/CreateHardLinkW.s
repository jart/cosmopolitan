.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreateHardLinkW,CreateHardLinkW,194

	.text.windows
CreateHardLink:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateHardLinkW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateHardLink,globl
	.previous
