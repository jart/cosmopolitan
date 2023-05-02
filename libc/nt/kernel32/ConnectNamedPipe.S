#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ConnectNamedPipe,ConnectNamedPipe,0

#ifdef __x86_64__
	.text.windows
ConnectNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ConnectNamedPipe(%rip),%rax
	jmp	__sysv2nt
	.endfn	ConnectNamedPipe,globl
	.previous
#endif
