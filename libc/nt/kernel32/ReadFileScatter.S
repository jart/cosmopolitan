#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ReadFileScatter,ReadFileScatter,0

	.text.windows
ReadFileScatter:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadFileScatter(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	ReadFileScatter,globl
	.previous
