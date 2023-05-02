#include "libc/nt/codegen.h"
.imp	kernel32,__imp_TryEnterCriticalSection,TryEnterCriticalSection,0

#ifdef __x86_64__
	.text.windows
TryEnterCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_TryEnterCriticalSection(%rip)
	leave
	ret
	.endfn	TryEnterCriticalSection,globl
	.previous
#endif
