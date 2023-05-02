#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FindNextVolumeW,FindNextVolumeW,0

#ifdef __x86_64__
	.text.windows
FindNextVolume:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindNextVolumeW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindNextVolume,globl
	.previous
#endif
