#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegDeleteKeyExW,RegDeleteKeyExW,0

#ifdef __x86_64__
	.text.windows
RegDeleteKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteKeyExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteKeyEx,globl
	.previous
#endif
