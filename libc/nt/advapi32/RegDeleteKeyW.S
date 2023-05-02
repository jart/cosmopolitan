#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegDeleteKeyW,RegDeleteKeyW,1624

#ifdef __x86_64__
	.text.windows
RegDeleteKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteKey,globl
	.previous
#endif
