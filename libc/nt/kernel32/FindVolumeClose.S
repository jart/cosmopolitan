#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FindVolumeClose,FindVolumeClose,0

#ifdef __x86_64__
	.text.windows
FindVolumeClose:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FindVolumeClose(%rip)
	leave
	ret
	.endfn	FindVolumeClose,globl
	.previous
#endif
