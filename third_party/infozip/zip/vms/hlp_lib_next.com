$!                                              21 November 2004.  SMS.
$!
$!    HLP_LIB_NEXT.COM
$!
$!    Find the next available HLP$LIBRARY[_*] logical name.
$!
$ base = "HLP$LIBRARY"
$ candidate = base
$ i = 0
$!
$ loop_top:
$    if (i .gt. 0) then candidate = base+ "_"+ f$string( i)
$    i = i+ 1
$    if (f$trnlnm( candidate) .nes. "") then goto loop_top
$!
$ write sys$output candidate
$!
