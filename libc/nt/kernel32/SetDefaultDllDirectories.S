#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetDefaultDllDirectories,SetDefaultDllDirectories,0

#ifdef __x86_64__
	.text.windows
SetDefaultDllDirectories:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetDefaultDllDirectories(%rip)
	leave
	ret
	.endfn	SetDefaultDllDirectories,globl
	.previous
#endif
