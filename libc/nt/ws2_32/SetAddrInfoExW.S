#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_SetAddrInfoExW,SetAddrInfoExW,38

#ifdef __x86_64__
	.text.windows
SetAddrInfoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetAddrInfoExW(%rip),%rax
	jmp	__sysv2nt12
	.endfn	SetAddrInfoEx,globl
	.previous
#endif
