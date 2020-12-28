.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_DeviceIoControl,DeviceIoControl,0

	.text.windows
DeviceIoControl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DeviceIoControl(%rip),%rax
	jmp	__sysv2nt8
	.endfn	DeviceIoControl,globl
	.previous
