#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegDeleteKeyW,RegDeleteKeyW,1624

	.text.windows
RegDeleteKey:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteKeyW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegDeleteKey,globl
	.previous
