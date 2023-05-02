#include "libc/nt/codegen.h"
.imp	MsWSock,__imp_DisconnectEx,DisconnectEx,0

#ifdef __x86_64__
	.text.windows
DisconnectEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DisconnectEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	DisconnectEx,globl
	.previous
#endif
