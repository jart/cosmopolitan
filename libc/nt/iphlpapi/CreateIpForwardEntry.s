.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_CreateIpForwardEntry,CreateIpForwardEntry,0

	.text.windows
CreateIpForwardEntry:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CreateIpForwardEntry(%rip)
	leave
	ret
	.endfn	CreateIpForwardEntry,globl
	.previous
