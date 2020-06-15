c Length of character string, excluding trailing blanks
c Same thing as LEN_TRIM()

	integer function trmlen(t)

	implicit none

c	Parameter:
	character t*(*)

      do 1 trmlen=LEN(t),1,-1
    1    if(t(trmlen:trmlen).ne.' ')RETURN
      trmlen=1
      end ! of integer function trmlen
