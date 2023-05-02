#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CancelIo,CancelIo,0

#ifdef __x86_64__
	.text.windows
CancelIo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CancelIo(%rip)
	leave
	ret
	.endfn	CancelIo,globl
	.previous
#endif
