#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FindFirstVolumeW,FindFirstVolumeW,0

#ifdef __x86_64__
	.text.windows
FindFirstVolume:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstVolumeW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindFirstVolume,globl
	.previous
#endif
