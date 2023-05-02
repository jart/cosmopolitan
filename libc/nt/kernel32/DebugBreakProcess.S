#include "libc/nt/codegen.h"
.imp	kernel32,__imp_DebugBreakProcess,DebugBreakProcess,263

#ifdef __x86_64__
	.text.windows
DebugBreakProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DebugBreakProcess(%rip)
	leave
	ret
	.endfn	DebugBreakProcess,globl
	.previous
#endif
