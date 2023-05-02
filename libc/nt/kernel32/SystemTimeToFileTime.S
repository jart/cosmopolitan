#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SystemTimeToFileTime,SystemTimeToFileTime,0

#ifdef __x86_64__
	.text.windows
SystemTimeToFileTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SystemTimeToFileTime(%rip),%rax
	jmp	__sysv2nt
	.endfn	SystemTimeToFileTime,globl
	.previous
#endif
