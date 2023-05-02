#include "libc/nt/codegen.h"
.imp	kernel32,__imp_TlsAlloc,TlsAlloc,0

#ifdef __x86_64__
	.text.windows
TlsAlloc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_TlsAlloc(%rip)
	leave
	ret
	.endfn	TlsAlloc,globl
	.previous
#endif
