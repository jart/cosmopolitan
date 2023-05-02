#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_GetAddrInfoExW,GetAddrInfoExW,31

#ifdef __x86_64__
	.text.windows
GetAddrInfoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAddrInfoExW(%rip),%rax
	jmp	__sysv2nt10
	.endfn	GetAddrInfoEx,globl
	.previous
#endif
