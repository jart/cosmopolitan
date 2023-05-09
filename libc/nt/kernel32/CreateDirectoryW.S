#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateDirectoryW,CreateDirectoryW,0

	.text.windows
__CreateDirectory:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateDirectoryW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__CreateDirectory,globl
	.previous
