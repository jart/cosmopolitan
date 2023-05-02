#include "libc/nt/codegen.h"
.imp	kernel32,__imp_DebugActiveProcessStop,DebugActiveProcessStop,0

#ifdef __x86_64__
	.text.windows
DebugActiveProcessStop:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DebugActiveProcessStop(%rip)
	leave
	ret
	.endfn	DebugActiveProcessStop,globl
	.previous
#endif
