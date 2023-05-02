#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FormatMessageW,FormatMessageW,0

#ifdef __x86_64__
	.text.windows
FormatMessage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FormatMessageW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	FormatMessage,globl
	.previous
#endif
