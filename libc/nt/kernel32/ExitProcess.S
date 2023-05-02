#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ExitProcess,ExitProcess,0

#ifdef __x86_64__
	.text.windows
ExitProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ExitProcess(%rip)
	leave
	ret
	.endfn	ExitProcess,globl
	.previous
#endif
