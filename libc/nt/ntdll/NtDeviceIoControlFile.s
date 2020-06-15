.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtDeviceIoControlFile

	.text.windows
NtDeviceIoControlFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtDeviceIoControlFile(%rip),%rax
	jmp	__sysv2nt10
	.endfn	NtDeviceIoControlFile,globl
	.previous
