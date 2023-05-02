#include "libc/nt/codegen.h"
.imp	advapi32,__imp_MapGenericMask,MapGenericMask,0

#ifdef __x86_64__
	.text.windows
MapGenericMask:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MapGenericMask(%rip),%rax
	jmp	__sysv2nt
	.endfn	MapGenericMask,globl
	.previous
#endif
