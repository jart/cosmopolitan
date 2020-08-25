CTAGS(1)                        Exuberant Ctags                       CTAGS(1)



𝐍𝐀𝐌𝐄
       ctags - Generate tag files for source code



𝐒𝐘𝐍𝐎𝐏𝐒𝐈𝐒
       𝗰𝘁𝗮𝗴𝘀 [𝗼𝗽𝘁𝗶𝗼𝗻𝘀] [f̲i̲l̲e̲(̲s̲)̲]

       𝗲𝘁𝗮𝗴𝘀 [𝗼𝗽𝘁𝗶𝗼𝗻𝘀] [f̲i̲l̲e̲(̲s̲)̲]



𝐃𝐄𝐒𝐂𝐑𝐈𝐏𝐓𝐈𝐎𝐍
       The  𝗰𝘁𝗮𝗴𝘀  and 𝗲𝘁𝗮𝗴𝘀 programs (hereinafter collectively referred
       to as 𝗰𝘁𝗮𝗴𝘀, except where distinguished) generate  an  index  (or
       "tag")  file  for a variety of language objects found in f̲i̲l̲e̲(̲s̲)̲.
       This tag file allows these items to be quickly and easily located
       by  a  text editor or other utility. A "tag" signifies a language
       object for which an index entry is available (or,  alternatively,
       the index entry created for that object).

       Alternatively,  𝗰𝘁𝗮𝗴𝘀  can  generate a cross reference file which
       lists, in human readable  form,  information  about  the  various
       source objects found in a set of language files.

       Tag  index  files  are supported by numerous editors, which allow
       the user to locate the object associated with a name appearing in
       a  source  file  and  jump to the file and line which defines the
       name. Those known about at the time of this release are:

           𝐕𝗶(1) and its derivatives (e.g.  Elvis,  Vim,  Vile,  Lemmy),
           𝐂𝐑𝗶𝐒𝐏,  𝐄𝗺𝗮𝗰𝘀,  𝐅𝐓𝐄 (Folding Text Editor), 𝐉𝐄𝐃, 𝗷𝐄𝗱𝗶𝘁, 𝐌𝗶𝗻𝗲𝗱,
           𝐍𝐄𝗱𝗶𝘁 (Nirvana Edit), 𝐓𝐒𝐄 (The  SemWare  Editor),  𝐔𝗹𝘁𝗿𝗮𝐄𝗱𝗶𝘁,
           𝐖𝗼𝗿𝗸𝐒𝗽𝗮𝗰𝗲, 𝐗𝟮, 𝐙𝗲𝘂𝘀

       𝐂𝘁𝗮𝗴𝘀  is  capable of generating different kinds of tags for each
       of many different languages. For a  complete  list  of  supported
       languages,  the names by which they are recognized, and the kinds
       of tags which are generated for each,  see  the  --𝗹𝗶𝘀𝘁-𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲𝘀
       and --𝗹𝗶𝘀𝘁-𝗸𝗶𝗻𝗱𝘀 options.



𝐒𝐎𝐔𝐑𝐂𝐄 𝐅𝐈𝐋𝐄𝐒
       Unless  the --𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲-𝗳𝗼𝗿𝗰𝗲 option is specified, the language of
       each source file is automatically selected based upon  a  mapping
       of  file names to languages. The mappings in effect for each lan‐
       guage may be display using the  --𝗹𝗶𝘀𝘁-𝗺𝗮𝗽𝘀  option  and  may  be
       changed  using  the --𝗹𝗮𝗻𝗴𝗺𝗮𝗽 option.  On platforms which support
       it, if the name of a file is not mapped to  a  language  and  the
       file  is executable, the first line of the file is checked to see
       if the file is a "#!" script for a recognized language.

       By default, all other files names are ignored. This permits  run‐
       ning 𝗰𝘁𝗮𝗴𝘀 on all files in either a single directory (e.g. "ctags
       *"), or on all files in an entire  source  directory  tree  (e.g.
       "ctags  -R"),  since  only  those files whose names are mapped to
       languages will be scanned.

       [The reason that .h extensions are mapped  to  C++  files  rather
       than C files is because it is common to use .h extensions in C++,
       and no harm results in treating them as C++ files.]



𝐎𝐏𝐓𝐈𝐎𝐍𝐒
       Despite the wealth of available options, defaults are set so that
       𝗰𝘁𝗮𝗴𝘀  is most commonly executed without any options (e.g. "ctags
       *", or "ctags -R"), which will create a tag file in  the  current
       directory  for all recognized source files. The options described
       below are provided merely to allow custom tailoring to meet  spe‐
       cial needs.

       Note  that spaces separating the single-letter options from their
       parameters are optional.

       Note also that the boolean parameters to the  long  form  options
       (those beginning with "--" and that take a "[̲=̲y̲e̲s̲|n̲o̲]̲" parameter)
       may be omitted, in which case "=y̲e̲s̲" is implied. (e.g. --𝘀𝗼𝗿𝘁  is
       equivalent  to  --𝘀𝗼𝗿𝘁=y̲e̲s̲). Note further that "=1̲" and "=o̲n̲" are
       considered synonyms for "=y̲e̲s̲", and that "=0̲" and "=o̲f̲f̲" are con‐
       sidered synonyms for "=n̲o̲".

       Some  options  are  either ignored or useful only when used while
       running in etags mode (see  -𝗲  option).  Such  options  will  be
       noted.

       Most  options  may appear anywhere on the command line, affecting
       only those files which follow the option. A few options, however,
       must appear before the first file name and will be noted as such.

       Options  taking  language names will accept those names in either
       upper or lower case. See the --𝗹𝗶𝘀𝘁-𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲𝘀 option for  a  com‐
       plete list of the built-in language names.


       -𝗮   Equivalent to --𝗮𝗽𝗽𝗲𝗻𝗱.


       -𝐁   Use  backward  searching patterns (e.g. ?pattern?). [Ignored
            in etags mode]


       -𝗲   Enable etags mode, which will create a tag file for use with
            the  Emacs  editor.  Alternatively, if 𝗰𝘁𝗮𝗴𝘀 is invoked by a
            name containing the string "etags" (either by  renaming,  or
            creating  a link to, the executable), etags mode will be en‐
            abled. This option must appear before the first file name.


       -𝗳 t̲a̲g̲f̲i̲l̲e̲
            Use the name specified by t̲a̲g̲f̲i̲l̲e̲ for the tag file  (default
            is "tags", or "TAGS" when running in etags mode). If t̲a̲g̲f̲i̲l̲e̲
            is specified as "-", then the tag file is written  to  stan‐
            dard  output  instead.  𝐂𝘁𝗮𝗴𝘀 will stubbornly refuse to take
            orders if t̲a̲g̲f̲i̲l̲e̲ exists and its first line  contains  some‐
            thing other than a valid tags line. This will save your neck
            if you mistakenly type "ctags -f *.c", which would otherwise
            overwrite  your  first C file with the tags generated by the
            rest! It will also refuse to accept a  multi-character  file
            name  which  begins  with a '-' (dash) character, since this
            most likely means that you left out the tag  file  name  and
            this  option tried to grab the next option as the file name.
            If you really want to name your  output  tag  file  "-ugly",
            specify  it as "./-ugly". This option must appear before the
            first file name. If this option is specified more than once,
            only the last will apply.


       -𝐅   Use  forward  searching patterns (e.g. /pattern/) (default).
            [Ignored in etags mode]


       -𝗵 l̲i̲s̲t̲
            Specifies a list of file extensions, separated  by  periods,
            which are to be interpreted as include (or header) files. To
            indicate files having no extension, use a  period  not  fol‐
            lowed  by  a  non-period character (e.g. ".", "..x", ".x.").
            This option only affects how the  scoping  of  a  particular
            kinds  of  tags is interpreted (i.e. whether or not they are
            considered as globally visible or visible  only  within  the
            file  in which they are defined); it does not map the exten‐
            sion to any particular language. Any tag which is located in
            a  non-include file and cannot be seen (e.g. linked to) from
            another  file  is  considered  to  have  file-limited  (e.g.
            static)  scope.  No kind of tag appearing in an include file
            will be considered to have file-limited scope. If the  first
            character in the list is a plus sign, then the extensions in
            the list will be appended to the  current  list;  otherwise,
            the  list  will  replace  the  current  list. See, also, the
            --𝗳𝗶𝗹𝗲-𝘀𝗰𝗼𝗽𝗲     option.     The     default     list     is
            ".h.H.hh.hpp.hxx.h++.inc.def".  To restore the default list,
            specify -𝗵 d̲e̲f̲a̲u̲l̲t̲. Note that if an  extension  supplied  to
            this  option  is not already mapped to a particular language
            (see 𝐒𝐎𝐔𝐑𝐂𝐄 𝐅𝐈𝐋𝐄𝐒, above), you will also need to use  either
            the --𝗹𝗮𝗻𝗴𝗺𝗮𝗽 or --𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲-𝗳𝗼𝗿𝗰𝗲 option.


       -𝐈 i̲d̲e̲n̲t̲i̲f̲i̲e̲r̲-̲l̲i̲s̲t̲
            Specifies  a  list  of identifiers which are to be specially
            handled while parsing C and C++ source files. This option is
            specifically   provided  to  handle  special  cases  arising
            through the use of preprocessor macros. When the identifiers
            listed are simple identifiers, these identifiers will be ig‐
            nored during parsing of the source files. If  an  identifier
            is suffixed with a '+' character, 𝗰𝘁𝗮𝗴𝘀 will also ignore any
            parenthesis-enclosed argument  list  which  may  immediately
            follow  the  identifier  in the source files. If two identi‐
            fiers are separated with the '=' character, the first  iden‐
            tifiers  is  replaced  by the second identifiers for parsing
            purposes. The list of identifiers may be  supplied  directly
            on  the command line or read in from a separate file. If the
            first character of i̲d̲e̲n̲t̲i̲f̲i̲e̲r̲-̲l̲i̲s̲t̲ is '@', '.' or a pathname
            separator  ('/' or '\'), or the first two characters specify
            a drive letter (e.g. "C:"),  the  parameter  i̲d̲e̲n̲t̲i̲f̲i̲e̲r̲-̲l̲i̲s̲t̲
            will  be interpreted as a filename from which to read a list
            of identifiers,  one  per  input  line.  Otherwise,  i̲d̲e̲n̲t̲i̲‐̲
            f̲i̲e̲r̲-̲l̲i̲s̲t̲  is a list of identifiers (or identifier pairs) to
            be specially handled, each delimited by a either a comma  or
            by  white  space (in which case the list should be quoted to
            keep the entire list as one command line argument). Multiple
            -𝐈  options  may  be  supplied.  To clear the list of ignore
            identifiers, supply a single dash ("-") for i̲d̲e̲n̲t̲i̲f̲i̲e̲r̲-̲l̲i̲s̲t̲.

            This feature is useful when preprocessor macros are used  in
            such  a way that they cause syntactic confusion due to their
            presence. Indeed, this is the best way of working  around  a
            number  of problems caused by the presence of syntax-busting
            macros in source files (see 𝐂𝐀𝐕𝐄𝐀𝐓𝐒, below).  Some  examples
            will illustrate this point.

               int foo ARGDECL4(void *, ptr, long int, nbytes)


            In  the above example, the macro "ARGDECL4" would be mistak‐
            enly interpreted to be the name of the function  instead  of
            the correct name of "foo". Specifying -𝐈 A̲R̲G̲D̲E̲C̲L̲4̲ results in
            the correct behavior.

               /* creates an RCS version string in module */
               MODULE_VERSION("$Revision: 750 $")


            In the above example the macro  invocation  looks  too  much
            like  a  function definition because it is not followed by a
            semicolon (indeed, it could even be  followed  by  a  global
            variable  definition  that  would look much like a K&R style
            function parameter declaration). In fact, this seeming func‐
            tion  definition  could  possibly even cause the rest of the
            file to be skipped over while trying to complete the defini‐
            tion. Specifying -𝐈 M̲O̲D̲U̲L̲E̲_V̲E̲R̲S̲I̲O̲N̲+̲ would avoid such a prob‐
            lem.

               CLASS Example {
                   // your content here
               };


            The example above uses "CLASS" as a preprocessor macro which
            expands  to  something  different for each platform. For in‐
            stance CLASS may be defined as "class __declspec(dllexport)"
            on  Win32  platforms  and simply "class" on UNIX.  Normally,
            the absence of the  C++  keyword  "class"  would  cause  the
            source  file  to be incorrectly parsed. Correct behavior can
            be restored by specifying -𝐈 C̲L̲A̲S̲S̲=̲c̲l̲a̲s̲s̲.


       -𝐋 f̲i̲l̲e̲
            Read from f̲i̲l̲e̲ a list of file names for which tags should be
            generated.  If f̲i̲l̲e̲ is specified as "-", then file names are
            read from standard input. File names read using this  option
            are  processed following file names appearing on the command
            line. Options are also accepted in this input. If  this  op‐
            tion  is specified more than once, only the last will apply.
            𝐍𝗼𝘁𝗲: f̲i̲l̲e̲ is read in line-oriented mode, where a  new  line
            is  the  only delimiter and non-trailing white space is con‐
            sidered significant, in order  that  file  names  containing
            spaces  may  be  supplied  (however, trailing white space is
            stripped from lines); this can affect how options are parsed
            if included in the input.


       -𝗻   Equivalent to --𝗲𝘅𝗰𝗺𝗱=n̲u̲m̲b̲e̲r̲.


       -𝐍   Equivalent to --𝗲𝘅𝗰𝗺𝗱=p̲a̲t̲t̲e̲r̲n̲.


       -𝗼 t̲a̲g̲f̲i̲l̲e̲
            Equivalent to -𝗳 t̲a̲g̲f̲i̲l̲e̲.


       -𝐑   Equivalent to --𝗿𝗲𝗰𝘂𝗿𝘀𝗲.


       -𝘂   Equivalent to --𝘀𝗼𝗿𝘁=n̲o̲ (i.e. "unsorted").


       -𝐕   Equivalent to --𝘃𝗲𝗿𝗯𝗼𝘀𝗲.


       -𝘄   This  option  is silently ignored for backward-compatibility
            with the ctags of SVR4 Unix.


       -𝘅   Print a tabular, human-readable cross reference (xref)  file
            to standard output instead of generating a tag file. The in‐
            formation contained in the output includes:  the  tag  name;
            the kind of tag; the line number, file name, and source line
            (with extra white space condensed) of the file which defines
            the  tag.  No  tag file is written and all options affecting
            tag file output will be ignored.  Example  applications  for
            this  feature  are generating a listing of all functions lo‐
            cated in a source file (e.g. 𝗰𝘁𝗮𝗴𝘀 -𝘅 --𝗰-𝗸𝗶𝗻𝗱𝘀=f̲ f̲i̲l̲e̲),  or
            generating a list of all externally visible global variables
            located  in  a  source  file  (e.g.  𝗰𝘁𝗮𝗴𝘀  -𝘅   --𝗰-𝗸𝗶𝗻𝗱𝘀=v̲
            --𝗳𝗶𝗹𝗲-𝘀𝗰𝗼𝗽𝗲=n̲o̲  f̲i̲l̲e̲).  This  option must appear before the
            first file name.


       --𝗮𝗽𝗽𝗲𝗻𝗱[=y̲e̲s̲|n̲o̲]
            Indicates whether tags generated from  the  specified  files
            should  be appended to those already present in the tag file
            or should replace them. This option is off by default.  This
            option must appear before the first file name.


       --𝗲𝘁𝗮𝗴𝘀-𝗶𝗻𝗰𝗹𝘂𝗱𝗲=f̲i̲l̲e̲
            Include a reference to f̲i̲l̲e̲ in the tag file. This option may
            be specified as many times as desired. This supports  Emacs'
            capability  to  use  a  tag  file which "includes" other tag
            files. [Available only in etags mode]


       --𝗲𝘅𝗰𝗹𝘂𝗱𝗲=[p̲a̲t̲t̲e̲r̲n̲]
            Add p̲a̲t̲t̲e̲r̲n̲ to a list of  excluded  files  and  directories.
            This  option  may be specified as many times as desired. For
            each file name considered by 𝗰𝘁𝗮𝗴𝘀, each  p̲a̲t̲t̲e̲r̲n̲  specified
            using this option will be compared against both the complete
            path (e.g.  some/path/base.ext)  and  the  base  name  (e.g.
            base.ext)  of the file, thus allowing patterns which match a
            given file name irrespective of its path, or  match  only  a
            specific  path. If appropriate support is available from the
            runtime library of your C compiler, then p̲a̲t̲t̲e̲r̲n̲ may contain
            the  usual  shell wildcards (not regular expressions) common
            on Unix (be sure to quote the option  parameter  to  protect
            the  wildcards from being expanded by the shell before being
            passed to 𝗰𝘁𝗮𝗴𝘀; also be aware that wildcards can match  the
            slash  character, '/'). You can determine if shell wildcards
            are available on your platform by examining  the  output  of
            the --𝘃𝗲𝗿𝘀𝗶𝗼𝗻 option, which will include "+wildcards" in the
            compiled feature list; otherwise, p̲a̲t̲t̲e̲r̲n̲ is matched against
            file names using a simple textual comparison.

            If  p̲a̲t̲t̲e̲r̲n̲  begins with the character '@', then the rest of
            the string is interpreted as a file name from which to  read
            exclusion  patterns,  one per line. If p̲a̲t̲t̲e̲r̲n̲ is empty, the
            list of excluded patterns is cleared.  Note that at  program
            startup, the default exclude list contains "EIFGEN", "SCCS",
            "RCS", and "CVS", which are names of directories  for  which
            it  is  generally  not desirable to descend while processing
            the --𝗿𝗲𝗰𝘂𝗿𝘀𝗲 option.


       --𝗲𝘅𝗰𝗺𝗱=t̲y̲p̲e̲
            Determines the type of EX command used to locate tags in the
            source file.  [Ignored in etags mode]

            The  valid  values  for  t̲y̲p̲e̲ (either the entire word or the
            first letter is accepted) are:


            n̲u̲m̲b̲e̲r̲   Use only line numbers in the tag file for  locating
                     tags. This has four advantages:
                     1.  Significantly reduces the size of the resulting
                         tag file.
                     2.  Eliminates failures to find  tags  because  the
                         line  defining the tag has changed, causing the
                         pattern match to fail (note that some  editors,
                         such  as  𝘃𝗶𝗺, are able to recover in many such
                         instances).
                     3.  Eliminates finding identical matching, but  in‐
                         correct, source lines (see 𝐁𝐔𝐆𝐒, below).
                     4.  Retains  separate  entries  in the tag file for
                         lines which are identical in content.  In  p̲a̲t̲‐̲
                         t̲e̲r̲n̲  mode,  duplicate  entries are dropped be‐
                         cause the search  patterns  they  generate  are
                         identical,  making  the  duplicate entries use‐
                         less.


                     However, this option has one significant  drawback:
                     changes to the source files can cause the line num‐
                     bers recorded in the tag file to no  longer  corre‐
                     spond  to  the  lines  in  the source file, causing
                     jumps to some tags to miss the target definition by
                     one  or  more lines. Basically, this option is best
                     used when the source code to which it is applied is
                     not  subject  to change. Selecting this option type
                     causes the following options to be ignored: -𝐁𝐅.


            p̲a̲t̲t̲e̲r̲n̲  Use only search patterns for all tags, rather  than
                     the  line  numbers  usually  used for macro defini‐
                     tions. This has the advantage  of  not  referencing
                     obsolete line numbers when lines have been added or
                     removed since the tag file was generated.


            m̲i̲x̲e̲d̲    In this mode, patterns are generally  used  with  a
                     few  exceptions.  For  C, line numbers are used for
                     macro definition tags. This was the default  format
                     generated  by the original 𝗰𝘁𝗮𝗴𝘀 and is, therefore,
                     retained as the default for this option.  For  For‐
                     tran,  line  numbers are used for common blocks be‐
                     cause their corresponding source lines  are  gener‐
                     ally identical, making pattern searches useless for
                     finding all matches.


       --𝗲𝘅𝘁𝗿𝗮=[̲+̲|̲-̲]̲f̲l̲a̲g̲s̲
            Specifies whether to include extra tag entries  for  certain
            kinds  of  information. The parameter f̲l̲a̲g̲s̲ is a set of one-
            letter flags, each representing one kind of extra tag  entry
            to  include  in the tag file. If f̲l̲a̲g̲s̲ is preceded by either
            the '+' or '-' character, the effect of each flag  is  added
            to,  or removed from, those currently enabled; otherwise the
            flags replace any current settings. The meaning of each flag
            is as follows:


               f̲   Include  an  entry  for  the  base file name of every
                   source file (e.g.  "example.c"), which addresses  the
                   first line of the file.

               q̲   Include  an  extra class-qualified tag entry for each
                   tag which is a member of a class (for  languages  for
                   which  this  information is extracted; currently C++,
                   Eiffel, and Java). The actual form of  the  qualified
                   tag  depends upon the language from which the tag was
                   derived (using a form that is most  natural  for  how
                   qualified  calls  are specified in the language). For
                   C++, it is in the form  "class::member";  for  Eiffel
                   and  Java, it is in the form "class.member". This may
                   allow easier location of a specific tags when  multi‐
                   ple  occurrences of a tag name occur in the tag file.
                   Note, however, that this could potentially more  than
                   double the size of the tag file.


       --𝗳𝗶𝗲𝗹𝗱𝘀=[̲+̲|̲-̲]̲f̲l̲a̲g̲s̲
            Specifies the available extension fields which are to be in‐
            cluded in the entries of the tag file (see 𝐓𝐀𝐆 𝐅𝐈𝐋𝐄  𝐅𝐎𝐑𝐌𝐀𝐓,
            below,  for  more information). The parameter f̲l̲a̲g̲s̲ is a set
            of one-letter flags, each representing one type of extension
            field  to  include, with the following meanings (disabled by
            default unless indicated):


               a̲   Access (or export) of class members
               f̲   File-restricted scoping [enabled]
               i̲   Inheritance information
               k̲   Kind of tag as a single letter [enabled]
               K̲   Kind of tag as full name
               l̲   Language of source file containing tag
               m̲   Implementation information
               n̲   Line number of tag definition
               s̲   Scope of tag definition [enabled]
               S̲   Signature of routine  (e.g.  prototype  or  parameter
                   list)
               z̲   Include the "kind:" key in kind field
               t̲   Type  and name of a variable or typedef as "typeref:"
                   field [enabled]

            Each letter or group of letters may be  preceded  by  either
            '+'  to  add it to the default set, or '-' to exclude it. In
            the absence of any preceding '+' or  '-'  sign,  only  those
            kinds  explicitly  listed  in  f̲l̲a̲g̲s̲ will be included in the
            output (i.e. overriding the default set). This option is ig‐
            nored  if  the option --𝗳𝗼𝗿𝗺𝗮𝘁=1̲ has been specified. The de‐
            fault value of this option is f̲k̲s̲t̲.


       --𝗳𝗶𝗹𝗲-𝘀𝗰𝗼𝗽𝗲[=y̲e̲s̲|n̲o̲]
            Indicates whether tags scoped only for a single  file  (i.e.
            tags  which cannot be seen outside of the file in which they
            are defined, such as "static" tags) should  be  included  in
            the output. See, also, the -𝗵 option. This option is enabled
            by default.


       --𝗳𝗶𝗹𝘁𝗲𝗿[=y̲e̲s̲|n̲o̲]
            Causes 𝗰𝘁𝗮𝗴𝘀 to behave as  a  filter,  reading  source  file
            names  from  standard input and printing their tags to stan‐
            dard output on a file-by-file basis. If --𝘀𝗼𝗿𝘁𝗲𝗱 is enabled,
            tags  are  sorted  only within the source file in which they
            are defined. File names are  read  from  standard  input  in
            line-oriented  input  mode (see note for -𝐋 option) and only
            after file names listed on the command line or from any file
            supplied  using  the -𝐋 option. When this option is enabled,
            the options -𝗳, -𝗼, and --𝘁𝗼𝘁𝗮𝗹𝘀 are ignored. This option is
            quite  esoteric and is disabled by default. This option must
            appear before the first file name.


       --𝗳𝗶𝗹𝘁𝗲𝗿-𝘁𝗲𝗿𝗺𝗶𝗻𝗮𝘁𝗼𝗿=s̲t̲r̲i̲n̲g̲
            Specifies a string to print to standard output following the
            tags  for  each file name parsed when the --𝗳𝗶𝗹𝘁𝗲𝗿 option is
            enabled. This may permit an application reading  the  output
            of  ctags to determine when the output for each file is fin‐
            ished. Note that if the file name read is  a  directory  and
            --𝗿𝗲𝗰𝘂𝗿𝘀𝗲  is enabled, this string will be printed only once
            at the end of all tags found for by  descending  the  direc‐
            tory. This string will always be separated from the last tag
            line for the file by its terminating newline.   This  option
            is  quite esoteric and is empty by default. This option must
            appear before the first file name.


       --𝗳𝗼𝗿𝗺𝗮𝘁=l̲e̲v̲e̲l̲
            Change the format of the output tag file. Currently the only
            valid  values  for  l̲e̲v̲e̲l̲  are 1̲ or 2̲. Level 1 specifies the
            original tag file format and level 2  specifies  a  new  ex‐
            tended  format  containing extension fields (but in a manner
            which retains backward-compatibility with original 𝘃𝗶(1) im‐
            plementations). The default level is 2. This option must ap‐
            pear before the first file name. [Ignored in etags mode]


       --𝗵𝗲𝗹𝗽
            Prints to standard output a detailed usage description,  and
            then exits.


       --𝗶𝗳𝟬[=y̲e̲s̲|n̲o̲]
            Indicates  a preference as to whether code within an "#if 0"
            branch of a preprocessor conditional should be examined  for
            non-macro tags (macro tags are always included). Because the
            intent of this construct is to  disable  code,  the  default
            value of this option is n̲o̲. Note that this indicates a pref‐
            erence only and does not guarantee skipping code  within  an
            "#if 0" branch, since the fall-back algorithm used to gener‐
            ate tags when preprocessor conditionals are too complex fol‐
            lows  all branches of a conditional. This option is disabled
            by default.


       --<𝐋𝐀𝐍𝐆>-𝗸𝗶𝗻𝗱𝘀=[̲+̲|̲-̲]̲k̲i̲n̲d̲s̲
            Specifies a list of  language-specific  kinds  of  tags  (or
            kinds)  to  include in the output file for a particular lan‐
            guage, where <𝐋𝐀𝐍𝐆> is case-insensitive and is  one  of  the
            built-in language names (see the --𝗹𝗶𝘀𝘁-𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲𝘀 option for
            a complete list). The parameter k̲i̲n̲d̲s̲ is a group of one-let‐
            ter  flags designating kinds of tags (particular to the lan‐
            guage) to either include or exclude  from  the  output.  The
            specific  sets  of flags recognized for each language, their
            meanings and defaults may be list using the --𝗹𝗶𝘀𝘁-𝗸𝗶𝗻𝗱𝘀 op‐
            tion. Each letter or group of letters may be preceded by ei‐
            ther '+' to add it to, or '-' to remove it from, the default
            set.  In  the absence of any preceding '+' or '-' sign, only
            those kinds explicitly listed in k̲i̲n̲d̲s̲ will be  included  in
            the  output  (i.e.  overriding the default for the specified
            language).

            As an example for the C language, in order to add prototypes
            and external variable declarations to the default set of tag
            kinds, but exclude macros, use --𝗰-𝗸𝗶𝗻𝗱𝘀=+̲p̲x̲-̲d̲;  to  include
            only tags for functions, use --𝗰-𝗸𝗶𝗻𝗱𝘀=f̲.


       --𝗹𝗮𝗻𝗴𝗱𝗲𝗳=n̲a̲m̲e̲
            Defines a new user-defined language, n̲a̲m̲e̲, to be parsed with
            regular expressions. Once defined, n̲a̲m̲e̲ may be used in other
            options  taking  language names. The typical use of this op‐
            tion is to first define the language, then map file names to
            it  using  -̲-̲l̲a̲n̲g̲m̲a̲p̲, then specify regular expressions using
            -̲-̲r̲e̲g̲e̲x̲-̲<̲L̲A̲N̲G̲>̲ to define how its tags are found.


       --𝗹𝗮𝗻𝗴𝗺𝗮𝗽=m̲a̲p̲[̲,̲m̲a̲p̲[̲.̲.̲.̲]̲]̲
            Controls how file names are mapped  to  languages  (see  the
            --𝗹𝗶𝘀𝘁-𝗺𝗮𝗽𝘀  option).  Each  comma-separated m̲a̲p̲ consists of
            the language name (either a built-in  or  user-defined  lan‐
            guage),  a  colon, and a list of file extensions and/or file
            name patterns. A file extension is  specified  by  preceding
            the extension with a period (e.g. ".c"). A file name pattern
            is specified by enclosing the pattern in  parentheses  (e.g.
            "([Mm]akefile)").  If  appropriate support is available from
            the runtime library of your C compiler, then the  file  name
            pattern may contain the usual shell wildcards common on Unix
            (be sure to quote the option parameter to protect the  wild‐
            cards  from  being expanded by the shell before being passed
            to 𝗰𝘁𝗮𝗴𝘀). You can determine if shell wildcards  are  avail‐
            able  on your platform by examining the output of the --𝘃𝗲𝗿‐
            𝘀𝗶𝗼𝗻 option, which will include "+wildcards" in the compiled
            feature  list; otherwise, the file name patterns are matched
            against file names using a simple textual  comparison.  When
            mapping a file extension, it will first be unmapped from any
            other languages.

            If the first character in a map is a plus sign, then the ex‐
            tensions and file name patterns in that map will be appended
            to the current map for that  language;  otherwise,  the  map
            will  replace  the current map. For example, to specify that
            only files with extensions of .c and .x are to be treated as
            C  language files, use "--𝗹𝗮𝗻𝗴𝗺𝗮𝗽=c̲:̲.̲c̲.̲x̲"; to also add files
            with extensions  of  .j  as  Java  language  files,  specify
            "--𝗹𝗮𝗻𝗴𝗺𝗮𝗽=c̲:̲.̲c̲.̲x̲,̲j̲a̲v̲a̲:̲+̲.̲j̲".  To  map  makefiles (e.g. files
            named either "Makefile", "makefile", or having the extension
            ".mak")    to    a    language    called   "make",   specify
            "--𝗹𝗮𝗻𝗴𝗺𝗮𝗽=m̲a̲k̲e̲:̲(̲[̲M̲m̲]̲a̲k̲e̲f̲i̲l̲e̲)̲.̲m̲a̲k̲".  To map files having  no
            extension,  specify  a  period  not followed by a non-period
            character (e.g. ".", "..x", ".x."). To clear the mapping for
            a  particular language (thus inhibiting automatic generation
            of tags for that language), specify an empty extension  list
            (e.g.   "--𝗹𝗮𝗻𝗴𝗺𝗮𝗽=f̲o̲r̲t̲r̲a̲n̲:̲").  To  restore the default lan‐
            guage mappings for all a  particular  language,  supply  the
            keyword  "default"  for the mapping.  To specify restore the
            default  language  mappings  for  all   languages,   specify
            "--𝗹𝗮𝗻𝗴𝗺𝗮𝗽=d̲e̲f̲a̲u̲l̲t̲".  Note  that  file extensions are tested
            before file name patterns when inferring the language  of  a
            file.


       --𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲-𝗳𝗼𝗿𝗰𝗲=l̲a̲n̲g̲u̲a̲g̲e̲
            By  default,  𝗰𝘁𝗮𝗴𝘀  automatically selects the language of a
            source file, ignoring those files whose language  cannot  be
            determined (see 𝐒𝐎𝐔𝐑𝐂𝐄 𝐅𝐈𝐋𝐄𝐒, above). This option forces the
            specified l̲a̲n̲g̲u̲a̲g̲e̲  (case-insensitive;  either  built-in  or
            user-defined)  to be used for every supplied file instead of
            automatically selecting the language based upon  its  exten‐
            sion. In addition, the special value a̲u̲t̲o̲ indicates that the
            language should be automatically selected (which effectively
            disables this option).


       --𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲𝘀=[̲+̲|̲-̲]̲l̲i̲s̲t̲
            Specifies the languages for which tag generation is enabled,
            with l̲i̲s̲t̲ containing  a  comma-separated  list  of  language
            names  (case-insensitive;  either built-in or user-defined).
            If the first language of l̲i̲s̲t̲ is not preceded  by  either  a
            '+'  or  '-', the current list will be cleared before adding
            or removing the languages in l̲i̲s̲t̲. Until a  '-'  is  encoun‐
            tered,  each  language in the list will be added to the cur‐
            rent list. As either the '+' or '-' is  encountered  in  the
            list,  the  languages following it are added or removed from
            the current list, respectively. Thus, it becomes  simple  to
            replace the current list with a new one, or to add or remove
            languages from the current list. The actual  list  of  files
            for  which  tags will be generated depends upon the language
            extension mapping in effect (see the --𝗹𝗮𝗻𝗴𝗺𝗮𝗽 option). Note
            that all languages, including user-defined languages are en‐
            abled unless explicitly disabled using this option. Language
            names  included  in l̲i̲s̲t̲ may be any built-in language or one
            previously defined with --𝗹𝗮𝗻𝗴𝗱𝗲𝗳.  The  default  is  "all",
            which  is  also  accepted  as  a  valid  argument.  See  the
            --𝗹𝗶𝘀𝘁-𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲𝘀 option for a complete list of the  built-in
            language names.


       --𝗹𝗶𝗰𝗲𝗻𝘀𝗲
            Prints a summary of the software license to standard output,
            and then exits.


       --𝗹𝗶𝗻𝗲-𝗱𝗶𝗿𝗲𝗰𝘁𝗶𝘃𝗲𝘀[=y̲e̲s̲|n̲o̲]
            Specifies whether "#line" directives should  be  recognized.
            These are present in the output of preprocessors and contain
            the line number, and possibly the file name, of the original
            source  file(s)  from which the preprocessor output file was
            generated. When enabled, this option  will  cause  𝗰𝘁𝗮𝗴𝘀  to
            generate  tag  entries  marked  with the file names and line
            numbers of their locations original source file(s),  instead
            of  their  actual  locations in the preprocessor output. The
            actual file names placed into the tag  file  will  have  the
            same  leading  path  components  as  the preprocessor output
            file, since it is assumed that the original source files are
            located relative to the preprocessor output file (unless, of
            course, the #line directive  specifies  an  absolute  path).
            This  option  is off by default. 𝐍𝗼𝘁𝗲: This option is gener‐
            ally only useful when used together with the  --𝗲𝘅𝗰𝗺𝗱=n̲u̲m̲b̲e̲r̲
            (-𝗻)  option. Also, you may have to use either the --𝗹𝗮𝗻𝗴𝗺𝗮𝗽
            or --𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲-𝗳𝗼𝗿𝗰𝗲 option if  the  extension  of  the  pre‐
            processor output file is not known to 𝗰𝘁𝗮𝗴𝘀.


       --𝗹𝗶𝗻𝗸𝘀[=y̲e̲s̲|n̲o̲]
            Indicates  whether  symbolic  links (if supported) should be
            followed. When disabled, symbolic links  are  ignored.  This
            option is on by default.


       --𝗹𝗶𝘀𝘁-𝗸𝗶𝗻𝗱𝘀[=l̲a̲n̲g̲u̲a̲g̲e̲|a̲l̲l̲]
            Lists the tag kinds recognized for either the specified lan‐
            guage or all languages, and then exits.  Each  kind  of  tag
            recorded  in  the  tag  file  is represented by a one-letter
            flag, which is also used to filter the tags placed into  the
            output  through  use of the --<𝐋𝐀𝐍𝐆>-𝗸𝗶𝗻𝗱𝘀 option. Note that
            some languages and/or tag kinds  may  be  implemented  using
            regular  expressions  and may not be available if regex sup‐
            port is not compiled into 𝗰𝘁𝗮𝗴𝘀 (see the --𝗿𝗲𝗴𝗲𝘅-<𝐋𝐀𝐍𝐆>  op‐
            tion).  Each  kind  listed  is  enabled  unless  followed by
            "[off]".


       --𝗹𝗶𝘀𝘁-𝗺𝗮𝗽𝘀[=l̲a̲n̲g̲u̲a̲g̲e̲|a̲l̲l̲]
            Lists the file extensions and file name patterns which asso‐
            ciate  a  file name with a language for either the specified
            language or all languages, and then exits. See the --𝗹𝗮𝗻𝗴𝗺𝗮𝗽
            option, and 𝐒𝐎𝐔𝐑𝐂𝐄 𝐅𝐈𝐋𝐄𝐒, above.


       --𝗹𝗶𝘀𝘁-𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲𝘀
            Lists  the  names  of the languages understood by 𝗰𝘁𝗮𝗴𝘀, and
            then exits.  These language names are case  insensitive  and
            may   be   used   in   the   --𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲-𝗳𝗼𝗿𝗰𝗲,  --𝗹𝗮𝗻𝗴𝘂𝗮𝗴𝗲𝘀,
            --<𝐋𝐀𝐍𝐆>-𝗸𝗶𝗻𝗱𝘀, and --𝗿𝗲𝗴𝗲𝘅-<𝐋𝐀𝐍𝐆> options.


       --𝗼𝗽𝘁𝗶𝗼𝗻𝘀=f̲i̲l̲e̲
            Read additional options from f̲i̲l̲e̲. The file  should  contain
            one option per line. As a special case, if --𝗼𝗽𝘁𝗶𝗼𝗻𝘀=N̲O̲N̲E̲ is
            specified as the first option on the command line,  it  will
            disable  the  automatic reading of any configuration options
            from either a file or the environment (see 𝐅𝐈𝐋𝐄𝐒).


       --𝗿𝗲𝗰𝘂𝗿𝘀𝗲[=y̲e̲s̲|n̲o̲]
            Recurse into directories encountered in the list of supplied
            files.  If  the  list of supplied files is empty and no file
            list is specified with the -𝐋 option, then the  current  di‐
            rectory  (i.e. ".") is assumed. Symbolic links are followed.
            If you don't like these behaviors, either explicitly specify
            the  files  or pipe the output of 𝗳𝗶𝗻𝗱(1) into 𝗰𝘁𝗮𝗴𝘀 -𝐋- in‐
            stead. 𝐍𝗼𝘁𝗲: This option is not supported on  all  platforms
            at present.  It is available if the output of the --𝗵𝗲𝗹𝗽 op‐
            tion includes this option.   See,  also,  the  --𝗲𝘅𝗰𝗹𝘂𝗱𝗲  to
            limit recursion.


       --𝗿𝗲𝗴𝗲𝘅-<𝐋𝐀𝐍𝐆>=/̲r̲e̲g̲e̲x̲p̲/̲r̲e̲p̲l̲a̲c̲e̲m̲e̲n̲t̲/̲[̲k̲i̲n̲d̲-̲s̲p̲e̲c̲/̲]̲[̲f̲l̲a̲g̲s̲]̲
            The  /̲r̲e̲g̲e̲x̲p̲/̲r̲e̲p̲l̲a̲c̲e̲m̲e̲n̲t̲/̲  pair  define a regular expression
            replacement pattern, similar in style  to  𝘀𝗲𝗱  substitution
            commands,  with  which  to  generate  tags from source files
            mapped to the named language, <𝐋𝐀𝐍𝐆>, (case-insensitive; ei‐
            ther  a  built-in or user-defined language). The regular ex‐
            pression, r̲e̲g̲e̲x̲p̲, defines  an  extended  regular  expression
            (roughly  that  used by 𝗲𝗴𝗿𝗲𝗽(1)), which is used to locate a
            single source line containing a  tag  and  may  specify  tab
            characters  using  \t.  When a matching line is found, a tag
            will be generated for the name defined by r̲e̲p̲l̲a̲c̲e̲m̲e̲n̲t̲, which
            generally   will  contain  the  special  back-references  \1
            through \9 to refer to matching sub-expression groups within
            r̲e̲g̲e̲x̲p̲.  The '/' separator characters shown in the parameter
            to the option can actually be  replaced  by  any  character.
            Note that whichever separator character is used will have to
            be escaped with a backslash ('\') character wherever  it  is
            used  in  the parameter as something other than a separator.
            The regular expression defined by this option  is  added  to
            the  current  list  of regular expressions for the specified
            language unless the parameter is omitted, in which case  the
            current list is cleared.

            Unless  modified  by f̲l̲a̲g̲s̲, r̲e̲g̲e̲x̲p̲ is interpreted as a Posix
            extended regular expression. The r̲e̲p̲l̲a̲c̲e̲m̲e̲n̲t̲  should  expand
            for  all matching lines to a non-empty string of characters,
            or a warning message will  be  reported.  An  optional  kind
            specifier  for  tags matching r̲e̲g̲e̲x̲p̲ may follow r̲e̲p̲l̲a̲c̲e̲m̲e̲n̲t̲,
            which will determine what kind of tag  is  reported  in  the
            "kind"  extension  field  (see  𝐓𝐀𝐆 𝐅𝐈𝐋𝐄 𝐅𝐎𝐑𝐌𝐀𝐓, below). The
            full form of k̲i̲n̲d̲-̲s̲p̲e̲c̲ is in the form of a single letter,  a
            comma, a name (without spaces), a comma, a description, fol‐
            lowed by a separator, which specify the short and long forms
            of the kind value and its textual description (displayed us‐
            ing --𝗹𝗶𝘀𝘁-𝗸𝗶𝗻𝗱𝘀). Either the kind name and/or the  descrip‐
            tion may be omitted. If k̲i̲n̲d̲-̲s̲p̲e̲c̲ is omitted, it defaults to
            "r̲,̲r̲e̲g̲e̲x̲". Finally, f̲l̲a̲g̲s̲  are  one  or  more  single-letter
            characters  having the following effect upon the interpreta‐
            tion of r̲e̲g̲e̲x̲p̲:


               b̲   The pattern is interpreted as a Posix  basic  regular
                   expression.

               e̲   The  pattern is interpreted as a Posix extended regu‐
                   lar expression (default).

               i̲   The regular expression is to be applied in a case-in‐
                   sensitive manner.

            Note  that  this  option is available only if 𝗰𝘁𝗮𝗴𝘀 was com‐
            piled with support for regular  expressions,  which  depends
            upon your platform. You can determine if support for regular
            expressions is compiled in by examining the  output  of  the
            --𝘃𝗲𝗿𝘀𝗶𝗼𝗻  option,  which  will include "+regex" in the com‐
            piled feature list.

            For more information on  the  regular  expressions  used  by
            𝗰𝘁𝗮𝗴𝘀,  see  either the 𝗿𝗲𝗴𝗲𝘅(𝟱,𝟳) man page, or the GNU info
            documentation for regex (e.g. "info regex").


       --𝘀𝗼𝗿𝘁[=y̲e̲s̲|n̲o̲|f̲o̲l̲d̲c̲a̲s̲e̲]
            Indicates whether the tag file should be sorted on  the  tag
            name (default is y̲e̲s̲). Note that the original 𝘃𝗶(1) required
            sorted tags.  The f̲o̲l̲d̲c̲a̲s̲e̲ value specifies case  insensitive
            (or case-folded) sorting.  Fast binary searches of tag files
            sorted with case-folding will require special  support  from
            tools using tag files, such as that found in the ctags read‐
            tags library, or Vim version 6.2 or higher (using  "set  ig‐
            norecase").  This  option  must appear before the first file
            name. [Ignored in etags mode]


       --𝘁𝗮𝗴-𝗿𝗲𝗹𝗮𝘁𝗶𝘃𝗲[=y̲e̲s̲|n̲o̲]
            Indicates that the file  paths  recorded  in  the  tag  file
            should be relative to the directory containing the tag file,
            rather than relative to the current  directory,  unless  the
            files  supplied on the command line are specified with abso‐
            lute paths. This option must appear before  the  first  file
            name. The default is y̲e̲s̲ when running in etags mode (see the
            -𝗲 option), n̲o̲ otherwise.


       --𝘁𝗼𝘁𝗮𝗹𝘀[=y̲e̲s̲|n̲o̲]
            Prints statistics about the source files read  and  the  tag
            file  written  during  the current invocation of 𝗰𝘁𝗮𝗴𝘀. This
            option is off by default.  This option  must  appear  before
            the first file name.


       --𝘃𝗲𝗿𝗯𝗼𝘀𝗲[=y̲e̲s̲|n̲o̲]
            Enable  verbose  mode. This prints out information on option
            processing and a brief message describing what action is be‐
            ing taken for each file considered by 𝗰𝘁𝗮𝗴𝘀. Normally, 𝗰𝘁𝗮𝗴𝘀
            does not read command line arguments until after options are
            read from the configuration files (see 𝐅𝐈𝐋𝐄𝐒, below) and the
            𝐂𝐓𝐀𝐆𝐒 environment variable. However, if this option  is  the
            first  argument on the command line, it will take effect be‐
            fore any options are read from these sources. The default is
            n̲o̲.


       --𝘃𝗲𝗿𝘀𝗶𝗼𝗻
            Prints  a  version  identifier for 𝗰𝘁𝗮𝗴𝘀 to standard output,
            and then exits. This is guaranteed  to  always  contain  the
            string "Exuberant Ctags".



𝐎𝐏𝐄𝐑𝐀𝐓𝐈𝐎𝐍𝐀𝐋 𝐃𝐄𝐓𝐀𝐈𝐋𝐒
       As  𝗰𝘁𝗮𝗴𝘀 considers each file name in turn, it tries to determine
       the language of the file by applying the following three tests in
       order:  if  the  file extension has been mapped to a language, if
       the file name matches a shell pattern mapped to a  language,  and
       finally if the file is executable and its first line specifies an
       interpreter using the Unix-style "#!" specification (if supported
       on  the  platform).  If  a  language  was identified, the file is
       opened and then the appropriate language parser is called to  op‐
       erate  on  the currently open file. The parser parses through the
       file and adds an entry to the tag file for each  language  object
       it  is written to handle. See 𝐓𝐀𝐆 𝐅𝐈𝐋𝐄 𝐅𝐎𝐑𝐌𝐀𝐓, below, for details
       on these entries.

       This implementation of 𝗰𝘁𝗮𝗴𝘀 imposes no  formatting  requirements
       on  C code as do legacy implementations. Older implementations of
       ctags tended to rely upon certain formatting assumptions in order
       to  help it resolve coding dilemmas caused by preprocessor condi‐
       tionals.

       In general, 𝗰𝘁𝗮𝗴𝘀 tries to be smart about conditional  preproces‐
       sor  directives.  If  a  preprocessor  conditional is encountered
       within a statement which defines a tag, 𝗰𝘁𝗮𝗴𝘀  follows  only  the
       first  branch  of that conditional (except in the special case of
       "#if 0", in which case it follows only the last branch). The rea‐
       son for this is that failing to pursue only one branch can result
       in ambiguous syntax, as in the following example:

              #ifdef TWO_ALTERNATIVES
              struct {
              #else
              union {
              #endif
                  short a;
                  long b;
              }

       Both branches cannot be followed, or braces become unbalanced and
       𝗰𝘁𝗮𝗴𝘀 would be unable to make sense of the syntax.

       If  the  application  of this heuristic fails to properly parse a
       file, generally  due  to  complicated  and  inconsistent  pairing
       within  the  conditionals, 𝗰𝘁𝗮𝗴𝘀 will retry the file using a dif‐
       ferent heuristic which does not  selectively  follow  conditional
       preprocessor  branches,  but instead falls back to relying upon a
       closing brace ("}") in column 1 as indicating the end of a  block
       once any brace imbalance results from following a #if conditional
       branch.

       𝐂𝘁𝗮𝗴𝘀 will also try to specially handle arguments lists  enclosed
       in  double  sets  of parentheses in order to accept the following
       conditional construct:

              extern void foo __ARGS((int one, char two));

       Any name immediately preceding the "((" will be automatically ig‐
       nored and the previous name will be used.

       C++ operator definitions are specially handled. In order for con‐
       sistency with all types of operators (overloaded and conversion),
       the  operator name in the tag file will always be preceded by the
       string "operator " (i.e. even if the actual  operator  definition
       was written as "operator<<").

       After  creating or appending to the tag file, it is sorted by the
       tag name, removing identical tag lines.



𝐓𝐀𝐆 𝐅𝐈𝐋𝐄 𝐅𝐎𝐑𝐌𝐀𝐓
       When not running in etags mode, each entry in the tag  file  con‐
       sists of a separate line, each looking like this in the most gen‐
       eral case:

        tag_name<TAB>file_name<TAB>ex_cmd;"<TAB>extension_fields

       The fields and separators of these lines are  specified  as  fol‐
       lows:

           1.  tag name
           2.  single tab character
           3.  name  of the file in which the object associated with the
               tag is located
           4.  single tab character
           5.  EX command used to locate the tag within the file; gener‐
               ally  a search pattern (either /pattern/ or ?pattern?) or
               line number (see --𝗲𝘅𝗰𝗺𝗱). Tag file format 2 (see  --𝗳𝗼𝗿‐
               𝗺𝗮𝘁)  extends this EX command under certain circumstances
               to include a set of extension  fields  (described  below)
               embedded  in an EX comment immediately appended to the EX
               command, which leaves it backward-compatible with  origi‐
               nal 𝘃𝗶(1) implementations.

       A  few  special  tags  are written into the tag file for internal
       purposes. These tags are composed in such a way that they  always
       sort to the top of the file.  Therefore, the first two characters
       of these tags are used a magic number to detect a  tag  file  for
       purposes  of  determining whether a valid tag file is being over‐
       written rather than a source file.

       Note that the name of each source file will be  recorded  in  the
       tag file exactly as it appears on the command line. Therefore, if
       the path you specified on the command line was  relative  to  the
       current  directory,  then it will be recorded in that same manner
       in the tag file. See, however, the --𝘁𝗮𝗴-𝗿𝗲𝗹𝗮𝘁𝗶𝘃𝗲 option for  how
       this behavior can be modified.

       Extension  fields  are  tab-separated key-value pairs appended to
       the end of the EX command as a comment, as described above. These
       key  value  pairs  appear  in the general form "k̲e̲y̲:v̲a̲l̲u̲e̲". Their
       presence in the lines of the  tag  file  are  controlled  by  the
       --𝗳𝗶𝗲𝗹𝗱𝘀  option. The possible keys and the meaning of their val‐
       ues are as follows:


       a̲c̲c̲e̲s̲s̲      Indicates the visibility of this class member,  where
                   v̲a̲l̲u̲e̲ is specific to the language.


       f̲i̲l̲e̲        Indicates  that  the tag has file-limited visibility.
                   This key has no corresponding value.


       k̲i̲n̲d̲        Indicates the type, or kind, of tag. Its value is ei‐
                   ther  one  of  the corresponding one-letter flags de‐
                   scribed  under  the  various  --<𝐋𝐀𝐍𝐆>-𝗸𝗶𝗻𝗱𝘀  options
                   above,  or  a  full name. It is permitted (and is, in
                   fact, the default) for the key portion of this  field
                   to  be omitted. The optional behaviors are controlled
                   with the --𝗳𝗶𝗲𝗹𝗱𝘀 option.


       i̲m̲p̲l̲e̲m̲e̲n̲t̲a̲t̲i̲o̲n̲
                   When present, this indicates a limited implementation
                   (abstract  vs. concrete) of a routine or class, where
                   v̲a̲l̲u̲e̲ is specific to the language ("virtual" or "pure
                   virtual" for C++; "abstract" for Java).


       i̲n̲h̲e̲r̲i̲t̲s̲    When  present,  v̲a̲l̲u̲e̲.  is  a comma-separated list of
                   classes from which this class is derived (i.e. inher‐
                   its from).


       s̲i̲g̲n̲a̲t̲u̲r̲e̲   When present, v̲a̲l̲u̲e̲ is a language-dependent represen‐
                   tation of the signature of a routine. A routine  sig‐
                   nature in its complete form specifies the return type
                   of a routine and its formal argument list.  This  ex‐
                   tension field is presently supported only for C-based
                   languages and does not include the return type.


       In addition, information on the scope of the tag  definition  may
       be  available, with the key portion equal to some language-depen‐
       dent construct name and its value the name declared for that con‐
       struct  in  the  program. This scope entry indicates the scope in
       which the tag was found. For example, a tag  generated  for  a  C
       structure  member  would  have  a scope looking like "struct:myS‐
       truct".



𝐇𝐎𝐖 𝐓𝐎 𝐔𝐒𝐄 𝐖𝐈𝐓𝐇 𝐕𝐈
       Vi will, by default, expect a tag file by the name "tags" in  the
       current directory. Once the tag file is built, the following com‐
       mands exercise the tag indexing feature:

       𝘃𝗶 -𝘁 𝘁𝗮𝗴   Start vi and position the cursor at the file and line
                   where "tag" is defined.

       :𝘁𝗮 𝘁𝗮𝗴     Find a tag.

       𝐂𝘁𝗿𝗹-]      Find the tag under the cursor.

       𝐂𝘁𝗿𝗹-𝐓      Return  to  previous location before jump to tag (not
                   widely implemented).



𝐇𝐎𝐖 𝐓𝐎 𝐔𝐒𝐄 𝐖𝐈𝐓𝐇 𝐆𝐍𝐔 𝐄𝐌𝐀𝐂𝐒
       Emacs will, by default, expect a tag file by the name  "TAGS"  in
       the  current directory. Once the tag file is built, the following
       commands exercise the tag indexing feature:

       𝐌-𝘅 𝘃𝗶𝘀𝗶𝘁-𝘁𝗮𝗴𝘀-𝘁𝗮𝗯𝗹𝗲 <𝐑𝐄𝐓> 𝐅𝐈𝐋𝐄 <𝐑𝐄𝐓>
                 Select the tag file, "FILE", to use.

       𝐌-. [𝐓𝐀𝐆] <𝐑𝐄𝐓>
                 Find the first definition of TAG. The  default  tag  is
                 the identifier under the cursor.

       𝐌-*       Pop back to where you previously invoked "M-.".

       𝐂-𝘂 𝐌-.   Find the next definition for the last tag.


       For more commands, see the T̲a̲g̲s̲ topic in the Emacs info document.



𝐇𝐎𝐖 𝐓𝐎 𝐔𝐒𝐄 𝐖𝐈𝐓𝐇 𝐍𝐄𝐃𝐈𝐓
       NEdit  version 5.1 and later can handle the new extended tag file
       format (see --𝗳𝗼𝗿𝗺𝗮𝘁). To make NEdit use  the  tag  file,  select
       "File->Load  Tags  File".  To  jump  to the definition for a tag,
       highlight the word, then press Ctrl-D. NEdit  5.1  can  can  read
       multiple tag files from different directories.  Setting the X re‐
       source nedit.tagFile to the name of a tag file instructs NEdit to
       automatically load that tag file at startup time.



𝐂𝐀𝐕𝐄𝐀𝐓𝐒
       Because  𝗰𝘁𝗮𝗴𝘀  is  neither a preprocessor nor a compiler, use of
       preprocessor macros can fool 𝗰𝘁𝗮𝗴𝘀 into either  missing  tags  or
       improperly generating inappropriate tags. Although 𝗰𝘁𝗮𝗴𝘀 has been
       designed to handle certain common cases, this is the single  big‐
       gest  cause  of reported problems. In particular, the use of pre‐
       processor constructs which alter the textual syntax of C can fool
       𝗰𝘁𝗮𝗴𝘀. You can work around many such problems by using the -𝐈 op‐
       tion.

       Note that since 𝗰𝘁𝗮𝗴𝘀 generates patterns for locating  tags  (see
       the  --𝗲𝘅𝗰𝗺𝗱 option), it is entirely possible that the wrong line
       may be found by your editor if there exists another  source  line
       which  is identical to the line containing the tag. The following
       example demonstrates this condition:

              int variable;

              /* ... */
              void foo(variable)
              int variable;
              {
                  /* ... */
              }

       Depending upon which editor you use and where  in  the  code  you
       happen  to  be, it is possible that the search pattern may locate
       the local parameter declaration in foo() before it finds the  ac‐
       tual  global  variable definition, since the lines (and therefore
       their search patterns are identical). This can be avoided by  use
       of the --𝗲𝘅𝗰𝗺𝗱=n̲ option.



𝐁𝐔𝐆𝐒
       𝐂𝘁𝗮𝗴𝘀 has more options than 𝗹𝘀(1).

       When  parsing  a  C++  member  function  definition (e.g. "class‐
       Name::function"), 𝗰𝘁𝗮𝗴𝘀 cannot determine whether the scope speci‐
       fier is a class name or a namespace specifier and always lists it
       as a class name in the scope portion  of  the  extension  fields.
       Also,  if a C++ function is defined outside of the class declara‐
       tion (the usual case), the  access  specification  (i.e.  public,
       protected,  or private) and implementation information (e.g. vir‐
       tual, pure virtual) contained in the function declaration are not
       known  when  the tag is generated for the function definition. It
       will, however be available for prototypes (e.g --𝗰++-𝗸𝗶𝗻𝗱𝘀=+̲p̲).

       No qualified tags are generated for  language  objects  inherited
       into a class.



𝐄𝐍𝐕𝐈𝐑𝐎𝐍𝐌𝐄𝐍𝐓 𝐕𝐀𝐑𝐈𝐀𝐁𝐋𝐄𝐒
       𝐂𝐓𝐀𝐆𝐒   If  this environment variable exists, it will be expected
               to contain a set of default options which are  read  when
               𝗰𝘁𝗮𝗴𝘀  starts,  after  the  configuration files listed in
               𝐅𝐈𝐋𝐄𝐒, below, are read, but before any command  line  op‐
               tions  are  read.  Options  appearing on the command line
               will override options specified in  this  variable.  Only
               options  will  be  read from this variable. Note that all
               white space in this variable is considered  a  separator,
               making it impossible to pass an option parameter contain‐
               ing an embedded space. If this is a problem, use  a  con‐
               figuration file instead.


       𝐄𝐓𝐀𝐆𝐒   Similar  to  the  𝐂𝐓𝐀𝐆𝐒 variable above, this variable, if
               found, will be read when 𝗲𝘁𝗮𝗴𝘀 starts. If  this  variable
               is not found, 𝗲𝘁𝗮𝗴𝘀 will try to use 𝐂𝐓𝐀𝐆𝐒 instead.


       𝐓𝐌𝐏𝐃𝐈𝐑  On  Unix-like  hosts  where  mkstemp()  is available, the
               value of this variable specifies the directory  in  which
               to  place temporary files. This can be useful if the size
               of a temporary file becomes too large to fit on the  par‐
               tition holding the default temporary directory defined at
               compilation time.  𝗰𝘁𝗮𝗴𝘀 creates temporary files only  if
               either  (1)  an  emacs-style tag file is being generated,
               (2) the tag file is being sent to standard output, or (3)
               the  program  was  compiled to use an internal sort algo‐
               rithm to sort the tag files instead of the the sort util‐
               ity  of  the operating system. If the sort utility of the
               operating system is being used, it will generally observe
               this  variable  also.  Note  that if 𝗰𝘁𝗮𝗴𝘀 is setuid, the
               value of TMPDIR will be ignored.



𝐅𝐈𝐋𝐄𝐒
       /̲c̲t̲a̲g̲s̲.̲c̲n̲f̲ (̲o̲n̲ M̲S̲D̲O̲S̲,̲ M̲S̲W̲i̲n̲d̲o̲w̲s̲ o̲n̲l̲y̲)̲
       /̲e̲t̲c̲/̲c̲t̲a̲g̲s̲.̲c̲o̲n̲f̲
       /̲u̲s̲r̲/̲l̲o̲c̲a̲l̲/̲e̲t̲c̲/̲c̲t̲a̲g̲s̲.̲c̲o̲n̲f̲
       $̲H̲O̲M̲E̲/̲.̲c̲t̲a̲g̲s̲
       $̲H̲O̲M̲E̲/̲c̲t̲a̲g̲s̲.̲c̲n̲f̲ (̲o̲n̲ M̲S̲D̲O̲S̲,̲ M̲S̲W̲i̲n̲d̲o̲w̲s̲ o̲n̲l̲y̲)̲
       .̲c̲t̲a̲g̲s̲
       c̲t̲a̲g̲s̲.̲c̲n̲f̲ (̲o̲n̲ M̲S̲D̲O̲S̲,̲ M̲S̲W̲i̲n̲d̲o̲w̲s̲ o̲n̲l̲y̲)̲
              If any of these configuration files exist,  each  will  be
              expected  to  contain  a  set of default options which are
              read in the order listed when 𝗰𝘁𝗮𝗴𝘀 starts, but before the
              𝐂𝐓𝐀𝐆𝐒 environment variable is read or any command line op‐
              tions are read. This makes it possible  to  set  up  site-
              wide,  personal  or project-level defaults. It is possible
              to compile 𝗰𝘁𝗮𝗴𝘀 to read an additional configuration  file
              before  any  of those shown above, which will be indicated
              if the output produced by the --𝘃𝗲𝗿𝘀𝗶𝗼𝗻 option  lists  the
              "custom-conf"  feature. Options appearing in the 𝐂𝐓𝐀𝐆𝐒 en‐
              vironment variable or on the command  line  will  override
              options  specified  in  these  files. Only options will be
              read from these files. Note that the option files are read
              in  line-oriented  mode  in  which  spaces are significant
              (since shell quoting is not possible). Each  line  of  the
              file  is read as one command line parameter (as if it were
              quoted with single quotes). Therefore, use  new  lines  to
              indicate separate command-line arguments.


       t̲a̲g̲s̲   The default tag file created by 𝗰𝘁𝗮𝗴𝘀.

       T̲A̲G̲S̲   The default tag file created by 𝗲𝘁𝗮𝗴𝘀.


𝐒𝐄𝐄 𝐀𝐋𝐒𝐎
       The official Exuberant Ctags web site at:

              http://ctags.sourceforge.net

       Also  𝗲𝘅(1), 𝘃𝗶(1), 𝗲𝗹𝘃𝗶𝘀, or, better yet, 𝘃𝗶𝗺, the official edi‐
       tor of 𝗰𝘁𝗮𝗴𝘀. For more information on 𝘃𝗶𝗺, see the VIM Pages  web
       site at:

              http://www.vim.org/



𝐀𝐔𝐓𝐇𝐎𝐑
       Darren Hiebert <dhiebert at users.sourceforge.net>
       http://DarrenHiebert.com/



𝐌𝐎𝐓𝐈𝐕𝐀𝐓𝐈𝐎𝐍
       "Think  ye at all times of rendering some service to every member
       of the human race."

       "All effort and exertion put forth by man from  the  fullness  of
       his  heart  is  worship, if it is prompted by the highest motives
       and the will to do service to humanity."

              -- From the Baha'i Writings



𝐂𝐑𝐄𝐃𝐈𝐓𝐒
       This version of 𝗰𝘁𝗮𝗴𝘀 was originally derived from and inspired by
       the  ctags program by Steve Kirkendall <kirkenda@cs.pdx.edu> that
       comes with the Elvis vi clone (though virtually none of the orig‐
       inal code remains).

       Credit  is  also due Bram Moolenaar <Bram@vim.org>, the author of
       𝘃𝗶𝗺, who has devoted so much of his time and energy both  to  de‐
       veloping  the  editor  as a service to others, and to helping the
       orphans of Uganda.

       The section entitled "HOW TO USE WITH GNU EMACS" was  shamelessly
       stolen from the info page for GNU 𝗲𝘁𝗮𝗴𝘀.



Darren Hiebert              Version 5.9~svn20110310                   CTAGS(1)
