#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegDeleteKeyExW,RegDeleteKeyExW,0

	.text.windows
RegDeleteKeyEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteKeyExW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegDeleteKeyEx,globl
	.previous
