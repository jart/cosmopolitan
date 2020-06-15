.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtOpenProcessToken

	.text.windows
NtOpenProcessToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenProcessToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenProcessToken,globl
	.previous
