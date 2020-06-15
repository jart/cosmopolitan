.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FindFirstVolumeW,FindFirstVolumeW,370

	.text.windows
FindFirstVolume:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstVolumeW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindFirstVolume,globl
	.previous
