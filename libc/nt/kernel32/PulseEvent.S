#include "libc/nt/codegen.h"
.imp	kernel32,__imp_PulseEvent,PulseEvent,0

#ifdef __x86_64__
	.text.windows
PulseEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_PulseEvent(%rip)
	leave
	ret
	.endfn	PulseEvent,globl
	.previous
#endif
