#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetExitCodeProcess,GetExitCodeProcess,0

#ifdef __x86_64__
	.text.windows
__GetExitCodeProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetExitCodeProcess(%rip),%rax
	jmp	__sysv2nt
	.endfn	__GetExitCodeProcess,globl
	.previous
#endif
