#include "libc/nt/codegen.h"
.imp	kernel32,__imp_DebugActiveProcess,DebugActiveProcess,0

#ifdef __x86_64__
	.text.windows
DebugActiveProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DebugActiveProcess(%rip)
	leave
	ret
	.endfn	DebugActiveProcess,globl
	.previous
#endif
