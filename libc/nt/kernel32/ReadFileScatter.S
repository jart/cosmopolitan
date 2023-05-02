#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ReadFileScatter,ReadFileScatter,0

#ifdef __x86_64__
	.text.windows
ReadFileScatter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadFileScatter(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadFileScatter,globl
	.previous
#endif
