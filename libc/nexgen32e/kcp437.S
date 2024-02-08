/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#include "libc/str/tab.internal.h"
#include "libc/macros.internal.h"
.rodata
.balign	2

//	ibm cp437 unicode table w/ string literal safety
//
//	            ░▄██▒▄█ ▐██ ░░░     ▀▀████▒▀█▄
//	           ▐███▓██░ ██▌            ▀████▄■█▄
//	          ▐█▓███▀█░██▀   ░          ░▀█████▓▄
//	         ▐█▓██▀▄█▒██▀  ▄▄░  ▄▄▄ ░░░   ░▀████▒▄
//	        ▐████▀▄█■█▀      ▀▀             ░█████░
//	       ▐█▓█▀████▀          ░             ▐▓███▒
//	       █░███▀▀     ░░░    ▄█       ░░░    █████
//	      ▐█▓█░▀▀  ░░▄█▄▄▄▄▄  ▀▄ ▌▄▄▄░▄▄▄▄▄   ▐████░
//	      ▐███▌   ▄▀█████████▄ ▌▐▄████████▄   ▐▓███░
//	      ▐███░░░▀▄█▀▄▄████▄▀░  ▐████████▒ ▀   ░███░
//	      ░████░ ▓▀ ▄███████▀▌   ▀▄■████▀▀█▀   ██▀█
//	       ▓███░ ░▄▀▀░░░ ▀  ░░▌   ▄▀▀▄░░▀░▄▀▄ ▐██▀▄
//	       ░███░  ▄▓▓▄▄░▀▀█▀█ ▌░░  ▀█▀█▀▀     ▐██▀
//	     █▀▄▐██   ▀░░   ▄▀ ▐ █    ▀ ▄▄▄░     ░▀▄█▄▀█
//	     ▌▄  █▓ ▒      ░  █▄█▄▀▄▄▄███▄▀▄ ░░   ░ ▀  █▌
//	      █▌▄░▌      ░░░▄▀█▀███████▄▀▄▀▄▀▀▄▄▄  █▀█░▐
//	       ██▄     ░░░▄█▄▀██▄█■██████▄█▄█▄■▀█░  ▐░▐
//	        ▀██░   ░▄██████████████████▄█▄█ ░█ ░ ▄▀
//	        ▀▓█▄▓░░  ▒█▀█████████████████████▒ ██▀
//	         ▀███ ▓▒   ██████████████▀▀▀▀█▄▀ ░▄█▒
//	          ▀███ ▀█▄▀▄█████▀▀  ▓▓▓▄░   ▐  ░▄██
//	            ▀██ ▄███████▄████████▀░░   ░▄██
//	      ▄██▀▀▄ █▄▀▄██▒▒███████████▀▀▀▄░ ░███░
//	    ▄██▀▄▄░░▀▐▄████▄ █████▀▄░░█▀▄▀░░  ▄██░
//	    █████▄▄▄███▀░█▌██▄▀▀█████▄▄░░░▄▄███▀██▄ ▄▀▀▀▄▄
//	     ▀██████▀■▄█▄▄ ░▀███████████████▓▓░░▄██▀▄████▄▄▀▄
//
//	    █▀█ █  █▀█ █▀█ █▄▀ ▐▀█▀▌█▀█ █▀█ █▄ █ ▀█▀ █▀█ █▀▀
//	    █▀▄ █  █ █ █   █ ▀▄  █  █▀▄ █ █ █ ▀█  █  █   ▀▀█
//	    █▄█ █▄▌█▄█ █▄█ █  █  █  █ █ █▄█ █  █ ▄█▄ █▄█ █▄█
//
//	    THERE WILL BE BLOCKS               march 01 2017
//
//	@see	libc/str/str.h
//	@see	kCp437i[]
kCp437:
.short	0x00a0,0x263a,0x263b,0x2665,0x2666,0x2663,0x2660,0x2022	//00: ☺☻♥♦♣♠•
.short	0x25d8,0x25cb,0x25d9,0x2642,0x2640,0x266a,0x266b,0x263c	//08:◘○◙♂♀♪♫☼
.short	0x25ba,0x25c4,0x2195,0x203c,0x00b6,0x00a7,0x25ac,0x21a8	//10:►◄↕‼¶§▬↨
.short	0x2191,0x2193,0x2192,0x2190,0x221f,0x2194,0x25b2,0x25bc	//18:↑↓→←∟↔▲▼
.short	0x0020,0x0021,0x201c,0x0023,0x0024,0x0025,0x0026,0x2018	//20: !“//$%&‘
.short	0x0028,0x0029,0x002a,0x002b,0x002c,0x002d,0x002e,0x002f	//28:()*+,-./
.short	0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037	//30:01234567
.short	0x0038,0x0039,0x003a,0x003b,0x003c,0x003d,0x003e,0x2047	//38:89:;<=>⁇
.short	0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047	//40:@ABCDEFG
.short	0x0048,0x0049,0x004a,0x004b,0x004c,0x004d,0x004e,0x004f	//48:HIJKLMNO
.short	0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057	//50:PQRSTUVW
.short	0x0058,0x0059,0x005a,0x005b,0x005c,0x005d,0x005e,0x005f	//58:XYZ[\]^_
.short	0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067	//60:`abcdefg
.short	0x0068,0x0069,0x006a,0x006b,0x006c,0x006d,0x006e,0x006f	//68:hijklmno
.short	0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077	//70:pqrstuvw
.short	0x0078,0x0079,0x007a,0x007b,0x007c,0x007d,0x007e,0x2302	//78:xyz{|}~⌂
.short	0x00c7,0x00fc,0x00e9,0x00e2,0x00e4,0x00e0,0x00e5,0x00e7	//80:Çüéâäàåç
.short	0x00ea,0x00eb,0x00e8,0x00ef,0x00ee,0x00ec,0x00c4,0x00c5	//88:êëèïîìÄÅ
.short	0x00c9,0x00e6,0x00c6,0x00f4,0x00f6,0x00f2,0x00fb,0x00f9	//90:ÉæÆôöòûù
.short	0x00ff,0x00d6,0x00dc,0x00a2,0x00a3,0x00a5,0x20ac,0x0192	//98:ÿÖÜ¢£¥€ƒ
.short	0x00e1,0x00ed,0x00f3,0x00fa,0x00f1,0x00d1,0x00aa,0x00ba	//a0:áíóúñÑªº
.short	0x00bf,0x2310,0x00ac,0x00bd,0x00bc,0x00a1,0x00ab,0x00bb	//a8:¿⌐¬½¼¡«»
.short	0x2591,0x2592,0x2593,0x2502,0x2524,0x2561,0x2562,0x2556	//b0:░▒▓│┤╡╢╖
.short	0x2555,0x2563,0x2551,0x2557,0x255d,0x255c,0x255b,0x2510	//b8:╕╣║╗╝╜╛┐
.short	0x2514,0x2534,0x252c,0x251c,0x2500,0x253c,0x255e,0x255f	//c0:└┴┬├─┼╞╟
.short	0x255a,0x2554,0x2569,0x2566,0x2560,0x2550,0x256c,0x2567	//c8:╚╔╩╦╠═╬╧
.short	0x2568,0x2564,0x2565,0x2559,0x2558,0x2552,0x2553,0x256b	//d0:╨╤╥╙╘╒╓╫
.short	0x256a,0x2518,0x250c,0x2588,0x2584,0x258c,0x2590,0x2580	//d8:╪┘┌█▄▌▐▀
.short	0x03b1,0x00df,0x0393,0x03c0,0x03a3,0x03c3,0x03bc,0x03c4	//e0:αßΓπΣσμτ
.short	0x03a6,0x0398,0x03a9,0x03b4,0x221e,0x03c6,0x03b5,0x2229	//e8:ΦΘΩδ∞φε∩
.short	0x2261,0x00b1,0x2265,0x2264,0x2320,0x2321,0x00f7,0x2248	//f0:≡±≥≤⌠⌡÷≈
.short	0x00b0,0x2219,0x00d7,0x221a,0x207f,0x00b2,0x25a0,0x03bb	//f8:°∙×√ⁿ²■λ
.endobj	kCp437,globl
.previous
