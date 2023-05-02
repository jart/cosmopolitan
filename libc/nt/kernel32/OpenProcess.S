#include "libc/nt/codegen.h"
.imp	kernel32,__imp_OpenProcess,OpenProcess,0

#ifdef __x86_64__
	.text.windows
__OpenProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_OpenProcess(%rip),%rax
	jmp	__sysv2nt
	.endfn	__OpenProcess,globl
	.previous
#endif
