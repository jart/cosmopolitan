#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegSaveKeyW,RegSaveKeyW,1676

#ifdef __x86_64__
	.text.windows
RegSaveKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSaveKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegSaveKey,globl
	.previous
#endif
