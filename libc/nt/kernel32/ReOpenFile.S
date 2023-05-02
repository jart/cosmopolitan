#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ReOpenFile,ReOpenFile,0

#ifdef __x86_64__
	.text.windows
__ReOpenFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReOpenFile(%rip),%rax
	jmp	__sysv2nt
	.endfn	__ReOpenFile,globl
	.previous
#endif
