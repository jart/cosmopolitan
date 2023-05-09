#include "libc/nt/codegen.h"
.imp	kernel32,__imp_UnmapViewOfFile2,UnmapViewOfFile2,0

	.text.windows
UnmapViewOfFile2:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnmapViewOfFile2(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	UnmapViewOfFile2,globl
	.previous
