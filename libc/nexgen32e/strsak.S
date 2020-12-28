/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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
#include "libc/nexgen32e/x86feature.h"
#include "libc/nexgen32e/macros.h"
#include "libc/macros.h"
.source	__FILE__

/	Returns pointer to first instance of character.
/
/	@param	rdi is a non-null NUL-terminated string pointer
/	@param	esi is the search byte
/	@return	rax points to character, or to NUL byte if not found
/	@note	this won't return NULL if search character is NUL
strchrnul:
	.leafprologue
	.profilable
	or	$-1,%r9
	jmp	0f
	.endfn	strchrnul,globl

/	Returns pointer to first instance of character, the BSD way.
/
/	@param	rdi is a non-null NUL-terminated string pointer
/	@param	esi is the search byte
/	@return	rax points to first result, or NULL if not found
/	@note	this won't return NULL if search character is NUL
index:	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	index,globl

/	Returns pointer to first instance of character.
/
/	@param	rdi is a non-null NUL-terminated string pointer
/	@param	esi is the search byte
/	@return	rax points to first result, or NULL if not found
/	@note	this won't return NULL if search character is NUL
/	@asyncsignalsafe
strchr:	.leafprologue
	.profilable
	xor	%r9d,%r9d
0:	movzbl	%sil,%edx
	or	$-1,%rsi
	xor	%r8,%r8
	jmp	strsak
	.endfn	strchr,globl

/	Returns pointer to first instance of character in range.
/
/	@param	rdi is a non-null pointer to memory
/	@param	esi is the search byte
/	@return	rax points to byte if found, or else undefined behavior
rawmemchr:
	or	$-1,%rdx
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	rawmemchr,globl

/	Returns pointer to first instance of character in range.
/
/	@param	rdi is a non-null pointer to memory
/	@param	esi is the search byte
/	@param	rdx is length of memory in bytes
/	@return	rax points to byte if found or NULL
/	@asyncsignalsafe
memchr:	.leafprologue
	.profilable
	xchg	%rsi,%rdx
	mov	%dl,%dh
	xor	%r8,%r8
	xor	%r10,%r10
	jmp	strsak
	.endfn	memchr,globl

/	Returns length of NUL-terminated string w/ security blankets.
/
/	This is like strnlen() except it'll return 0 if (1) RDI is NULL
/	or (2) a NUL-terminator wasn't found in the first RSI bytes.
/
/	@param	rdi is a nullable NUL-terminated string pointer
/	@param	rsi is the maximum number of bytes to consider
/	@return	rax is the number of bytes, excluding the NUL
strnlen_s:
	.leafprologue
	.profilable
	xor	%eax,%eax
	xor	%r10d,%r10d
	test	%rdi,%rdi
	jnz	0f
	.leafepilogue
	.endfn	strnlen_s,globl

/	Returns length of NUL-terminated memory, with limit.
/
/	@param	rdi is non-null memory
/	@param	rsi is the maximum number of bytes to consider
/	@return	rax is the number of bytes, excluding the NUL
/	@asyncsignalsafe
strnlen:.leafprologue
	.profilable
	or	$-1,%r10
0:	xor	%edx,%edx
	mov	%rdi,%r8
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	strnlen,globl

/	Swiss army knife of string character scanning.
/	Fourteen fast functions in one.
/
/	@param	rdi is non-null string memory
/	@param	rsi is max number of bytes to consider
/	@param	dl is search character #1
/	@param	dh is search character #2
/	@param	r8 is subtracted from result (for length vs. pointer)
/	@param	r9 masks result if DH is found (for NUL vs. NULL)
/	@param	r10 masks result on bytes exhausted (for length v. NULL)
/	@return	rax end pointer after r8/r9/r10 modifications
strsak:	lea	-1(%rdi),%rax
1:	add	$1,%rax
	sub	$1,%rsi
	jb	.Lend
	test	$31,%al
	jz	.Lfast
.Lbyte:	mov	(%rax),%cl
	cmp	%cl,%dl
	je	.Ldone
	cmp	%cl,%dh
	je	.Lnul
	jmp	1b
.Ldone:	sub	%r8,%rax
	jmp	.Lret
.Lend:	mov	%r10,%r9
.Lnul:	sub	%r8,%rax
	and	%r9,%rax
.Lret:	.leafepilogue
.Lslow:	add	$32,%rsi
	jmp	.Lbyte
.Lfast:	movzbl	%dl,%ecx
	movd	%ecx,%xmm0
	movzbl	%dh,%ecx
	movd	%ecx,%xmm1
	sub	$32,%rax
#if !X86_NEED(AVX2)
	testb	X86_HAVE(AVX2)+kCpuids(%rip)
	jz	.Lsse2
#endif
	vpbroadcastb %xmm0,%ymm0
	vpbroadcastb %xmm1,%ymm1
1:	add	$32,%rax
	sub	$32,%rsi
9:	jb	.Lslow
	vmovdqa	(%rax),%ymm2
	vpcmpeqb %ymm0,%ymm2,%ymm3
	vpcmpeqb %ymm1,%ymm2,%ymm2
	vpor	%ymm3,%ymm2,%ymm2
	vpmovmskb %ymm2,%ecx
	bsf	%ecx,%ecx
	je	1b
	vzeroupper
2:	add	%rcx,%rax
	jmp	.Lbyte
#if !X86_NEED(AVX2)
.Lsse2:	pbroadcastb %xmm0
	pbroadcastb %xmm1
1:	add	$32,%rax
	sub	$32,%rsi
	jb	9b
	movdqa	(%rax),%xmm2
	movdqa	16(%rax),%xmm3
	movdqa	%xmm3,%xmm4
	pcmpeqb	%xmm0,%xmm3
	pcmpeqb %xmm1,%xmm4
	por	%xmm4,%xmm3
	pmovmskb %xmm3,%ecx
	shl	$16,%ecx
	movdqa	%xmm2,%xmm4
	pcmpeqb	%xmm0,%xmm2
	pcmpeqb %xmm1,%xmm4
	por	%xmm4,%xmm2
	pmovmskb %xmm2,%r11d
	or	%r11d,%ecx
	bsf	%ecx,%ecx
	je	1b
	jmp	2b
#endif
	.endfn	strsak,globl,hidden

/*	benchmarked on intel core i7-6700 @ 3.40GHz (skylake)
	includes function call overhead (unless marked otherwise)

	your strlen, &c (strsak+avx2) for #c per n where c ≈ 0.293ns
	N                     x1            x8           x64	mBps
	------------------------------------------------------------
	1                 47.000        36.375        35.141      99
	1                 35.000        34.625        36.234      96
	2                 31.500        18.812        18.992     184
	3                 19.667        13.042        13.182     265
	4                 30.750        10.281        10.285     339
	7                 15.857         8.946         7.551     462
	8                 12.125         9.203         7.119     490
	15                10.467         5.475         4.601     758
	16                 6.812         5.523         4.798     727
	31                 5.387         4.327         3.517     992
	32                 4.719         1.645         1.532    2278
	63                 5.000         2.403         2.034    1715
	64                 2.047         0.779         0.788    4427
	127                2.134         1.194         1.027    3399
	128                1.742         0.444         0.419    8327
	255                0.945         0.594         0.554    6295
	256                0.574         0.271         0.264   13226
	511                0.785         0.362         0.307   11384
	512                0.326         0.178         0.151   23134
	1023               0.288         0.242         0.185   18862
	1024               0.208         0.114         0.107   32565
	2047               0.235         0.127         0.123   28430
	2048               0.127         0.090         0.084   41413
	4095               0.119         0.106         0.099   35116
	4096               0.100         0.081         0.079   44372
	8191               0.092         0.082         0.081   43176
	8192               0.081         0.072         0.071   49419
	16383              0.076         0.072         0.071   48847
	16384              0.071         0.068         0.067   52381
	32767              0.072         0.069         0.068   51154
	32768              0.068         0.066         0.065   53409

	your tinystrlen()
	N                     x1            x8           x64	mBps
	------------------------------------------------------------
	1                 53.000        33.625        33.672      97
	1                 33.000        32.125        32.234     101
	2                 24.500        19.438        17.711     184
	3                 23.667        12.875        11.911     273
	4                 13.750         9.281         9.238     352
	7                 11.000         6.125         5.801     560
	8                  7.625         5.609         5.232     621
	15                11.800         3.825         3.364     966
	16                 4.562         3.648         3.173    1024 « optimal
	31                 3.710         2.851         2.298    1414
	32                 3.031         2.254         2.159    1506 « dropoff
	63                 2.683         1.827         1.691    1922
	64                 2.078         1.932         1.689    1924
	127                1.630         1.647         1.622    2004
	128                1.727         1.671         1.652    1968
	255                1.392         1.450         1.435    2265
	256                1.473         1.427         1.437    2262
	511                1.325         1.353         1.337    2431
	512                1.408         1.343         1.337    2431
	1023               1.289         1.281         1.287    2525
	1024               1.269         1.295         1.297    2506
	2047               1.269         1.274         1.269    2561
	2048               1.280         1.263         1.281    2538
	4095               1.262         1.270         1.266    2568
	4096               1.270         1.264         1.265    2570
	8191               1.253         1.254         1.254    2592
	8192               1.219         1.224         1.225    2653
	16383              1.225         1.222         1.220    2663
	16384              1.226         1.221         1.222    2659
	32767              1.227         1.224         1.223    2658
	32768              1.220         1.221         1.222    2659

	glibc strlen for #c per n where c ≈ 0.273ns
	N                     x1            x8           x64	mBps
	------------------------------------------------------------
	1               3497.000        53.125        42.641      82
	1                 69.000        44.875        42.547      82
	2                 45.500        24.188        21.852     160
	3                 23.000        15.625        14.557     240
	4                 22.250        11.406        10.637     328
	7                 10.143         6.768         6.230     560
	8                 11.125         5.797         5.486     636
	15                 5.800         3.142         2.859    1220
	16                 7.062         3.070         2.737    1275
	31                 2.806         1.585         1.407    2481
	32                 3.156         1.574         1.349    2587
	63                 2.016         0.895         0.691    5049
	64                 1.328         0.744         0.670    5207
	127                1.441         0.521         0.407    8577
	128                0.648         0.454         0.405    8619
	255                0.553         0.286         0.214   16277
	256                0.387         0.235         0.218   15984
	511                0.456         0.151         0.129   27077
	512                0.182         0.134         0.129   27117
	1023               0.171         0.106         0.082   42795
	1024               0.112         0.088         0.082   42741
	2047               0.099         0.069         0.059   59537
	2048               0.072         0.060         0.058   59925
	4095               0.065         0.053         0.047   74122
	4096               0.061         0.048         0.047   74478
	8191               0.048         0.045         0.044   79117
	8192               0.051         0.045         0.044   79181
	16383              0.042         0.040         0.061   57018
	16384              0.069         0.063         0.061   57245
	32767              0.081         0.073         0.068   51426
	32768              0.084         0.072         0.068   51285

	GCC strlen (-Os REPNZ SCASB) for #c per n where c ≈ 0.293ns
	N                     x1            x8           x64	mBps
	------------------------------------------------------------
	1                103.000        84.125        88.766      37
	1                 81.000        85.125        87.328      37
	2                 43.500        44.562        45.508      71
	3                 33.000        30.208        30.995     105
	4                 24.750        23.156        23.113     141
	7                 17.000        13.054        15.355     212
	8                 13.375        14.047        13.982     232
	15                 9.533         9.258        55.111      59
	16                 6.312         6.352         6.364     511
	31                 4.032         4.141         4.141     785
	32                 3.969         4.059         4.048     803
	63                 2.937         2.970         2.995    1086
	64                 2.922         2.939         2.956    1100
	127                2.386         2.408         2.403    1353
	128                2.383         2.403         2.401    1354
	255                2.129         2.118         2.124    1530
	256                2.137         2.133         2.130    1526
	511                1.982         1.986         3.351     970
	512                1.982         1.990         1.986    1637
	1023               1.915         1.916         2.587    1257
	1024               1.868         1.867         1.866    1742
	2047               1.835         1.833         1.832    1775
	2048               1.830         1.831         1.832    1775
	4095               1.814         1.814         1.815    1791
	4096               1.810         1.815         1.815    1791
	8191               1.805         1.807         1.806    1800
	8192               1.805         1.806         1.806    1800
	16383              1.803         1.756         1.756    1851
	16384              1.758         1.756         1.756    1851
	32767              1.756         1.754         1.754    1853
	32768              1.756         1.754         1.754    1853

	Intel Optimz. Manual (SSE4.2) for #c per n where c ≈ 0.273ns
	N                     x1            x8           x64	mBps
	------------------------------------------------------------
	1                 37.000        43.125        34.078     102
	1                 33.000        33.875        34.016     103
	2                 39.500        17.188        17.555     199
	3                 18.333        12.208        12.036     290
	4                 30.250         9.344         9.137     382
	7                 14.429         5.732         5.766     605
	8                  7.875         6.797         5.354     652
	15                10.733         5.825         3.516     993
	16                 3.812         2.383         2.325    1501
	31                 4.097         2.609         2.079    1678
	32                 3.031         1.395         1.349    2587
	63                 2.937         1.558         1.079    3235
	64                 2.016         0.893         0.690    5056
	127                1.929         0.721         0.607    5745
	128                0.617         0.483         0.428    8147
	255                1.275         0.404         0.411    8486
	256                0.480         0.319         0.299   11681
	511                0.479         0.307         0.288   12127
	512                0.322         0.244         0.232   15013
	1023               0.324         0.224         0.225   15512
	1024               0.245         0.240         0.223   15651
	2047               0.222         0.213         0.206   16938
	2048               0.204         0.194         0.192   18140
	4095               0.204         0.188         0.185   18888
	4096               0.183         0.179         0.179   19446
	8191               0.179         0.176         0.174   20000
	8192               0.174         0.172         0.171   20383
	16383              0.171         0.170         0.169   20604
	16384              0.169         0.169         0.168   20808
	32767              0.213         0.225         0.267   13064
	32768              0.231         0.215         0.220   15852

	musl libc strlen for #c per n where c ≈ 0.273ns
	N                     x1            x8           x64	mBps
	------------------------------------------------------------
	1                 65.000        36.125        37.984      92
	1                 39.000        37.625        37.422      93
	2                 41.500        21.938        20.695     169
	3                 22.333        17.625        15.859     220
	4                 21.250        13.656        12.105     288
	7                 22.143         9.018         7.609     459
	8                 31.125         7.234         7.346     475
	15                11.267         5.025         4.709     741
	16                 9.438         4.039         3.849     907
	31                 4.871         3.133         2.488    1402
	32                 5.219         2.246         2.039    1712
	63                 4.302         1.462         1.407    2479
	64                 2.109         1.428         1.155    3023
	127                1.551         1.078         0.879    3971
	128                1.742         0.903         0.760    4591
	255                0.922         0.558         0.605    5764
	256                0.934         0.575         0.537    6495
	511                0.550         0.493         0.455    7674
	512                0.646         0.490         0.426    8183
	1023               0.550         0.439         0.425    8203
	1024               0.472         0.421         0.408    8549
	2047               0.507         0.334         0.373    9360
	2048               0.403         0.426         0.409    8540
	4095               0.391         0.240         0.236   14799
	4096               0.238         0.222         0.221   15766
	8191               0.225         0.223         0.221   15779
	8192               0.225         0.214         0.215   16250
	16383              0.212         0.212         0.210   16595
	16384              0.209         0.210         0.211   16535
	32767              0.214         0.208         0.205   17001
	32768              0.207         0.207         0.291   12002

	newlib strlen for #c per n where c ≈ 0.273ns
	N                     x1            x8           x64	mBps
	------------------------------------------------------------
	1                 33.000        34.625        34.141     102
	1                 33.000        34.125        33.984     103
	2                 58.500        18.562        17.508     199
	3                 16.333        12.792        12.016     290
	4                 19.250         9.219         9.215     379
	7                 17.571         6.089         5.685     614
	8                 16.625         5.078         5.432     642
	15                 8.467         4.042         3.207    1088
	16                 3.938         2.773         2.733    1277
	31                 3.645         1.673         1.598    2183
	32                 3.281         1.527         1.493    2338
	63                 2.619         1.042         0.895    3901
	64                 1.422         0.928         0.813    4294
	127                0.984         0.718         0.561    6222
	128                1.195         0.591         0.532    6558
	255                0.600         0.404         0.397    8785
	256                0.621         0.429         0.376    9280
	511                0.346         0.311         0.306   11421
	512                0.420         0.308         0.296   11776
	1023               0.284         0.285         0.285   12237
	1024               0.321         0.282         0.280   12456
	2047               0.253         0.252         0.252   13864
	2048               0.260         0.249         0.249   14012
	4095               0.236         0.236         0.236   14811
	4096               0.239         0.235         0.234   14906
	8191               0.233         0.228         0.227   15371
	8192               0.230         0.227         0.227   15397
	16383              0.223         0.224         0.223   15638
	16384              0.223         0.224         0.223   15663
	32767              0.224         0.387         0.225   15527
	32768              0.223         0.222         0.222   15724

	Agner Fog's strlen (SSE2) for #c per n where c ≈ 0.273ns
	N                     x1            x8           x64	mBps
	------------------------------------------------------------
	1                 59.000        38.375        38.453      91
	1                 37.000        38.625        38.234      91
	2                 18.500        19.062        19.273     181
	3                 13.000        12.792        12.859     271
	4                  9.250         9.594         9.660     361
	7                  5.286         5.554         5.502     634
	8                  4.625         4.703         4.791     728
	15                 2.600         2.858         2.622    1331
	16                 2.438         2.414         2.421    1442
	31                 2.161         1.399         1.290    2706
	32                 1.219         1.262         1.250    2793
	63                 1.508         0.875         0.693    5038
	64                 0.641         0.654         0.655    5328
	127                1.205         0.406         0.379    9200
	128                0.367         0.372         0.369    9463
	255                0.467         0.310         0.235   14835
	256                0.230         0.232         0.232   15034
	511                0.272         0.181         0.159   21918
	512                0.174         0.161         0.158   22148
	1023               0.175         0.134         0.120   29043
	1024               0.140         0.122         0.120   29005
	2047               0.128         0.114         0.112   31205
	2048               0.130         0.113         0.112   31242
	4095               0.105         0.098         0.097   35984
	4096               0.105         0.098         0.097   35973
	8191               0.093         0.090         0.090   38953
	8192               0.094         0.090         0.090   38986
	16383              0.088         0.086         0.086   40648
	16384              0.088         0.086         0.086   40652
	32767              0.088         0.086         0.085   40956
	32768              0.087         0.085         0.085   41114 */
