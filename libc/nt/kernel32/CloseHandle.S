#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CloseHandle,CloseHandle,0

#ifdef __x86_64__
	.text.windows
__CloseHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CloseHandle(%rip)
	leave
	ret
	.endfn	__CloseHandle,globl
	.previous
#endif
