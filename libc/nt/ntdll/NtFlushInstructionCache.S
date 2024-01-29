#include "libc/nt/ntdllimport.h"
.ntimp	NtFlushInstructionCache,NtFlushInstructionCache

	.text.windows
	.ftrace1
NtFlushInstructionCache:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtFlushInstructionCache(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtFlushInstructionCache,globl
	.previous
