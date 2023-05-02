#include "libc/nt/codegen.h"
.imp	kernel32,__imp_InitializeProcThreadAttributeList,InitializeProcThreadAttributeList,0

#ifdef __x86_64__
	.text.windows
InitializeProcThreadAttributeList:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InitializeProcThreadAttributeList(%rip),%rax
	jmp	__sysv2nt
	.endfn	InitializeProcThreadAttributeList,globl
	.previous
#endif
