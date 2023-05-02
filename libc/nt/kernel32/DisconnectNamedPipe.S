#include "libc/nt/codegen.h"
.imp	kernel32,__imp_DisconnectNamedPipe,DisconnectNamedPipe,0

#ifdef __x86_64__
	.text.windows
DisconnectNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DisconnectNamedPipe(%rip)
	leave
	ret
	.endfn	DisconnectNamedPipe,globl
	.previous
#endif
