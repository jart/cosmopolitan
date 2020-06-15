.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FindFirstVolumeA,FindFirstVolumeA,392

	.text.windows
FindFirstVolumeA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstVolumeA(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindFirstVolumeA,globl
	.previous
