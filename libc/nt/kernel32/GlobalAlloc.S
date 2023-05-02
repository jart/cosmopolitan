#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GlobalAlloc,GlobalAlloc,0

#ifdef __x86_64__
	.text.windows
GlobalAlloc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GlobalAlloc(%rip),%rax
	jmp	__sysv2nt
	.endfn	GlobalAlloc,globl
	.previous
#endif
