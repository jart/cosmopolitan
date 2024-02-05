#include "libc/nt/ntdllimport.h"
.ntimp	RtlNtStatusToDosError,RtlNtStatusToDosError

	.text.windows
	.ftrace1
RtlNtStatusToDosError:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlNtStatusToDosError(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	RtlNtStatusToDosError,globl
	.previous
