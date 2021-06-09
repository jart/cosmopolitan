$!                                              1 December 2006.  SMS.
$!
$! Info-ZIP VMS accessory procedure.
$!
$!    For the product named by P1,
$!    collect all source file dependencies specified by P3,
$!    and add P4 prefix.
$!    Convert absolute dependencies to relative from one level above P5.
$!    P2 = output file specification.
$!
$! MMS /EXTENDED_SYNTAX can't easily pass a macro invocation for P4, so
$! we remove any internal spaces which might have been added to prevent
$! immediate evaluation of a macro invocation.
$!
$ prefix = f$edit( p4, "COLLAPSE")
$!
$ dev_lose = f$edit( f$parse( p5, , , "DEVICE", "SYNTAX_ONLY"), "UPCASE")
$ dir_lose = f$edit( f$parse( p5, , , "DIRECTORY", "SYNTAX_ONLY"), "UPCASE")
$ suffix = ".VMS]"
$ suffix_loc = f$locate( suffix, dir_lose)
$ if (suffix_loc .lt f$length( dir_lose))
$ then
$    dev_dir_lose = dev_lose+ dir_lose- suffix
$ else
$    dev_dir_lose = dev_lose+ dir_lose- "]"
$ endif
$!
$! For portability, make the output file record format Stream_LF.
$!
$ create /fdl = sys$input 'p2'
RECORD
        Carriage_Control carriage_return
        Format stream_lf
$!
$ open /read /write /error = end_main deps_out 'p2'
$ on error then goto loop_main_end
$!
$! Include proper-inclusion-check preface.
$!
$ incl_macro = "INCL_"+ f$parse( p2, , , "NAME", "SYNTAX_ONLY")
$ write deps_out "#"
$ write deps_out "# ''p1' for VMS - MMS (or MMK) Source Dependency File."
$ write deps_out "#"
$ write deps_out ""
$ write deps_out -
   "# This description file is included by other description files.  It is"
$ write deps_out -
   "# not intended to be used alone.  Verify proper inclusion."
$ write deps_out ""
$ write deps_out ".IFDEF ''incl_macro'"
$ write deps_out ".ELSE"
$ write deps_out -
   "$$$$ THIS DESCRIPTION FILE IS NOT INTENDED TO BE USED THIS WAY."
$ write deps_out ".ENDIF"
$ write deps_out ""
$!
$! Actual dependencies from individual dependency files.
$!
$ loop_main_top:
$    file = f$search( p3)
$    if (file .eqs. "") then goto loop_main_end
$!
$    open /read /error = end_subs deps_in 'file'
$    loop_subs_top:
$       read /error = loop_subs_end deps_in line
$       line_reduced = f$edit( line, "COMPRESS, TRIM, UPCASE")
$       colon = f$locate( " : ", line_reduced)
$       d_d_l_loc = f$locate( dev_dir_lose, -
         f$extract( (colon+ 3), 1000, line_reduced))
$       if (d_d_l_loc .eq. 0)
$       then
$          front = f$extract( 0, (colon+ 3), line_reduced)
$          back = f$extract( (colon+ 3+ f$length( dev_dir_lose)), -
            1000, line_reduced)
$          line = front+ "["+ back
$       endif
$       write deps_out "''prefix'"+ "''line'"
$    goto loop_subs_top
$!
$    loop_subs_end:
$    close deps_in
$!
$ goto loop_main_top
$!
$ loop_main_end:
$ close deps_out
$!
$ end_main:
$!
