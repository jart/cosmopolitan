.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSASocketW,WSASocketW,120

	.text.windows
WSASocket:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASocketW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WSASocket,globl
	.previous
