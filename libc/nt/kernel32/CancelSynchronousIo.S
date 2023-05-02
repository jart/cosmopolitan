#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CancelSynchronousIo,CancelSynchronousIo,0

#ifdef __x86_64__
	.text.windows
CancelSynchronousIo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CancelSynchronousIo(%rip)
	leave
	ret
	.endfn	CancelSynchronousIo,globl
	.previous
#endif
