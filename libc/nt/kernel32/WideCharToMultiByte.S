#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WideCharToMultiByte,WideCharToMultiByte,1553

#ifdef __x86_64__
	.text.windows
WideCharToMultiByte:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WideCharToMultiByte(%rip),%rax
	jmp	__sysv2nt8
	.endfn	WideCharToMultiByte,globl
	.previous
#endif
