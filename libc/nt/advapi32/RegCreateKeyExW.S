#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegCreateKeyExW,RegCreateKeyExW,0

#ifdef __x86_64__
	.text.windows
RegCreateKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegCreateKeyExW(%rip),%rax
	jmp	__sysv2nt10
	.endfn	RegCreateKeyEx,globl
	.previous
#endif
