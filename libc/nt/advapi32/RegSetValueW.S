#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegSetValueW,RegSetValueW,1683

#ifdef __x86_64__
	.text.windows
RegSetValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSetValueW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegSetValue,globl
	.previous
#endif
