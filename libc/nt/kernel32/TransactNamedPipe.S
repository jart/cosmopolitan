#include "libc/nt/codegen.h"
.imp	kernel32,__imp_TransactNamedPipe,TransactNamedPipe,0

#ifdef __x86_64__
	.text.windows
TransactNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TransactNamedPipe(%rip),%rax
	jmp	__sysv2nt8
	.endfn	TransactNamedPipe,globl
	.previous
#endif
