#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FindVolumeClose,FindVolumeClose

	.text.windows
	.ftrace1
FindVolumeClose:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FindVolumeClose(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	FindVolumeClose,globl
	.previous
