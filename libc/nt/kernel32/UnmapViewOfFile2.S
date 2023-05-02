#include "libc/nt/codegen.h"
.imp	kernel32,__imp_UnmapViewOfFile2,UnmapViewOfFile2,0

#ifdef __x86_64__
	.text.windows
UnmapViewOfFile2:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnmapViewOfFile2(%rip),%rax
	jmp	__sysv2nt
	.endfn	UnmapViewOfFile2,globl
	.previous
#endif
