$! BUILD_ZIP.COM
$!
$!     Build procedure for VMS versions of Zip.
$!
$!     last revised:  2007-03-15  SMS.
$!
$!     Command arguments:
$!     - suppress help file processing: "NOHELP"
$!     - suppress message file processing: "NOMSG"
$!     - select link-only: "LINK"
$!     - select compiler environment: "VAXC", "DECC", "GNUC"
$!     - select large-file support: "LARGE"
$!     - select compiler listings: "LIST"  Note that the whole argument
$!       is added to the compiler command, so more elaborate options
$!       like "LIST/SHOW=ALL" (quoted or space-free) may be specified.
$!     - supply additional compiler options: "CCOPTS=xxx"  Allows the
$!       user to add compiler command options like /ARCHITECTURE or
$!       /[NO]OPTIMIZE.  For example, CCOPTS=/ARCH=HOST/OPTI=TUNE=HOST
$!       or CCOPTS=/DEBUG/NOOPTI.  These options must be quoted or
$!       space-free.
$!     - supply additional linker options: "LINKOPTS=xxx"  Allows the
$!       user to add linker command options like /DEBUG or /MAP.  For
$!       example: LINKOPTS=/DEBUG or LINKOPTS=/MAP/CROSS.  These options
$!       must be quoted or space-free.  Default is
$!       LINKOPTS=/NOTRACEBACK, but if the user specifies a LINKOPTS
$!       string, /NOTRACEBACK will not be included unless specified by
$!       the user.
$!     - select installation of CLI interface version of zip:
$!       "VMSCLI" or "CLI"
$!     - force installation of UNIX interface version of zip
$!       (override LOCAL_ZIP environment): "NOVMSCLI" or "NOCLI"
$!     - select BZIP2 support: "IZ_BZIP2=dev:[dir]", where "dev:[dir]"
$!       (or a suitable logical name) tells where to find "bzlib.h".
$!       The BZIP2 object library (LIBBZ2_NS.OLB) is expected to be in
$!       a "[.dest]" directory under that one ("dev:[dir.ALPHAL]", for
$!       example), or in that directory itself.
$!
$!     To specify additional options, define the global symbol
$!     LOCAL_ZIP as a comma-separated list of the C macros to be
$!     defined, and then run BUILD_ZIP.COM.  For example:
$!
$!             $ LOCAL_ZIP == "VMS_IM_EXTRA"
$!             $ @ [.VMS]BUILD_ZIP.COM
$!
$!     Valid VMS-specific options include VMS_PK_EXTRA and VMS_IM_EXTRA. 
$!     See the INSTALL file for other options.  (VMS_PK_EXTRA is the
$!     default.)
$!
$!     If editing this procedure to set LOCAL_ZIP, be sure to use only
$!     one "=", to avoid affecting other procedures.  For example:
$!             $ LOCAL_ZIP = "VMS_IM_EXTRA"
$!
$!     Note: This command procedure always generates both the "default"
$!     Zip having the UNIX style command interface and the "VMSCLI" Zip
$!     having the CLI compatible command interface.  There is no need to
$!     add "VMSCLI" to the LOCAL_ZIP symbol.  (The only effect of
$!     "VMSCLI" now is the selection of the CLI style Zip executable in
$!     the foreign command definition.)
$!
$!
$ on error then goto error
$ on control_y then goto error
$ OLD_VERIFY = f$verify( 0)
$!
$ edit := edit                  ! override customized edit commands
$ say := write sys$output
$!
$!##################### Read settings from environment ########################
$!
$ if (f$type( LOCAL_ZIP) .eqs. "")
$ then
$     LOCAL_ZIP = ""
$ else  ! Trim blanks and append comma if missing
$     LOCAL_ZIP = f$edit( LOCAL_ZIP, "TRIM")
$     if (f$extract( f$length( LOCAL_ZIP)- 1, 1, LOCAL_ZIP) .nes. ",")
$     then
$         LOCAL_ZIP = LOCAL_ZIP + ","
$     endif
$ endif
$!
$! Check for the presence of "VMSCLI" in LOCAL_ZIP.  If yes, we will
$! define the foreign command for "zip" to use the executable
$! containing the CLI interface.
$!
$ len_local_zip = f$length( LOCAL_ZIP)
$!
$ pos_cli = f$locate( "VMSCLI", LOCAL_ZIP)
$ if (pos_cli .ne. len_local_zip)
$ then
$     CLI_IS_DEFAULT = 1
$     ! Remove "VMSCLI" macro from LOCAL_ZIP. The Zip executable
$     ! including the CLI interface is now created unconditionally.
$     LOCAL_ZIP = f$extract( 0, pos_cli, LOCAL_ZIP)+ -
       f$extract( pos_cli+7, len_local_zip- (pos_cli+ 7), LOCAL_ZIP)
$ else
$     CLI_IS_DEFAULT = 0
$ endif
$ delete /symbol /local pos_cli
$!
$! Check for the presence of "VMS_IM_EXTRA" in LOCAL_ZIP.  If yes, we
$! will (later) add "I" to the destination directory name.
$!
$ desti = ""
$ pos_im = f$locate( "VMS_IM_EXTRA", LOCAL_ZIP)
$ if (pos_im .ne. len_local_zip)
$ then
$    desti = "I"
$ endif
$!
$ delete /symbol /local len_local_zip
$!
$!##################### Customizing section #############################
$!
$ zipx_unx = "ZIP"
$ zipx_cli = "ZIP_CLI"
$!
$ CCOPTS = ""
$ IZ_BZIP2 = ""
$ LINKOPTS = "/notraceback"
$ LINK_ONLY = 0
$ LISTING = " /nolist"
$ LARGE_FILE = 0
$ MAKE_HELP = 1
$ MAKE_MSG = 1
$ MAY_USE_DECC = 1
$ MAY_USE_GNUC = 0
$!
$! Process command line parameters requesting optional features.
$!
$ arg_cnt = 1
$ argloop:
$     current_arg_name = "P''arg_cnt'"
$     curr_arg = f$edit( 'current_arg_name', "UPCASE")
$     if (curr_arg .eqs. "") then goto argloop_out
$!
$     if (f$extract( 0, 5, curr_arg) .eqs. "CCOPT")
$     then
$         opts = f$edit( curr_arg, "COLLAPSE")
$         eq = f$locate( "=", opts)
$         CCOPTS = f$extract( (eq+ 1), 1000, opts)
$         goto argloop_end
$     endif
$!
$     if f$extract( 0, 7, curr_arg) .eqs. "IZ_BZIP"
$     then
$         opts = f$edit( curr_arg, "COLLAPSE")
$         eq = f$locate( "=", opts)
$         IZ_BZIP2 = f$extract( (eq+ 1), 1000, opts)
$         goto argloop_end
$     endif
$!
$     if (f$extract( 0, 5, curr_arg) .eqs. "LARGE")
$     then
$         LARGE_FILE = 1
$         goto argloop_end
$     endif
$!
$     if (f$extract( 0, 7, curr_arg) .eqs. "LINKOPT")
$     then
$         opts = f$edit( curr_arg, "COLLAPSE")
$         eq = f$locate( "=", opts)
$         LINKOPTS = f$extract( (eq+ 1), 1000, opts)
$         goto argloop_end
$     endif
$!
$! Note: LINK test must follow LINKOPTS test.
$!
$     if (f$extract( 0, 4, curr_arg) .eqs. "LINK")
$     then
$         LINK_ONLY = 1
$         goto argloop_end
$     endif
$!
$     if (f$extract( 0, 4, curr_arg) .eqs. "LIST")
$     then
$         LISTING = "/''curr_arg'"      ! But see below for mods.
$         goto argloop_end
$     endif
$!
$     if (curr_arg .eqs. "NOHELP")
$     then
$         MAKE_HELP = 0
$         goto argloop_end
$     endif
$!
$     if (curr_arg .eqs. "NOMSG")
$     then
$         MAKE_MSG = 0
$         goto argloop_end
$     endif
$!
$     if (curr_arg .eqs. "VAXC")
$     then
$         MAY_USE_DECC = 0
$         MAY_USE_GNUC = 0
$         goto argloop_end
$     endif
$!
$     if (curr_arg .eqs. "DECC")
$     then
$         MAY_USE_DECC = 1
$         MAY_USE_GNUC = 0
$         goto argloop_end
$     endif
$!
$     if (curr_arg .eqs. "GNUC")
$     then
$         MAY_USE_DECC = 0
$         MAY_USE_GNUC = 1
$         goto argloop_end
$     endif
$!
$     if ((curr_arg .eqs. "VMSCLI") .or. (curr_arg .eqs. "CLI"))
$     then
$         CLI_IS_DEFAULT = 1
$         goto argloop_end
$     endif
$!
$     if ((curr_arg .eqs. "NOVMSCLI") .or. (curr_arg .eqs. "NOCLI"))
$     then
$         CLI_IS_DEFAULT = 0
$         goto argloop_end
$     endif
$!
$     say "Unrecognized command-line option: ''curr_arg'"
$     goto error
$!
$     argloop_end:
$     arg_cnt = arg_cnt + 1
$ goto argloop
$ argloop_out:
$!
$ if (CLI_IS_DEFAULT)
$ then
$     ZIPEXEC = zipx_cli
$ else
$     ZIPEXEC = zipx_unx
$ endif
$!
$!#######################################################################
$!
$! Find out current disk, directory, compiler and options
$!
$ workdir = f$environment( "default")
$ here = f$parse( workdir, , , "device")+ f$parse( workdir, , , "directory")
$!
$! Sense the host architecture (Alpha, Itanium, or VAX).
$!
$ if (f$getsyi( "HW_MODEL") .lt. 1024)
$ then
$     arch = "VAX"
$ else
$     if (f$getsyi( "ARCH_TYPE") .eq. 2)
$     then
$         arch = "ALPHA"
$     else
$         if (f$getsyi( "ARCH_TYPE") .eq. 3)
$         then
$             arch = "IA64"
$         else
$             arch = "unknown_arch"
$         endif
$     endif
$ endif
$!
$ dest = arch
$ cmpl = "DEC/Compaq/HP C"
$ opts = ""
$ if (arch .nes. "VAX")
$ then
$     HAVE_DECC_VAX = 0
$     USE_DECC_VAX = 0
$!
$     if (MAY_USE_GNUC)
$     then
$         say "GNU C is not supported for ''arch'."
$         say "You must use DEC/Compaq/HP C to build Zip."
$         goto error
$     endif
$!
$     if (.not. MAY_USE_DECC)
$     then
$         say "VAX C is not supported for ''arch'."
$         say "You must use DEC/Compaq/HP C to build Zip."
$         goto error
$     endif
$!
$     cc = "cc /standard = relax /prefix = all /ansi"
$     defs = "''LOCAL_ZIP' VMS"
$     if (LARGE_FILE .ne. 0)
$     then
$         defs = "LARGE_FILE_SUPPORT, ''defs'"
$     endif
$ else
$     if (LARGE_FILE .ne. 0)
$     then
$        say "LARGE_FILE_SUPPORT is not available on VAX."
$        LARGE_FILE = 0
$     endif
$     HAVE_DECC_VAX = (f$search( "SYS$SYSTEM:DECC$COMPILER.EXE") .nes. "")
$     HAVE_VAXC_VAX = (f$search( "SYS$SYSTEM:VAXC.EXE") .nes. "")
$     MAY_HAVE_GNUC = (f$trnlnm( "GNU_CC") .nes. "")
$     if (HAVE_DECC_VAX .and. MAY_USE_DECC)
$     then
$         ! We use DECC:
$         USE_DECC_VAX = 1
$         cc = "cc /decc /prefix = all"
$         defs = "''LOCAL_ZIP' VMS"
$     else
$         ! We use VAXC (or GNU C):
$         USE_DECC_VAX = 0
$         defs = "''LOCAL_ZIP' VMS"
$         if ((.not. HAVE_VAXC_VAX .and. MAY_HAVE_GNUC) .or. MAY_USE_GNUC)
$         then
$             cc = "gcc"
$             opts = "GNU_CC:[000000]GCCLIB.OLB /LIBRARY,"
$             dest = "''dest'G"
$             cmpl = "GNU C"
$         else
$             if (HAVE_DECC_VAX)
$             then
$                 cc = "cc /vaxc"
$             else
$                 cc = "cc"
$             endif
$             dest = "''dest'V"
$             cmpl = "VAC C"
$         endif
$         opts = "''opts' SYS$DISK:[.''dest']VAXCSHR.OPT /OPTIONS,"
$     endif
$ endif
$!
$! Change the destination directory, according to the VMS_IM_EXTRA and
$! large-file options.  Set the bzip2 directory.
$!
$ dest = dest+ desti
$ seek_bz = arch
$ if (LARGE_FILE .ne. 0)
$ then
$     dest = dest+ "L"
$     seek_bz = seek_bz+ "L"
$ endif
$!
$! If BZIP2 support was selected, find the object library.
$! Complain if things fail.
$!
$ cc_incl = "[]"
$ incl_bzip2_m = ""
$ lib_bzip2_opts = ""
$ if (IZ_BZIP2 .nes. "")
$ then
$     bz2_olb = "LIBBZ2_NS.OLB"
$     define incl_bzip2 'IZ_BZIP2'
$     defs = "''defs', BZIP2_SUPPORT"
$     @ [.VMS]FIND_BZIP2_LIB.COM 'IZ_BZIP2' 'seek_bz' 'bz2_olb' lib_bzip2
$     if (f$trnlnm( "lib_bzip2") .eqs. "")
$     then
$         say "Can't find BZIP2 object library.  Can't link."
$         goto error
$     else
$         say "BZIP2 dir = ''f$trnlnm( "lib_bzip2")'"
$         incl_bzip2_m = ", ZBZ2ERR"
$         lib_bzip2_opts = "lib_bzip2:''bz2_olb' /library, "
$         cc_incl = cc_incl+ ", [.VMS]"
$     endif
$ endif
$!
$! Reveal the plan.  If compiling, set some compiler options.
$!
$ if (LINK_ONLY)
$ then
$     say "Linking on ''arch' for ''cmpl'."
$ else
$     say "Compiling on ''arch' using ''cmpl'."
$!
$     DEF_UNX = "/define = (''defs')"
$     DEF_CLI = "/define = (''defs', VMSCLI)"
$     DEF_UTIL = "/define = (''defs', UTIL)"
$ endif
$!
$! If [.'dest'] does not exist, either complain (link-only) or make it.
$!
$ if (f$search( "''dest'.DIR;1") .eqs. "")
$ then
$     if (LINK_ONLY)
$     then
$         say "Can't find directory ""[.''dest']"".  Can't link."
$         goto error
$     else
$         create /directory [.'dest']
$     endif
$ endif
$!
$ if (.not. LINK_ONLY)
$ then
$!
$! Arrange to get arch-specific list file placement, if LISTING, and if
$! the user didn't specify a particular "/LIST =" destination.
$!
$     L = f$edit( LISTING, "COLLAPSE")
$     if ((f$extract( 0, 5, L) .eqs. "/LIST") .and. -
       (f$extract( 4, 1, L) .nes. "="))
$     then
$         LISTING = " /LIST = [.''dest']"+ f$extract( 5, 1000, LISTING)
$     endif
$!
$! Define compiler command.
$!
$     cc = cc+ " /include = (''cc_incl')"+ LISTING+ CCOPTS
$!
$ endif
$!
$! Define linker command.
$!
$ link = "link ''LINKOPTS'"
$!
$! Make a VAXCRTL options file for GNU C or VAC C, if needed.
$!
$ if ((opts .nes. "") .and. -
   (f$locate( "VAXCSHR", f$edit( opts, "UPCASE")) .lt. f$length( opts)) .and. -
   (f$search( "[.''dest']VAXCSHR.OPT") .eqs. ""))
$ then
$     open /write opt_file_ln [.'dest']VAXCSHR.OPT
$     write opt_file_ln "SYS$SHARE:VAXCRTL.EXE /SHARE"
$     close opt_file_ln
$ endif
$!
$! Show interesting facts.
$!
$ say "   architecture = ''arch' (destination = [.''dest'])"
$ if (.not. LINK_ONLY)
$ then
$     say "   cc = ''cc'"
$ endif
$ say "   link = ''link'"
$ if (.not. MAKE_HELP)
$ then
$     say "   Not making new help files."
$ endif
$ say ""
$ if (.not. MAKE_MSG)
$ then
$     say "   Not making new message files."
$ endif
$ say ""
$!
$ tmp = f$verify( 1)    ! Turn echo on to see what's happening.
$!
$!-------------------------------- Zip section -------------------------------
$!
$ if (.not. LINK_ONLY)
$ then
$!
$! Process the help file, if desired.
$!
$     if (MAKE_HELP)
$     then
$         runoff /out = ZIP.HLP [.VMS]VMS_ZIP.RNH
$     endif
$!
$! Process the message file, if desired.
$!
$     if (MAKE_MSG)
$     then
$!
$! Create the message source file first, if it's not found.
$!
$         if (f$search( "[.VMS]ZIP_MSG.MSG") .eqs. "")
$         then
$             cc /include = [] /object = [.'dest']VMS_MSG_GEN.OBJ -
               [.VMS]VMS_MSG_GEN.C
$             link /executable = [.'dest']VMS_MSG_GEN.EXE -
               [.'dest']VMS_MSG_GEN.OBJ
$             create /fdl = [.VMS]STREAM_LF.FDL [.VMS]ZIP_MSG.MSG
$             define /user_mode sys$output [.VMS]ZIP_MSG.MSG
$             run [.'dest']VMS_MSG_GEN.EXE
$             purge [.VMS]ZIP_MSG.MSG
$             delete [.'dest']VMS_MSG_GEN.EXE;*, -
               [.'dest']VMS_MSG_GEN.OBJ;*
$         endif
$!
$         message /object = [.'dest']ZIP_MSG.OBJ /nosymbols -
           [.VMS]ZIP_MSG.MSG
$         link /shareable = [.'dest']ZIP_MSG.EXE [.'dest']ZIP_MSG.OBJ
$     endif
$!
$! Compile the sources.
$!
$     cc 'DEF_UNX' /object = [.'dest']ZIP.OBJ ZIP.C
$     cc 'DEF_UNX' /object = [.'dest']CRC32.OBJ CRC32.C
$     cc 'DEF_UNX' /object = [.'dest']CRYPT.OBJ CRYPT.C
$     cc 'DEF_UNX' /object = [.'dest']DEFLATE.OBJ DEFLATE.C
$     cc 'DEF_UNX' /object = [.'dest']FILEIO.OBJ FILEIO.C
$     cc 'DEF_UNX' /object = [.'dest']GLOBALS.OBJ GLOBALS.C
$     cc 'DEF_UNX' /object = [.'dest']TREES.OBJ TREES.C
$     cc 'DEF_UNX' /object = [.'dest']TTYIO.OBJ TTYIO.C
$     cc 'DEF_UNX' /object = [.'dest']UTIL.OBJ UTIL.C
$     cc 'DEF_UNX' /object = [.'dest']ZBZ2ERR.OBJ ZBZ2ERR.C
$     cc 'DEF_UNX' /object = [.'dest']ZIPFILE.OBJ ZIPFILE.C
$     cc 'DEF_UNX' /object = [.'dest']ZIPUP.OBJ ZIPUP.C
$     cc /include = [] 'DEF_UNX' /object = [.'dest']VMS.OBJ -
       [.VMS]VMS.C
$     cc /include = [] 'DEF_UNX' /object = [.'dest']VMSMUNCH.OBJ -
       [.VMS]VMSMUNCH.C
$     cc /include = [] 'DEF_UNX' /object = [.'dest']VMSZIP.OBJ -
       [.VMS]VMSZIP.C
$!
$! Create the object library.
$!
$     if (f$search( "[.''dest']ZIP.OLB") .eqs. "") then -
       libr /object /create [.'dest']ZIP.OLB
$!
$     libr /object /replace [.'dest']ZIP.OLB -
       [.'dest']CRC32.OBJ, -
       [.'dest']CRYPT.OBJ, -
       [.'dest']DEFLATE.OBJ, -
       [.'dest']FILEIO.OBJ, -
       [.'dest']GLOBALS.OBJ, -
       [.'dest']TREES.OBJ, -
       [.'dest']TTYIO.OBJ, -
       [.'dest']UTIL.OBJ, -
       [.'dest']ZBZ2ERR.OBJ, -
       [.'dest']ZIPFILE.OBJ, -
       [.'dest']ZIPUP.OBJ, -
       [.'dest']VMS.OBJ, -
       [.'dest']VMSMUNCH.OBJ, -
       [.'dest']VMSZIP.OBJ
$!
$ endif
$!
$! Link the executable.
$!
$ link /executable = [.'dest']'ZIPX_UNX'.EXE -
   [.'dest']ZIP.OBJ, -
   [.'dest']ZIP.OLB /include = (GLOBALS 'incl_bzip2_m') /library, -
   'lib_bzip2_opts' -
   'opts' -
   SYS$DISK:[.VMS]ZIP.OPT /options
$!
$!------------------------ Zip (CLI interface) section -----------------------
$!
$ if (.not. LINK_ONLY)
$ then
$!
$! Process the CLI help file, if desired.
$!
$     if (MAKE_HELP)
$     then
$         set default [.VMS]
$         edit /tpu /nosection /nodisplay /command = cvthelp.tpu -
           zip_cli.help
$         set default [-]
$         runoff /output = ZIP_CLI.HLP [.VMS]ZIP_CLI.RNH
$     endif
$!
$! Compile the CLI sources.
$!
$     cc 'DEF_CLI' /object = [.'dest']ZIPCLI.OBJ ZIP.C
$     cc /include = [] 'DEF_CLI' /object = [.'dest']CMDLINE.OBJ -
       [.VMS]CMDLINE.C
$!
$! Create the command definition object file.
$!
$     set command /object = [.'dest']ZIP_CLI.OBJ [.VMS]ZIP_CLI.CLD
$!
$! Create the CLI object library.
$!
$     if (f$search( "[.''dest']ZIPCLI.OLB") .eqs. "") then -
       libr /object /create [.'dest']ZIPCLI.OLB
$!
$     libr /object /replace [.'dest']ZIPCLI.OLB -
       [.'dest']ZIPCLI.OBJ, -
       [.'dest']CMDLINE.OBJ, -
       [.'dest']ZIP_CLI.OBJ
$!
$ endif
$!
$! Link the CLI executable.
$!
$ link /executable = [.'dest']'ZIPX_CLI'.EXE -
   [.'dest']ZIPCLI.OBJ, -
   [.'dest']ZIPCLI.OLB /library, -
   [.'dest']ZIP.OLB /include = (GLOBALS 'incl_bzip2_m') /library, -
   'lib_bzip2_opts' -
   'opts' -
   SYS$DISK:[.VMS]ZIP.OPT /options
$!
$!--------------------------- Zip utilities section --------------------------
$!
$ if (.not. LINK_ONLY)
$ then
$!
$! Compile the variant Zip utilities library sources.
$!
$     cc 'DEF_UTIL' /object = [.'dest']CRC32_.OBJ CRC32.C
$     cc 'DEF_UTIL' /object = [.'dest']CRYPT_.OBJ CRYPT.C
$     cc 'DEF_UTIL' /object = [.'dest']FILEIO_.OBJ FILEIO.C
$     cc 'DEF_UTIL' /object = [.'dest']UTIL_.OBJ UTIL.C
$     cc 'DEF_UTIL' /object = [.'dest']ZIPFILE_.OBJ ZIPFILE.C
$     cc 'DEF_UTIL' /include = [] /object = [.'dest']VMS_.OBJ [.VMS]VMS.C
$!
$! Create the Zip utilities object library.
$!
$     if f$search( "[.''dest']ZIPUTILS.OLB") .eqs. "" then -
       libr /object /create [.'dest']ZIPUTILS.OLB
$!
$     libr /object /replace [.'dest']ZIPUTILS.OLB -
       [.'dest']CRC32_.OBJ, -
       [.'dest']CRYPT_.OBJ, -
       [.'dest']FILEIO_.OBJ, -
       [.'dest']GLOBALS.OBJ, -
       [.'dest']TTYIO.OBJ, -
       [.'dest']UTIL_.OBJ, -
       [.'dest']ZIPFILE_.OBJ, -
       [.'dest']VMS_.OBJ, -
       [.'dest']VMSMUNCH.OBJ
$!
$! Compile the Zip utilities main program sources.
$!
$     cc 'DEF_UTIL' /object = [.'dest']ZIPCLOAK.OBJ ZIPCLOAK.C
$     cc 'DEF_UTIL' /object = [.'dest']ZIPNOTE.OBJ ZIPNOTE.C
$     cc 'DEF_UTIL' /object = [.'dest']ZIPSPLIT.OBJ ZIPSPLIT.C
$!
$ endif
$!
$! Link the Zip utilities executables.
$!
$ link /executable = [.'dest']ZIPCLOAK.EXE -
   [.'dest']ZIPCLOAK.OBJ, -
   [.'dest']ZIPUTILS.OLB /include = (GLOBALS) /library, -
   'opts' -
   SYS$DISK:[.VMS]ZIP.OPT /options
$!
$ link /executable = [.'dest']ZIPNOTE.EXE -
   [.'dest']ZIPNOTE.OBJ, -
   [.'dest']ZIPUTILS.OLB /include = (GLOBALS) /library, -
   'opts' -
   SYS$DISK:[.VMS]ZIP.OPT /OPTIONS
$!
$ LINK /EXECUTABLE = [.'DEST']ZIPSPLIT.EXE -
   [.'DEST']ZIPSPLIT.OBJ, -
   [.'DEST']ZIPUTILS.OLB /INCLUDE = (globals) /LIBRARY, -
   'opts' -
   SYS$DISK:[.VMS]ZIP.OPT /options
$!
$!----------------------- Logical name removal section -----------------------
$!
$ if (IZ_BZIP2 .nes. "")
$ then
$     if (f$trnlnm( "incl_bzip2", "LNM$PROCESS_TABLE") .nes. "")
$     then
$         deassign incl_bzip2
$     endif
$     if (f$trnlnm( "lib_bzip2", "LNM$PROCESS_TABLE") .nes. "")
$     then
$         deassign lib_bzip2
$     endif
$ endif
$!
$!------------------------------ Symbols section -----------------------------
$!
$ there = here- "]"+ ".''dest']"
$!
$! Define the foreign command symbols.  Similar commands may be useful
$! in SYS$MANAGER:SYLOGIN.COM and/or users' LOGIN.COM.
$!
$ zip      == "$''there'''ZIPEXEC'.exe"
$ zipcloak == "$''there'zipcloak.exe"
$ zipnote  == "$''there'zipnote.exe"
$ zipsplit == "$''there'zipsplit.exe"
$!
$! Restore the original default directory and DCL verify status.
$!
$ error:
$!
$ if (f$type( here) .nes. "")
$ then
$     if (here .nes. "")
$     then
$         set default 'here'
$     endif
$ endif
$!
$ if (f$type( OLD_VERIFY) .nes. "")
$ then
$     tmp = f$verify( OLD_VERIFY)
$ endif
$!
$ exit
$!
