.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryInformationProcess

	.text.windows
NtQueryInformationProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryInformationProcess(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryInformationProcess,globl
	.previous
