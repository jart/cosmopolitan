#include "libc/nt/codegen.h"
.imp	advapi32,__imp_CreateProcessAsUserW,CreateProcessAsUserW,0

#ifdef __x86_64__
	.text.windows
CreateProcessAsUser:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateProcessAsUserW(%rip),%rax
	jmp	__sysv2nt12
	.endfn	CreateProcessAsUser,globl
	.previous
#endif
