.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryVolumeInformationFile

	.text.windows
NtQueryVolumeInformationFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryVolumeInformationFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryVolumeInformationFile,globl
	.previous
