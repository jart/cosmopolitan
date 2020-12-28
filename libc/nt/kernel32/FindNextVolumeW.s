.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FindNextVolumeW,FindNextVolumeW,0

	.text.windows
FindNextVolume:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindNextVolumeW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindNextVolume,globl
	.previous
