.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_MapVirtualKeyExW,MapVirtualKeyExW,2155

	.text.windows
MapVirtualKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MapVirtualKeyExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	MapVirtualKeyEx,globl
	.previous
