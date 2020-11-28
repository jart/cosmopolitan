.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlNtStatusToDosError

	.text.windows
RtlNtStatusToDosError:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlNtStatusToDosError(%rip)
	leave
	ret
	.endfn	RtlNtStatusToDosError,globl
	.previous
