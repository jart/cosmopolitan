#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FlushViewOfFile,FlushViewOfFile,0

#ifdef __x86_64__
	.text.windows
__FlushViewOfFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FlushViewOfFile(%rip),%rax
	jmp	__sysv2nt
	.endfn	__FlushViewOfFile,globl
	.previous
#endif
