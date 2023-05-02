#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetVolumeInformationByHandleW,GetVolumeInformationByHandleW,0

#ifdef __x86_64__
	.text.windows
GetVolumeInformationByHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetVolumeInformationByHandleW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	GetVolumeInformationByHandle,globl
	.previous
#endif
