#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryVolumeInformationFile,NtQueryVolumeInformationFile

	.text.windows
	.ftrace1
NtQueryVolumeInformationFile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQueryVolumeInformationFile(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryVolumeInformationFile,globl
	.previous
