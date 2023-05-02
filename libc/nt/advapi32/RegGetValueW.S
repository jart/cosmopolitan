#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegGetValueW,RegGetValueW,0

#ifdef __x86_64__
	.text.windows
RegGetValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegGetValueW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegGetValue,globl
	.previous
#endif
