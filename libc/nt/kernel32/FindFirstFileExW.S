#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FindFirstFileExW,FindFirstFileExW,0

	.text.windows
FindFirstFileEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstFileExW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	FindFirstFileEx,globl
	.previous
