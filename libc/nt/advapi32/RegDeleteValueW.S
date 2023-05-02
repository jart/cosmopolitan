#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegDeleteValueW,RegDeleteValueW,0

#ifdef __x86_64__
	.text.windows
RegDeleteValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteValueW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteValue,globl
	.previous
#endif
