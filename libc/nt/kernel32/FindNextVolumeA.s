.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FindNextVolumeA,FindNextVolumeA,403

	.text.windows
FindNextVolumeA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindNextVolumeA(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindNextVolumeA,globl
	.previous
