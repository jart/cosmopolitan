.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_DeviceIoControl,DeviceIoControl,265

	.text.windows
DeviceIoControl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DeviceIoControl(%rip),%rax
	jmp	__sysv2nt8
	.endfn	DeviceIoControl,globl
	.previous
