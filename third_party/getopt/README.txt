GETOPT(3)         Cosmopolitan Library Functions Manual              GETOPT(3)

𝐍𝐀𝐌𝐄
     𝗴𝗲𝘁𝗼𝗽𝘁 — get option character from command line argument list

𝐒𝐘𝐍𝐎𝐏𝐒𝐈𝐒
     #𝗶𝗻𝗰𝗹𝘂𝗱𝗲 <𝘂𝗻𝗶𝘀𝘁𝗱.𝗵>

     e̲x̲t̲e̲r̲n̲ c̲h̲a̲r̲ *̲o̲p̲t̲a̲r̲g̲;̲
     e̲x̲t̲e̲r̲n̲ i̲n̲t̲ o̲p̲t̲e̲r̲r̲;̲
     e̲x̲t̲e̲r̲n̲ i̲n̲t̲ o̲p̲t̲i̲n̲d̲;̲
     e̲x̲t̲e̲r̲n̲ i̲n̲t̲ o̲p̲t̲o̲p̲t̲;̲
     e̲x̲t̲e̲r̲n̲ i̲n̲t̲ o̲p̲t̲r̲e̲s̲e̲t̲;̲

     i̲n̲t̲
     𝗴𝗲𝘁𝗼𝗽𝘁(i̲n̲t̲ a̲r̲g̲c̲, c̲h̲a̲r̲ *̲ c̲o̲n̲s̲t̲ *̲a̲r̲g̲v̲, c̲o̲n̲s̲t̲ c̲h̲a̲r̲ *̲o̲p̲t̲s̲t̲r̲i̲n̲g̲);

𝐃𝐄𝐒𝐂𝐑𝐈𝐏𝐓𝐈𝐎𝐍
     The 𝗴𝗲𝘁𝗼𝗽𝘁() function incrementally parses a command line argument
     list a̲r̲g̲v̲ and returns the next k̲n̲o̲w̲n̲ option character.  An option
     character is k̲n̲o̲w̲n̲ if it has been specified in the string of
     accepted option characters, o̲p̲t̲s̲t̲r̲i̲n̲g̲.

     The option string o̲p̲t̲s̲t̲r̲i̲n̲g̲ may contain the following elements:
     individual characters, characters followed by a colon, and charac‐
     ters followed by two colons.  A character followed by a single
     colon indicates that an argument is to follow the option on the
     command line.  Two colons indicates that the argument is optional -
     this is an extension not covered by POSIX.  For example, an option
     string "x" recognizes an option -𝘅, and an option string "x:" rec‐
     ognizes an option and argument -𝘅 a̲r̲g̲u̲m̲e̲n̲t̲.  It does not matter to
     𝗴𝗲𝘁𝗼𝗽𝘁() if a following argument has leading whitespace; except in
     the case where the argument is optional, denoted with two colons,
     no leading whitespace is permitted.

     On return from 𝗴𝗲𝘁𝗼𝗽𝘁(), o̲p̲t̲a̲r̲g̲ points to an option argument, if it
     is anticipated, and the variable o̲p̲t̲i̲n̲d̲ contains the index to the
     next a̲r̲g̲v̲ argument for a subsequent call to 𝗴𝗲𝘁𝗼𝗽𝘁().

     The variables o̲p̲t̲e̲r̲r̲ and o̲p̲t̲i̲n̲d̲ are both initialized to 1.  The
     o̲p̲t̲i̲n̲d̲ variable may be set to another value larger than 0 before a
     set of calls to 𝗴𝗲𝘁𝗼𝗽𝘁() in order to skip over more or less a̲r̲g̲v̲
     entries.  An o̲p̲t̲i̲n̲d̲ value of 0 is reserved for compatibility with
     GNU 𝗴𝗲𝘁𝗼𝗽𝘁().

     In order to use 𝗴𝗲𝘁𝗼𝗽𝘁() to evaluate multiple sets of arguments, or
     to evaluate a single set of arguments multiple times, the variable
     o̲p̲t̲r̲e̲s̲e̲t̲ must be set to 1 before the second and each additional set
     of calls to 𝗴𝗲𝘁𝗼𝗽𝘁(), and the variable o̲p̲t̲i̲n̲d̲ must be reinitial‐
     ized.

     The 𝗴𝗲𝘁𝗼𝗽𝘁() function returns -1 when the argument list is
     exhausted.  The interpretation of options in the argument list may
     be cancelled by the option ‘--’ (double dash) which causes 𝗴𝗲𝘁𝗼𝗽𝘁()
     to signal the end of argument processing and return -1.  When all
     options have been processed (i.e., up to the first non-option argu‐
     ment), 𝗴𝗲𝘁𝗼𝗽𝘁() returns -1.

𝐑𝐄𝐓𝐔𝐑𝐍 𝐕𝐀𝐋𝐔𝐄𝐒
     The 𝗴𝗲𝘁𝗼𝗽𝘁() function returns the next known option character in
     o̲p̲t̲s̲t̲r̲i̲n̲g̲.  If 𝗴𝗲𝘁𝗼𝗽𝘁() encounters a character not found in
     o̲p̲t̲s̲t̲r̲i̲n̲g̲ or if it detects a missing option argument, it returns
     ‘?’ (question mark).  If o̲p̲t̲s̲t̲r̲i̲n̲g̲ has a leading ‘:’ then a missing
     option argument causes ‘:’ to be returned instead of ‘?’.  In
     either case, the variable o̲p̲t̲o̲p̲t̲ is set to the character that
     caused the error.  The 𝗴𝗲𝘁𝗼𝗽𝘁() function returns -1 when the argu‐
     ment list is exhausted.

𝐄𝐗𝐀𝐌𝐏𝐋𝐄𝐒
     The following code accepts the options -𝗯 and -𝗳 a̲r̲g̲u̲m̲e̲n̲t̲ and
     adjusts a̲r̲g̲c̲ and a̲r̲g̲v̲ after option argument processing has com‐
     pleted.

           int bflag, ch, fd;

           bflag = 0;
           while ((ch = getopt(argc, argv, "bf:")) != -1) {
                   switch (ch) {
                   case 'b':
                           bflag = 1;
                           break;
                   case 'f':
                           if ((fd = open(optarg, O_RDONLY, 0)) == -1)
                                   err(1, "%s", optarg);
                           break;
                   default:
                           usage();
                   }
           }
           argc -= optind;
           argv += optind;

𝐃𝐈𝐀𝐆𝐍𝐎𝐒𝐓𝐈𝐂𝐒
     If the 𝗴𝗲𝘁𝗼𝗽𝘁() function encounters a character not found in the
     string o̲p̲t̲s̲t̲r̲i̲n̲g̲ or detects a missing option argument, it writes an
     error message to s̲t̲d̲e̲r̲r̲ and returns ‘?’.  Setting o̲p̲t̲e̲r̲r̲ to a zero
     will disable these error messages.  If o̲p̲t̲s̲t̲r̲i̲n̲g̲ has a leading ‘:’
     then a missing option argument causes a ‘:’ to be returned in addi‐
     tion to suppressing any error messages.

     Option arguments are allowed to begin with ‘-’; this is reasonable
     but reduces the amount of error checking possible.

𝐒𝐄𝐄 𝐀𝐋𝐒𝐎
     getopt(1), getopt_long(3), getsubopt(3)

𝐒𝐓𝐀𝐍𝐃𝐀𝐑𝐃𝐒
     The 𝗴𝗲𝘁𝗼𝗽𝘁() function implements a superset of the functionality
     specified by IEEE Std 1003.1 (“POSIX.1”).

     The following extensions are supported:

     ·   The o̲p̲t̲r̲e̲s̲e̲t̲ variable was added to make it possible to call the
         𝗴𝗲𝘁𝗼𝗽𝘁() function multiple times.

     ·   If the o̲p̲t̲i̲n̲d̲ variable is set to 0, 𝗴𝗲𝘁𝗼𝗽𝘁() will behave as if
         the o̲p̲t̲r̲e̲s̲e̲t̲ variable has been set.  This is for compatibility
         with GNU 𝗴𝗲𝘁𝗼𝗽𝘁().  New code should use o̲p̲t̲r̲e̲s̲e̲t̲ instead.

     ·   If the first character of o̲p̲t̲s̲t̲r̲i̲n̲g̲ is a plus sign (‘+’), it
         will be ignored.  This is for compatibility with GNU 𝗴𝗲𝘁𝗼𝗽𝘁().

     ·   If the first character of o̲p̲t̲s̲t̲r̲i̲n̲g̲ is a dash (‘-’), non-
         options will be returned as arguments to the option character
         ‘\1’.  This is for compatibility with GNU 𝗴𝗲𝘁𝗼𝗽𝘁().

     ·   A single dash (‘-’) may be specified as a character in
         o̲p̲t̲s̲t̲r̲i̲n̲g̲, however it should n̲e̲v̲e̲r̲ have an argument associated
         with it.  This allows 𝗴𝗲𝘁𝗼𝗽𝘁() to be used with programs that
         expect ‘-’ as an option flag.  This practice is wrong, and
         should not be used in any current development.  It is provided
         for backward compatibility o̲n̲l̲y̲.  Care should be taken not to
         use ‘-’ as the first character in o̲p̲t̲s̲t̲r̲i̲n̲g̲ to avoid a semantic
         conflict with GNU 𝗴𝗲𝘁𝗼𝗽𝘁() semantics (see above).  By default,
         a single dash causes 𝗴𝗲𝘁𝗼𝗽𝘁() to return -1.

     Historic BSD versions of 𝗴𝗲𝘁𝗼𝗽𝘁() set o̲p̲t̲o̲p̲t̲ to the last option
     character processed.  However, this conflicts with IEEE Std 1003.1
     (“POSIX.1”) which stipulates that o̲p̲t̲o̲p̲t̲ be set to the last charac‐
     ter that caused an error.

𝐇𝐈𝐒𝐓𝐎𝐑𝐘
     The 𝗴𝗲𝘁𝗼𝗽𝘁() function appeared in 4.3BSD.

𝐁𝐔𝐆𝐒
     The 𝗴𝗲𝘁𝗼𝗽𝘁() function was once specified to return EOF instead of
     -1.  This was changed by IEEE Std 1003.2-1992 (“POSIX.2”) to decou‐
     ple 𝗴𝗲𝘁𝗼𝗽𝘁() from <s̲t̲d̲i̲o̲.̲h̲>.

     It is possible to handle digits as option letters.  This allows
     𝗴𝗲𝘁𝗼𝗽𝘁() to be used with programs that expect a number (“-3”) as an
     option.  This practice is wrong, and should not be used in any cur‐
     rent development.  It is provided for backward compatibility o̲n̲l̲y̲.
     The following code fragment works in most cases and can handle
     mixed number and letter arguments.

           int aflag = 0, bflag = 0, ch, lastch = '\0';
           int length = -1, newarg = 1, prevoptind = 1;

           while ((ch = getopt(argc, argv, "0123456789ab")) != -1) {
                   switch (ch) {
                   case '0': case '1': case '2': case '3': case '4':
                   case '5': case '6': case '7': case '8': case '9':
                           if (newarg || !isdigit(lastch))
                                   length = 0;
                           else if (length > INT_MAX / 10)
                                   usage();
                           length = (length * 10) + (ch - '0');
                           break;
                   case 'a':
                           aflag = 1;
                           break;
                   case 'b':
                           bflag = 1;
                           break;
                   default:
                           usage();
                   }
                   lastch = ch;
                   newarg = optind != prevoptind;
                   prevoptind = optind;
           }

COSMOPOLITAN                    January 4, 2016                            BSD

────────────────────────────────────────────────────────────────────────────

GETOPT_LONG(3)      Cosmopolitan Library Functions Manual       GETOPT_LONG(3)

𝐍𝐀𝐌𝐄
     𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴, 𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴_𝗼𝗻𝗹𝘆 — get long options from command line
     argument list

𝐒𝐘𝐍𝐎𝐏𝐒𝐈𝐒
     #𝗶𝗻𝗰𝗹𝘂𝗱𝗲 <𝗴𝗲𝘁𝗼𝗽𝘁.𝗵>

     e̲x̲t̲e̲r̲n̲ c̲h̲a̲r̲ *̲o̲p̲t̲a̲r̲g̲;̲
     e̲x̲t̲e̲r̲n̲ i̲n̲t̲ o̲p̲t̲i̲n̲d̲;̲
     e̲x̲t̲e̲r̲n̲ i̲n̲t̲ o̲p̲t̲o̲p̲t̲;̲
     e̲x̲t̲e̲r̲n̲ i̲n̲t̲ o̲p̲t̲e̲r̲r̲;̲
     e̲x̲t̲e̲r̲n̲ i̲n̲t̲ o̲p̲t̲r̲e̲s̲e̲t̲;̲

     i̲n̲t̲
     𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴(i̲n̲t̲ a̲r̲g̲c̲, c̲h̲a̲r̲ *̲ c̲o̲n̲s̲t̲ *̲a̲r̲g̲v̲, c̲o̲n̲s̲t̲ c̲h̲a̲r̲ *̲o̲p̲t̲s̲t̲r̲i̲n̲g̲,
         c̲o̲n̲s̲t̲ s̲t̲r̲u̲c̲t̲ o̲p̲t̲i̲o̲n̲ *̲l̲o̲n̲g̲o̲p̲t̲s̲, i̲n̲t̲ *̲l̲o̲n̲g̲i̲n̲d̲e̲x̲);

     i̲n̲t̲
     𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴_𝗼𝗻𝗹𝘆(i̲n̲t̲ a̲r̲g̲c̲, c̲h̲a̲r̲ *̲ c̲o̲n̲s̲t̲ *̲a̲r̲g̲v̲,
         c̲o̲n̲s̲t̲ c̲h̲a̲r̲ *̲o̲p̲t̲s̲t̲r̲i̲n̲g̲, c̲o̲n̲s̲t̲ s̲t̲r̲u̲c̲t̲ o̲p̲t̲i̲o̲n̲ *̲l̲o̲n̲g̲o̲p̲t̲s̲,
         i̲n̲t̲ *̲l̲o̲n̲g̲i̲n̲d̲e̲x̲);

𝐃𝐄𝐒𝐂𝐑𝐈𝐏𝐓𝐈𝐎𝐍
     The 𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴() function is similar to getopt(3) but it accepts
     options in two forms: words and characters.  The 𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴()
     function provides a superset of the functionality of getopt(3).
     𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴() can be used in two ways.  In the first way, every
     long option understood by the program has a corresponding short
     option, and the option structure is only used to translate from
     long options to short options.  When used in this fashion,
     𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴() behaves identically to getopt(3).  This is a good way
     to add long option processing to an existing program with the mini‐
     mum of rewriting.

     In the second mechanism, a long option sets a flag in the o̲p̲t̲i̲o̲n̲
     structure passed, or will store a pointer to the command line argu‐
     ment in the o̲p̲t̲i̲o̲n̲ structure passed to it for options that take
     arguments.  Additionally, the long option's argument may be speci‐
     fied as a single argument with an equal sign, e.g.

           $ myprogram --myoption=somevalue

     When a long option is processed, the call to 𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴() will
     return 0.  For this reason, long option processing without short‐
     cuts is not backwards compatible with getopt(3).

     It is possible to combine these methods, providing for long options
     processing with short option equivalents for some options.  Less
     frequently used options would be processed as long options only.

     Abbreviated long option names are accepted when 𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴() pro‐
     cesses long options if the abbreviation is unique.  An exact match
     is always preferred for a defined long option.

     The 𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴() call requires an array to be initialized describ‐
     ing the long options.  Each element of the array is a structure:

           struct option {
                   char *name;
                   int has_arg;
                   int *flag;
                   int val;
           };

     The n̲a̲m̲e̲ field should contain the option name without the leading
     double dash.

     The h̲a̲s̲_a̲r̲g̲ field should be one of:

           no_argument        no argument to the option is expected.
           required_argument  an argument to the option is required.
           optional_argument  an argument to the option may be pre‐
                              sented.

     If f̲l̲a̲g̲ is not NULL, then the integer pointed to by it will be set
     to the value in the v̲a̲l̲ field.  If the f̲l̲a̲g̲ field is NULL, then the
     v̲a̲l̲ field will be returned.  Setting f̲l̲a̲g̲ to NULL and setting v̲a̲l̲
     to the corresponding short option will make this function act just
     like getopt(3).

     If the l̲o̲n̲g̲i̲n̲d̲e̲x̲ field is not NULL, then the integer pointed to by
     it will be set to the index of the long option relative to
     l̲o̲n̲g̲o̲p̲t̲s̲.

     The last element of the l̲o̲n̲g̲o̲p̲t̲s̲ array has to be filled with
     zeroes.

     The 𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴_𝗼𝗻𝗹𝘆() function behaves identically to
     𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴() with the exception that long options may start with
     ‘-’ in addition to ‘--’.  If an option starting with ‘-’ does not
     match a long option but does match a single-character option, the
     single-character option is returned.

𝐑𝐄𝐓𝐔𝐑𝐍 𝐕𝐀𝐋𝐔𝐄𝐒
     If the f̲l̲a̲g̲ field in struct option is NULL, 𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴() and
     𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴_𝗼𝗻𝗹𝘆() return the value specified in the v̲a̲l̲ field,
     which is usually just the corresponding short option.  If f̲l̲a̲g̲ is
     not NULL, these functions return 0 and store v̲a̲l̲ in the location
     pointed to by f̲l̲a̲g̲.  These functions return ‘:’ if there was a
     missing option argument, ‘?’ if the user specified an unknown or
     ambiguous option, and -1 when the argument list has been exhausted.

𝐈𝐌𝐏𝐋𝐄𝐌𝐄𝐍𝐓𝐀𝐓𝐈𝐎𝐍 𝐃𝐈𝐅𝐅𝐄𝐑𝐄𝐍𝐂𝐄𝐒
     This section describes differences to the GNU implementation found
     in glibc-2.1.3:

     ·   handling of ‘-’ within the option string (not the first charac‐
         ter):

         GNU      treats a ‘-’ on the command line as a non-argument.

         OpenBSD  a ‘-’ within the option string matches a ‘-’ (single
                  dash) on the command line.  This functionality is pro‐
                  vided for backward compatibility with programs, such
                  as su(1), that use ‘-’ as an option flag.  This prac‐
                  tice is wrong, and should not be used in any current
                  development.

     ·   handling of ‘::’ in the option string in the presence of
         POSIXLY_CORRECT:

         Both     GNU and OpenBSD ignore POSIXLY_CORRECT here and take
                  ‘::’ to mean the preceding option takes an optional
                  argument.

     ·   return value in case of missing argument if first character
         (after ‘+’ or ‘-’) in the option string is not ‘:’:

         GNU      returns ‘?’

         OpenBSD  returns ‘:’ (since OpenBSD's getopt(3) does).

     ·   handling of ‘--a’ in getopt(3):

         GNU      parses this as option ‘-’, option ‘a’.

         OpenBSD  parses this as ‘--’, and returns -1 (ignoring the ‘a’)
                  (because the original 𝗴𝗲𝘁𝗼𝗽𝘁() did.)

     ·   setting of o̲p̲t̲o̲p̲t̲ for long options with f̲l̲a̲g̲ non-NULL:

         GNU      sets o̲p̲t̲o̲p̲t̲ to v̲a̲l̲.

         OpenBSD  sets o̲p̲t̲o̲p̲t̲ to 0 (since v̲a̲l̲ would never be returned).

     ·   handling of ‘-W’ with ‘W;’ in the option string in getopt(3)
         (not 𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴()):

         GNU      causes a segmentation fault.

         OpenBSD  no special handling is done; ‘W;’ is interpreted as
                  two separate options, neither of which take an argu‐
                  ment.

     ·   setting of o̲p̲t̲a̲r̲g̲ for long options without an argument that are
         invoked via ‘-W’ (with ‘W;’ in the option string):

         GNU      sets o̲p̲t̲a̲r̲g̲ to the option name (the argument of ‘-W’).

         OpenBSD  sets o̲p̲t̲a̲r̲g̲ to NULL (the argument of the long option).

     ·   handling of ‘-W’ with an argument that is not (a prefix to) a
         known long option (with ‘W;’ in the option string):

         GNU      returns ‘-W’ with o̲p̲t̲a̲r̲g̲ set to the unknown option.

         OpenBSD  treats this as an error (unknown option) and returns
                  ‘?’ with o̲p̲t̲o̲p̲t̲ set to 0 and o̲p̲t̲a̲r̲g̲ set to NULL (as
                  GNU's man page documents).

     ·   The error messages are different.

     ·   OpenBSD does not permute the argument vector at the same points
         in the calling sequence as GNU does.  The aspects normally used
         by the caller (ordering after -1 is returned, value of o̲p̲t̲i̲n̲d̲
         relative to current positions) are the same, though.  (We do
         fewer variable swaps.)

𝐄𝐍𝐕𝐈𝐑𝐎𝐍𝐌𝐄𝐍𝐓
     POSIXLY_CORRECT  If set, option processing stops when the first
                      non-option is found and a leading ‘+’ in the
                      o̲p̲t̲s̲t̲r̲i̲n̲g̲ is ignored.

𝐄𝐗𝐀𝐌𝐏𝐋𝐄𝐒
     int bflag, ch, fd;
     int daggerset;

     /* options descriptor */
     static struct option longopts[] = {
             { "buffy",      no_argument,            NULL,           'b' },
             { "fluoride",   required_argument,      NULL,           'f' },
             { "daggerset",  no_argument,            &daggerset,     1 },
             { NULL,         0,                      NULL,           0 }
     };

     bflag = 0;
     while ((ch = getopt_long(argc, argv, "bf:", longopts, NULL)) != -1)
             switch (ch) {
             case 'b':
                     bflag = 1;
                     break;
             case 'f':
                     if ((fd = open(optarg, O_RDONLY, 0)) == -1)
                             err(1, "unable to open %s", optarg);
                     break;
             case 0:
                     if (daggerset)
                             fprintf(stderr, "Buffy will use her dagger to "
                                 "apply fluoride to dracula's teeth\n");
                     break;
             default:
                     usage();
             }
     argc -= optind;
     argv += optind;

𝐒𝐄𝐄 𝐀𝐋𝐒𝐎
     getopt(3)

𝐇𝐈𝐒𝐓𝐎𝐑𝐘
     The 𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴() and 𝗴𝗲𝘁𝗼𝗽𝘁_𝗹𝗼𝗻𝗴_𝗼𝗻𝗹𝘆() functions first appeared
     in GNU libiberty.  This implementation first appeared in
     OpenBSD 3.3.

𝐁𝐔𝐆𝐒
     The a̲r̲g̲v̲ argument is not really const as its elements may be per‐
     muted (unless POSIXLY_CORRECT is set).

COSMOPOLITAN                    January 4, 2016                            BSD
