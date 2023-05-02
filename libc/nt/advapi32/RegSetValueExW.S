#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegSetValueExW,RegSetValueExW,0

#ifdef __x86_64__
	.text.windows
RegSetValueEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSetValueExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegSetValueEx,globl
	.previous
#endif
