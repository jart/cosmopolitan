$!                                              3 March 2005.  SMS.
$!
$! Info-ZIP VMS accessory procedure.
$!
$!    Modify a dependencies file (P1), changing the object file name to
$!    P2.
$!    P3 = output file specification.
$!
$!
$ prefix = f$edit( p3, "COLLAPSE")
$!
$! Strip any device:[directory] from P2.
$!
$ obj_name = f$parse( P2, , , "NAME", "SYNTAX_ONLY")+ -
   f$parse( P2, , , "TYPE", "SYNTAX_ONLY")
$!
$ open /read /error = end_main deps_in 'p1'
$ open /write /error = end_main deps_out 'p3'
$ on error then goto loop_main_end
$ loop_main_top:
$     read /error = loop_main_end deps_in line
$     line_reduced = f$edit( line, "COMPRESS, TRIM")
$     colon = f$locate( " : ", line_reduced)
$     line = obj_name+ f$extract( colon, 2000, line)
$     write deps_out "''line'"
$ goto loop_main_top
$!
$ loop_main_end:
$ close deps_in
$ close deps_out
$!
$ end_main:
$!
