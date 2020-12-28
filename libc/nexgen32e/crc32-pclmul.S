/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.h"

/	Computes Phil Katz CRC-32 w/ carryless multiply isa.
/
/	This is support code that's abstracted by crc32_z().
/
/	@param	edi is initial value
/	@param	rsi points to buffer
/	@param	rdx is bytes in buffer that's >=64 and %16==0
/	@return	eax is crc32
/	@note	needs Westmere (c.2010) or Bulldozer (c.2011)
/	@see	“Fast CRC Computation for Generic Polynomials Using
/		 PCLMULQDQ Instruction” V. Gopal, E. Ozturk, et al.,
/		 2009, intel.ly/2ySEwL0
crc32$pclmul:
	.leafprologue
	.profilable
	movdqu	(%rsi),%xmm7
	movd	%edi,%xmm1
	movdqu	16(%rsi),%xmm9
	movdqu	32(%rsi),%xmm4
	movdqu	48(%rsi),%xmm0
	lea	-64(%rdx),%rdi
	lea	64(%rsi),%rcx
	pxor	%xmm7,%xmm1
	movdqa	.Lk1k2(%rip),%xmm8
	cmp	$63,%rdi
	jbe	2f
	lea	-128(%rdx),%rdi
	mov	%rdi,%rdx
	shr	$6,%rdx
	lea	2(%rdx),%rax
	sal	$6,%rax
	add	%rax,%rsi
	mov	%rcx,%rax
3:	add	$64,%rax
	movdqa	%xmm1,%xmm7
	movdqa	%xmm4,%xmm5
	movdqa	%xmm0,%xmm3
	movdqa	%xmm9,%xmm6
	movdqa	%xmm9,%xmm2
	movdqu	-48(%rax),%xmm9
	pclmullqlqdq %xmm8,%xmm7
	pclmullqlqdq %xmm8,%xmm6
	pclmullqlqdq %xmm8,%xmm5
	pclmulhqhqdq %xmm8,%xmm1
	pclmulhqhqdq %xmm8,%xmm2
	pclmulhqhqdq %xmm8,%xmm4
	pxor	%xmm7,%xmm1
	movdqu	-64(%rax),%xmm7
	pxor	%xmm6,%xmm2
	pxor	%xmm5,%xmm4
	movdqu	-32(%rax),%xmm6
	movdqu	-16(%rax),%xmm5
	pclmullqlqdq %xmm8,%xmm3
	pclmulhqhqdq %xmm8,%xmm0
	pxor	%xmm7,%xmm1
	pxor	%xmm3,%xmm0
	pxor	%xmm2,%xmm9
	pxor	%xmm6,%xmm4
	pxor	%xmm5,%xmm0
	cmp	%rsi,%rax
	jne	3b
	lea	1(%rdx),%rax
	sal	$6,%rdx
	sal	$6,%rax
	sub	%rdx,%rdi
	add	%rax,%rcx
2:	movdqa	.Lk3k4(%rip),%xmm3
	movdqa	%xmm1,%xmm2
	movdqa	%xmm1,%xmm5
	pclmulhqhqdq %xmm3,%xmm2
	pclmullqlqdq %xmm3,%xmm5
	pxor	%xmm9,%xmm2
	pxor	%xmm5,%xmm2
	movdqa	%xmm2,%xmm5
	pclmulhqhqdq %xmm3,%xmm2
	movdqa	%xmm2,%xmm1
	pclmullqlqdq %xmm3,%xmm5
	pxor	%xmm4,%xmm1
	pxor	%xmm5,%xmm1
	movdqa	%xmm1,%xmm2
	pclmulhqhqdq %xmm3,%xmm1
	pclmullqlqdq %xmm3,%xmm2
	pxor	%xmm1,%xmm0
	pxor	%xmm2,%xmm0
	cmp	$15,%rdi
	jbe	4f
	sub	$16,%rdi
	mov	%rcx,%rax
	and	$-16,%rdi
	lea	16(%rcx,%rdi),%rdx
5:	movdqa	%xmm0,%xmm1
	movdqu	(%rax),%xmm6
	pclmulhqhqdq %xmm3,%xmm0
	add	$16,%rax
	pclmullqlqdq %xmm3,%xmm1
	pxor	%xmm1,%xmm0
	pxor	%xmm6,%xmm0
	cmp	%rax,%rdx
	jne	5b
4:	movdqa	%xmm0,%xmm1
	movdqa	.Lboop(%rip),%xmm2
	psrldq	$8,%xmm0
	pclmullqhqdq %xmm3,%xmm1
	movdqa	.Lpoly(%rip),%xmm3
	pxor	%xmm1,%xmm0
	movdqa	%xmm0,%xmm1
	pand	%xmm2,%xmm0
	pclmullqlqdq .Lk5k0(%rip),%xmm0
	psrldq	$4,%xmm1
	pxor	%xmm0,%xmm1
	movdqa	%xmm1,%xmm0
	pand	%xmm2,%xmm0
	pclmullqhqdq %xmm3,%xmm0
	pand	%xmm2,%xmm0
	pclmullqlqdq %xmm3,%xmm0
	pxor	%xmm1,%xmm0
	pextrd	$1,%xmm0,%eax
	.leafepilogue
	.endfn	crc32$pclmul,globl,hidden

/	Definitions of the bit-reflected domain constants k1,k2,k3, etc.
/	and the CRC32+Barrett polynomials given at the end of the paper.
	.rodata.cst16
.Lk1k2:	.quad	0x0000000154442bd4
	.quad	0x00000001c6e41596
	.endobj	.Lk1k2
.Lk3k4:	.quad	0x00000001751997d0
	.quad	0x00000000ccaa009e
	.endobj	.Lk3k4
.Lk5k0:	.quad	0x0000000163cd6124
	.quad	0x0000000000000000
	.endobj	.Lk5k0
.Lboop:	.quad	0x00000000ffffffff
	.quad	0x00000000ffffffff
	.endobj	.Lboop
.Lpoly:	.quad	0x00000001db710641
	.quad	0x00000001f7011641
	.endobj	.Lpoly
	.previous

/*	crc32() w/ pclmul for #c per n where c ≈ 0.293ns
	N                     x1            x8           x64	mBps
	------------------------------------------------------------
	1               4437.000        42.375        38.141      85
	1                 45.000        39.375        38.234      85
	2                 31.500        25.312        23.102     141
	3                 25.667        19.792        17.911     181
	4                 21.250        16.219        15.035     216
	7                 18.429        12.946        11.712     277
	8                 16.125        12.578        10.998     296
	15                12.867         9.925         9.161     355
	16                12.438         9.836         9.114     357
	31                11.194         8.528         8.149     399
	32                10.781         8.418         8.098     401
	63                 9.063         7.780         7.647     425
	64                 3.109         1.604         1.414    2299
	127                2.260         1.824         1.729    1880
	128                1.305         0.860         0.806    4033
	255                1.290         1.001         0.948    3428
	256                0.574         0.491         0.476    6822
	511                0.773         0.571         0.546    5956
	512                0.354         0.320         0.306   10613
	1023               0.425         0.365         0.347    9375
	1024               0.237         0.229         0.231   14097
	2047               0.278         0.251         0.246   13236
	2048               0.187         0.187         0.188   17306
	4095               0.229         0.200         0.194   16761
	4096               0.162         0.170         0.167   19438
	8191               0.182         0.173         0.178   18266
	8192               0.162         0.155         0.158   20560
	16383              0.156         0.162         0.154   21136
	16384              0.156         0.156         0.148   22005
	32767              0.163         0.149         0.149   21768
	32768              0.150         0.146         0.145   22491
	65535              0.158         0.141         0.141   23102
	65536              0.149         0.140         0.138   23478
	131071             0.150         0.145         0.141   23011
	131072             0.148         0.141         0.148   21892
	262143             0.151         0.148         0.147   22136
	262144             0.149         0.146         0.146   22298
	524287             0.150         0.149         0.149   21832
	524288             0.148         0.148         0.147   22043
	1048575            0.148         0.158         0.163   19913
	1048576            0.156         0.179         0.153   21186
	2097151            0.153         0.149         0.148   21979
	2097152            0.147         0.148         0.147   22040
	4194303            0.148         0.148         0.151   21482
	4194304            0.148         0.148         0.147   22061
	8388607            0.185         0.183         0.185   17536
	8388608            0.193         0.180         0.183   17769

	crc32() w/ 10+ year old cpus for #c per n where c ≈ 0.293ns
	N                     x1            x8           x64	mBps
	------------------------------------------------------------
	1               4447.000        43.625        37.641      86
	1                 41.000        37.125        37.609      86
	2                 31.500        26.562        22.477     145
	3                 25.000        20.125        17.422     187
	4                 21.250        16.594        15.230     213
	7                 16.714        13.089        11.717     277
	8                 16.875        12.609        11.174     291
	15                12.733         9.958         9.339     348
	16                12.438         9.852         9.208     353
	31                10.935         8.617         8.164     398
	32                10.906         8.496         8.155     399
	63                 9.095         7.819         7.692     423
	64                 9.172         7.807         7.692     423
	127                8.165         7.531         7.438     437
	128                8.133         7.503         7.437     437
	255                7.714         7.329         7.293     446
	256                7.723         7.348         7.293     446
	511                7.434         7.253         7.223     450
	512                7.412         7.237         7.218     450
	1023               7.274         7.214         7.201     451
	1024               7.292         7.203         7.189     452
	2047               7.232         7.185         7.178     453
	2048               7.239         7.189         7.186     452
	4095               7.189         7.175         7.172     453
	4096               7.192         7.173         7.172     453
	8191               7.187         7.173         7.172     453
	8192               7.183         7.174         7.181     453
	16383              7.175         7.170         7.169     453
	16384              7.176         7.169         7.169     453
	32767              7.169         7.182         7.170     453
	32768              7.173         7.172         7.172     453
	65535              7.170         7.170         7.171     453
	65536              7.172         7.171         7.204     451
	131071             7.170         7.354         7.260     448
	131072             7.172         7.172         7.182     453
	262143             7.037         7.178         7.182     453
	262144             7.169         7.343         7.205     451
	524287             7.438         7.170         7.206     451
	524288             7.169         7.164         7.209     451
	1048575            6.995         7.119         7.158     454
	1048576            7.168         7.110         7.157     454
	2097151            7.057         7.058         7.065     460
	2097152            6.977         7.047         7.089     458
	4194303            7.017         7.504         7.030     462
	4194304            7.025         7.059         7.030     462
	8388607            7.082         6.980         6.997     464
	8388608            7.051         6.985         6.999     464 */
	.source	__FILE__
