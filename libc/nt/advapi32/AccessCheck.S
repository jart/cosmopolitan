#include "libc/nt/codegen.h"
.imp	advapi32,__imp_AccessCheck,AccessCheck,0

#ifdef __x86_64__
	.text.windows
AccessCheck:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AccessCheck(%rip),%rax
	jmp	__sysv2nt8
	.endfn	AccessCheck,globl
	.previous
#endif
