.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FindNextVolumeW,FindNextVolumeW,378

	.text.windows
FindNextVolume:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindNextVolumeW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindNextVolume,globl
	.previous
