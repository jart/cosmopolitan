#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateDirectoryObject,NtCreateDirectoryObject

	.text.windows
	.ftrace1
NtCreateDirectoryObject:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtCreateDirectoryObject(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCreateDirectoryObject,globl
	.previous
