#include "libc/nt/codegen.h"
.imp	kernel32,__imp_TlsSetValue,TlsSetValue,0

#ifdef __x86_64__
	.text.windows
TlsSetValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TlsSetValue(%rip),%rax
	jmp	__sysv2nt
	.endfn	TlsSetValue,globl
	.previous
#endif
