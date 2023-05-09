#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FindClose,FindClose,0

	.text.windows
__FindClose:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FindClose(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	__FindClose,globl
	.previous
