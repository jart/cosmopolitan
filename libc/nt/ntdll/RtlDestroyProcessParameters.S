#include "libc/nt/ntdllimport.h"
.ntimp	RtlDestroyProcessParameters,RtlDestroyProcessParameters

#ifdef __x86_64__
	.text.windows
RtlDestroyProcessParameters:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlDestroyProcessParameters(%rip)
	leave
	ret
	.endfn	RtlDestroyProcessParameters,globl
	.previous
#endif
