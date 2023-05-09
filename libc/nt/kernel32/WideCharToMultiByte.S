#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WideCharToMultiByte,WideCharToMultiByte,1553

	.text.windows
WideCharToMultiByte:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WideCharToMultiByte(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WideCharToMultiByte,globl
	.previous
