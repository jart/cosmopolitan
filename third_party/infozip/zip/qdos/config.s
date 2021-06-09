;===========================================================================
; Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 1999-Oct-05 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, both of these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
;===========================================================================
#
.globl _qlflag
.globl _qlwait
#ifdef ZIP
.globl _dtype
#endif

.data
        ds.w    0
        dc.b    '<<QCFX>>01'
#ifdef ZIP
        dc.w    8
        dc.b    'Info-ZIP'
*                12345678901234567890
        ds.w    0
        dc.w    4
        dc.b    'qdos'
        ds.w    0
#else
        dc.w    10
        dc.b    'Info-UNZIP'
*                12345678901234567890
        ds.w    0
        dc.w    4
        dc.b    'qdos'
        ds.w    0
#endif
        dc.b    10
        dc.b    0
l_4:    dc.w    _qlwait-l_4
        dc.w    0
        dc.w    0
l_5:    dc.w    hpt-l_5
l_6:    dc.w    hxx-l_6

#ifdef ZIP
        dc.b    10
        dc.b    0
d_4:    dc.w    _dtype-d_4
        dc.w    0
        dc.w    0
d_5:    dc.w    dpt-d_5
d_6:    dc.w    dxx-d_6

#else
        dc.b    4
        dc.b    0
l5:
        dc.w    list1-l5
        dc.w    0
l5a:
        dc.w    Postit-l5a              ; post proc
l6:
        dc.w    apt-l6
l7:
        dc.w    axx-l7
* -------------------------------------
        dc.b    4
        dc.b    0
l8:
        dc.w    list2-l8
        dc.w    0
l8a:
        dc.w    Postit-l8a              ; post proc
l9:
        dc.w    bpt-l9
la:
        dc.w    bxx-la
* -------------------------------------
#endif
        dc.w    -1                          ; end

_qlflag:
        dc.w    0
_qlwait:
        dc.w    250
_dtype:
        dc.w    255

hpt:    dc.w    10
        dc.b    'Exit Delay'
*                12345678901234567890
        ds.w    0
hxx:    dc.w    0
        dc.w    $ffff
        dc.w    -1
#ifdef  ZIP
dpt:    dc.w    14
        dc.b    'Directory Type'
*                12345678901234567890
        ds.w    0
dxx:    dc.w    3
        dc.w    $ff
        dc.w    -1
#else

list1:
        dc.b    0
list2:
        dc.b    0

apt:
        dc.w    11
        dc.b    'Unpack Mode'
*                12345678901234567890
.even
axx:    dc.b    0
        dc.b    0
        dc.w    8
        dc.b    'SMS/QDOS'
.even
        dc.b    1
        dc.b    0
        dc.w    7
        dc.b    'Default'
.even
        dc.w    -1
.even
bpt:
        dc.w    12
        dc.b    'Listing Mode'
*                12345678901234567890
.even
bxx:
        dc.w    0
        dc.w    7
        dc.b    'Default'
.even
        dc.b    2
        dc.b    0
        dc.w    8
        dc.b    'SMS/QDOS'
*                12345678901234567890
.even
         dc.w    -1
Postit:
        lea.l   _qlflag,a0
        move.b  list1,d0
        move.b  d0,(a0)
        move.b  list2,d0
        or.b    d0,(a0)
        moveq   #0,d0
        rts
#endif
        end
