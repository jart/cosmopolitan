#include "libc/nt/codegen.h"
.imp	comdlg32,__imp_GetSaveFileNameW,GetSaveFileNameW,116

#ifdef __x86_64__
	.text.windows
GetSaveFileName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetSaveFileNameW(%rip)
	leave
	ret
	.endfn	GetSaveFileName,globl
	.previous
#endif
