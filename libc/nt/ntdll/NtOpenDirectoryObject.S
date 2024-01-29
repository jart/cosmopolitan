#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenDirectoryObject,NtOpenDirectoryObject

	.text.windows
	.ftrace1
NtOpenDirectoryObject:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtOpenDirectoryObject(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtOpenDirectoryObject,globl
	.previous
