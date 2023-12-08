/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "net/https/https.h"

__static_yoink("zipos");
__static_yoink("usr/share/ssl/root/");
__static_yoink("usr/share/ssl/root/amazon.pem");
__static_yoink("usr/share/ssl/root/certum.pem");
__static_yoink("usr/share/ssl/root/comodo.pem");
__static_yoink("usr/share/ssl/root/digicert.pem");
__static_yoink("usr/share/ssl/root/geotrust.pem");
__static_yoink("usr/share/ssl/root/globalsign.pem");
__static_yoink("usr/share/ssl/root/godaddy.pem");
__static_yoink("usr/share/ssl/root/google.pem");
__static_yoink("usr/share/ssl/root/isrg.pem");
__static_yoink("usr/share/ssl/root/quovadis.pem");
__static_yoink("usr/share/ssl/root/redbean.pem");
__static_yoink("usr/share/ssl/root/starfield.pem");
__static_yoink("usr/share/ssl/root/verisign.pem");

char ssl_root_support;
