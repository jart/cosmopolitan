#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegSaveKeyW,RegSaveKeyW,1676

	.text.windows
RegSaveKey:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSaveKeyW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegSaveKey,globl
	.previous
