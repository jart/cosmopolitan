#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WriteFile,WriteFile,0

#ifdef __x86_64__
	.text.windows
WriteFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteFile,globl
	.previous
#endif
