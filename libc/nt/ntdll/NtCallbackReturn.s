.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCallbackReturn

	.text.windows
NtCallbackReturn:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCallbackReturn(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCallbackReturn,globl
	.previous
