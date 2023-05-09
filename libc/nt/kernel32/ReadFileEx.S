#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ReadFileEx,ReadFileEx,0

	.text.windows
ReadFileEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadFileEx(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	ReadFileEx,globl
	.previous
