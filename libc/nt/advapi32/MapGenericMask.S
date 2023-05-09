#include "libc/nt/codegen.h"
.imp	advapi32,__imp_MapGenericMask,MapGenericMask,0

	.text.windows
MapGenericMask:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MapGenericMask(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	MapGenericMask,globl
	.previous
