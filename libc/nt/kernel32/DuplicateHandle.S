#include "libc/nt/codegen.h"
.imp	kernel32,__imp_DuplicateHandle,DuplicateHandle,0

	.text.windows
DuplicateHandle:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DuplicateHandle(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	DuplicateHandle,globl
	.previous
