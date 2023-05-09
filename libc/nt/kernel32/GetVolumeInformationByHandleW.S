#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetVolumeInformationByHandleW,GetVolumeInformationByHandleW,0

	.text.windows
GetVolumeInformationByHandle:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetVolumeInformationByHandleW(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetVolumeInformationByHandle,globl
	.previous
