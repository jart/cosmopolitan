#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FindFirstFileExW,FindFirstFileExW,0

#ifdef __x86_64__
	.text.windows
FindFirstFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstFileExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	FindFirstFileEx,globl
	.previous
#endif
