/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/math.h"
#include "libc/tinymath/freebsd.internal.h"

// clang-format off

/**
 * Returns remainder of dividing 𝑥 by 𝑦.
 */
float remainderf(float x, float y) {
  int q;
  return remquof(x, y, &q);
}

static const float zero = 0.0;

float
remainderf2(float x, float p)
{
	int32_t hx,hp;
	uint32_t sx;
	float p_half;

	GET_FLOAT_WORD(hx,x);
	GET_FLOAT_WORD(hp,p);
	sx = hx&0x80000000;
	hp &= 0x7fffffff;
	hx &= 0x7fffffff;

    /* purge off exception values */
	if((hp==0)||			 	/* p = 0 */
	  (hx>=0x7f800000)||			/* x not finite */
	  ((hp>0x7f800000)))			/* p is NaN */
	    return nan_mix_op(x, p, *)/nan_mix_op(x, p, *);


	if (hp<=0x7effffff) x = fmodf(x,p+p);	/* now x < 2p */
	if ((hx-hp)==0) return zero*x;
	x  = fabsf(x);
	p  = fabsf(p);
	if (hp<0x01000000) {
	    if(x+x>p) {
		x-=p;
		if(x+x>=p) x -= p;
	    }
	} else {
	    p_half = (float)0.5*p;
	    if(x>p_half) {
		x-=p;
		if(x>=p_half) x -= p;
	    }
	}
	GET_FLOAT_WORD(hx,x);
	if ((hx&0x7fffffff)==0) hx = 0;
	SET_FLOAT_WORD(x,hx^sx);
	return x;
}
