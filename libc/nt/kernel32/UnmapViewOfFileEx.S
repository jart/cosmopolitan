#include "libc/nt/codegen.h"
.imp	kernel32,__imp_UnmapViewOfFileEx,UnmapViewOfFileEx,0

#ifdef __x86_64__
	.text.windows
UnmapViewOfFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnmapViewOfFileEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	UnmapViewOfFileEx,globl
	.previous
#endif
