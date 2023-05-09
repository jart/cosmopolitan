#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetSystemDirectoryW,GetSystemDirectoryW,0

	.text.windows
GetSystemDirectory:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemDirectoryW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetSystemDirectory,globl
	.previous
