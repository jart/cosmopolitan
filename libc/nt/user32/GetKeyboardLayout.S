#include "libc/nt/codegen.h"
.imp	user32,__imp_GetKeyboardLayout,GetKeyboardLayout,1867

#ifdef __x86_64__
	.text.windows
GetKeyboardLayout:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetKeyboardLayout(%rip)
	leave
	ret
	.endfn	GetKeyboardLayout,globl
	.previous
#endif
