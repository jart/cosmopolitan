#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateSymbolicLinkW,CreateSymbolicLinkW,0

	.text.windows
__CreateSymbolicLink:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateSymbolicLinkW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__CreateSymbolicLink,globl
	.previous
