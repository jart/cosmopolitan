/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│                                                                              │
│  Copyright 2015 Intel Corporation                                            │
│                                                                              │
│  Redistribution and use in source and binary forms, with or without          │
│  modification, are permitted provided that the following conditions          │
│  are met:                                                                    │
│                                                                              │
│    * Redistributions of source code must retain the above copyright          │
│      notice, this list of conditions and the following disclaimer.           │
│    * Redistributions in binary form must reproduce the above copyright       │
│      notice, this list of conditions and the following disclaimer in         │
│      the documentation and/or other materials provided with the              │
│      distribution.                                                           │
│    * Neither the name of Intel Corporation nor the names of its              │
│      contributors may be used to endorse or promote products derived         │
│      from this software without specific prior written permission.           │
│                                                                              │
│  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS         │
│  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT           │
│  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR       │
│  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT        │
│  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,       │
│  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT            │
│  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,       │
│  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY       │
│  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT         │
│  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE       │
│  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.        │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.internal.h"

.text
.balign 32
.ident "\n\
Intel SHA-NI (BSD-3 License)\n\
Copyright 2015 Intel Corporation\n\
Sean Gulley <sean.m.gulley@intel.com>\n\
Tim Chen <tim.c.chen@linux.intel.com>\n"
.include "libc/disclaimer.inc"

#define DIGEST_PTR	%rdi	/* 1st arg */
#define DATA_PTR	%rsi	/* 2nd arg */
#define NUM_BLKS	%rdx	/* 3rd arg */
#define SHA256CONSTANTS	%rax
#define MSG		%xmm0
#define STATE0		%xmm1
#define STATE1		%xmm2
#define MSGTMP0		%xmm3
#define MSGTMP1		%xmm4
#define MSGTMP2		%xmm5
#define MSGTMP3		%xmm6
#define MSGTMP4		%xmm7
#define SHUF_MASK	%xmm8
#define ABEF_SAVE	%xmm9
#define CDGH_SAVE	%xmm10

//	Performs Intel® SHA-NI™ optimized SHA-256 update.
//
//	The function takes a pointer to the current hash values, a
//	pointer to the input data, and a number of 64 byte blocks to
//	process. Once all blocks have been processed, the digest pointer
//	is updated with the resulting hash value. The function only
//	processes complete blocks, there is no functionality to store
//	partial blocks. All message padding and hash value
//	initialization must be done outside the update function.
//
//	The indented lines in the loop are instructions related to
//	rounds processing. The non-indented lines are instructions
//	related to the message schedule.
//
//	    void sha256_transform_ni(uint32_t digest[static 8],
//	                             const void *data,
//	                             int32_t numBlocks);
//
//	@param	%rdi points to output digest
//	@param	%rsi points to input data
//	@param	%rdx is number of blocks to process
//	@see	X86_HAVE(SHA)
	.ftrace1
sha256_transform_ni:
	.ftrace2
	.leafprologue
	shl	$6,NUM_BLKS			# convert to bytes
	jz	.Ldone_hash
	add	DATA_PTR,NUM_BLKS		# pointer to end of data

//	Load initial hash values
//	Need to reorder these appropriately
//	DCBA, HGFE -> ABEF, CDGH
	movdqu	0*16(DIGEST_PTR),STATE0
	movdqu	1*16(DIGEST_PTR),STATE1

	pshufd	$0xB1,STATE0,STATE0		# CDAB
	pshufd	$0x1B,STATE1,STATE1		# EFGH
	movdqa	STATE0,MSGTMP4
	palignr	$8,STATE1,STATE0		# ABEF
	pblendw	$0xF0,MSGTMP4,STATE1		# CDGH

	movdqa	PSHUFFLE_BYTE_FLIP_MASK(%rip),SHUF_MASK
	lea	kSha256(%rip),SHA256CONSTANTS

.Lloop0:

//	Save hash values for addition after rounds
	movdqa		STATE0,ABEF_SAVE
	movdqa		STATE1,CDGH_SAVE

//	Rounds 0-3
	movdqu		0*16(DATA_PTR),MSG
	pshufb		SHUF_MASK,MSG
	movdqa		MSG,MSGTMP0
	paddd		0*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0

//	Rounds 4-7
	movdqu		1*16(DATA_PTR),MSG
	pshufb		SHUF_MASK,MSG
	movdqa		MSG,MSGTMP1
	paddd		1*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP1,MSGTMP0

//	Rounds 8-11
	movdqu		2*16(DATA_PTR),MSG
	pshufb		SHUF_MASK,MSG
	movdqa		MSG,MSGTMP2
	paddd		2*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP2,MSGTMP1

//	Rounds 12-15
	movdqu		3*16(DATA_PTR),MSG
	pshufb		SHUF_MASK,MSG
	movdqa		MSG,MSGTMP3
	paddd		3*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP3,MSGTMP4
	palignr		$4,MSGTMP2,MSGTMP4
	paddd		MSGTMP4,MSGTMP0
	sha256msg2	MSGTMP3,MSGTMP0
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP3,MSGTMP2

//	Rounds 16-19
	movdqa		MSGTMP0,MSG
	paddd		4*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP0,MSGTMP4
	palignr		$4,MSGTMP3,MSGTMP4
	paddd		MSGTMP4,MSGTMP1
	sha256msg2	MSGTMP0,MSGTMP1
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP0,MSGTMP3

//	Rounds 20-23
	movdqa		MSGTMP1,MSG
	paddd		5*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP1,MSGTMP4
	palignr		$4,MSGTMP0,MSGTMP4
	paddd		MSGTMP4,MSGTMP2
	sha256msg2	MSGTMP1,MSGTMP2
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP1,MSGTMP0

//	Rounds 24-27
	movdqa		MSGTMP2,MSG
	paddd		6*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP2,MSGTMP4
	palignr		$4,MSGTMP1,MSGTMP4
	paddd		MSGTMP4,MSGTMP3
	sha256msg2	MSGTMP2,MSGTMP3
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP2,MSGTMP1

//	Rounds 28-31
	movdqa		MSGTMP3,MSG
	paddd		7*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP3,MSGTMP4
	palignr		$4,MSGTMP2,MSGTMP4
	paddd		MSGTMP4,MSGTMP0
	sha256msg2	MSGTMP3,MSGTMP0
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP3,MSGTMP2

//	Rounds 32-35
	movdqa		MSGTMP0,MSG
	paddd		8*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP0,MSGTMP4
	palignr		$4,MSGTMP3,MSGTMP4
	paddd		MSGTMP4,MSGTMP1
	sha256msg2	MSGTMP0,MSGTMP1
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP0,MSGTMP3

//	Rounds 36-39
	movdqa		MSGTMP1,MSG
	paddd		9*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP1,MSGTMP4
	palignr		$4,MSGTMP0,MSGTMP4
	paddd		MSGTMP4,MSGTMP2
	sha256msg2	MSGTMP1,MSGTMP2
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP1,MSGTMP0

//	Rounds 40-43
	movdqa		MSGTMP2,MSG
	paddd		10*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP2,MSGTMP4
	palignr		$4,MSGTMP1,MSGTMP4
	paddd		MSGTMP4,MSGTMP3
	sha256msg2	MSGTMP2,MSGTMP3
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP2,MSGTMP1

//	Rounds 44-47
	movdqa		MSGTMP3,MSG
	paddd		11*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP3,MSGTMP4
	palignr		$4,MSGTMP2,MSGTMP4
	paddd		MSGTMP4,MSGTMP0
	sha256msg2	MSGTMP3,MSGTMP0
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP3,MSGTMP2

//	Rounds 48-51
	movdqa		MSGTMP0,MSG
	paddd		12*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP0,MSGTMP4
	palignr		$4,MSGTMP3,MSGTMP4
	paddd		MSGTMP4,MSGTMP1
	sha256msg2	MSGTMP0,MSGTMP1
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0
	sha256msg1	MSGTMP0,MSGTMP3

//	Rounds 52-55
	movdqa		MSGTMP1,MSG
	paddd		13*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP1,MSGTMP4
	palignr		$4,MSGTMP0,MSGTMP4
	paddd		MSGTMP4,MSGTMP2
	sha256msg2	MSGTMP1,MSGTMP2
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0

//	Rounds 56-59
	movdqa		MSGTMP2,MSG
	paddd		14*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	movdqa		MSGTMP2,MSGTMP4
	palignr		$4,MSGTMP1,MSGTMP4
	paddd		MSGTMP4,MSGTMP3
	sha256msg2	MSGTMP2,MSGTMP3
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0

//	Rounds 60-63
	movdqa		MSGTMP3,MSG
	paddd		15*16(SHA256CONSTANTS),MSG
	sha256rnds2	STATE0,STATE1
	pshufd 		$0x0E,MSG,MSG
	sha256rnds2	STATE1,STATE0

//	Add current hash values with previously saved
	paddd		ABEF_SAVE,STATE0
	paddd		CDGH_SAVE,STATE1

//	Increment data pointer and loop if more to process
	add		$64,DATA_PTR
	cmp		NUM_BLKS,DATA_PTR
	jne		.Lloop0

//	Write hash values back in the correct order
	pshufd		$0x1B,STATE0,STATE0		# FEBA
	pshufd		$0xB1,STATE1,STATE1		# DCHG
	movdqa		STATE0,MSGTMP4
	pblendw		$0xF0,STATE1,STATE0		# DCBA
	palignr		$8,MSGTMP4,STATE1		# HGFE

	movdqu		STATE0,0*16(DIGEST_PTR)
	movdqu		STATE1,1*16(DIGEST_PTR)

.Ldone_hash:
	.leafepilogue
	.endfn	sha256_transform_ni,globl

.section .rodata.cst16.PSHUFFLE_BYTE_FLIP_MASK,"aM",@progbits,16
.balign 16
PSHUFFLE_BYTE_FLIP_MASK:
	.octa	0x0c0d0e0f08090a0b0405060700010203
	.endobj	PSHUFFLE_BYTE_FLIP_MASK
