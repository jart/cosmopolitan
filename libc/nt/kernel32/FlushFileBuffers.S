#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FlushFileBuffers,FlushFileBuffers,0

	.text.windows
__FlushFileBuffers:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FlushFileBuffers(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	__FlushFileBuffers,globl
	.previous
