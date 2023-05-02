#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegOpenKeyExW,RegOpenKeyExW,0

#ifdef __x86_64__
	.text.windows
RegOpenKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOpenKeyExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegOpenKeyEx,globl
	.previous
#endif
