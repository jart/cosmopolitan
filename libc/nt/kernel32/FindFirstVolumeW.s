.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FindFirstVolumeW,FindFirstVolumeW,0

	.text.windows
FindFirstVolume:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstVolumeW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindFirstVolume,globl
	.previous
