#include "libc/nt/codegen.h"
.imp	advapi32,__imp_OpenProcessToken,OpenProcessToken,0

#ifdef __x86_64__
	.text.windows
OpenProcessToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_OpenProcessToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	OpenProcessToken,globl
	.previous
#endif
