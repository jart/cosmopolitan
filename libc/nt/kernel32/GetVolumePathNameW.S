#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetVolumePathNameW,GetVolumePathNameW,0

	.text.windows
GetVolumePathName:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetVolumePathNameW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetVolumePathName,globl
	.previous
