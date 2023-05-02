#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegQueryValueW,RegQueryValueW,1667

#ifdef __x86_64__
	.text.windows
RegQueryValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryValueW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegQueryValue,globl
	.previous
#endif
