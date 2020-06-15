M4(1)              Cosmopolitan General Commands Manual             -*-text-*-

𝐍𝐀𝐌𝐄
     𝗺𝟰 — macro language processor

𝐒𝐘𝐍𝐎𝐏𝐒𝐈𝐒
     𝗺𝟰 [-𝐄𝗴𝐏𝘀] [-𝐃n̲a̲m̲e̲[=v̲a̲l̲u̲e̲]] [-𝗱 f̲l̲a̲g̲s̲] [-𝐈 d̲i̲r̲n̲a̲m̲e̲] [-𝗼 f̲i̲l̲e̲n̲a̲m̲e̲]
        [-𝘁 m̲a̲c̲r̲o̲] [-𝐔n̲a̲m̲e̲] [f̲i̲l̲e̲ .̲.̲.̲]

𝐃𝐄𝐒𝐂𝐑𝐈𝐏𝐓𝐈𝐎𝐍
     The 𝗺𝟰 utility is a macro processor that can be used as a front end
     to any language (e.g., C, ratfor, fortran, lex, and yacc).  If no
     input files are given, 𝗺𝟰 reads from the standard input, otherwise
     files specified on the command line are processed in the given
     order.  Input files can be regular files, files in the m4 include
     paths, or a single dash (‘-’), denoting standard input.  𝗺𝟰 writes
     the processed text to the standard output, unless told otherwise.

     Macro calls have the form name(argument1[, argument2, ..., argu‐
     mentN]).

     There cannot be any space following the macro name and the open
     parenthesis (‘(’).  If the macro name is not followed by an open
     parenthesis it is processed with no arguments.

     Macro names consist of a leading alphabetic or underscore possibly
     followed by alphanumeric or underscore characters, e.g., valid
     macro names match the pattern “[a-zA-Z_][a-zA-Z0-9_]*”.

     In arguments to macros, leading unquoted space, tab, and newline
     (‘\n’) characters are ignored.  To quote strings, use left and
     right single quotes (e.g., ‘ this is a string with a leading
     space’).  You can change the quote characters with the 𝗰𝗵𝗮𝗻𝗴𝗲𝗾𝘂𝗼𝘁𝗲
     built-in macro.

     Most built-ins don't make any sense without arguments, and hence
     are not recognized as special when not followed by an open paren‐
     thesis.

     The options are as follows:

     -𝐃n̲a̲m̲e̲[=v̲a̲l̲u̲e̲]
             Define the symbol n̲a̲m̲e̲ to have some value (or NULL).

     -𝗱 f̲l̲a̲g̲s̲
             Set trace flags.  f̲l̲a̲g̲s̲ may hold the following:

             a̲       print macro arguments.

             c̲       print macro expansion over several lines.

             e̲       print result of macro expansion.

             f̲       print filename location.

             l̲       print line number.

             q̲       quote arguments and expansion with the current
                     quotes.

             t̲       start with all macros traced.

             x̲       number macro expansions.

             V̲       turn on all options.

             By default, trace is set to "eq".

     -𝐄      Set warnings to be fatal.  When a single -𝐄 flag is speci‐
             fied, if warnings are issued, execution continues but 𝗺𝟰
             will exit with a non-zero exit status.  When multiple -𝐄
             flags are specified, execution will halt upon issuing the
             first warning and 𝗺𝟰 will exit with a non-zero exit status.
             This behaviour matches GNU-m4 1.4.9 and later.

     -𝗴      Activate GNU-m4 compatibility mode.  In this mode, translit
             handles simple character ranges (e.g., a-z), regular
             expressions mimic emacs behavior, multiple m4wrap calls are
             handled as a stack, the number of diversions is unlimited,
             empty names for macro definitions are allowed, and eval
             understands ‘0rbase:value’ numbers.

     -𝐈 d̲i̲r̲n̲a̲m̲e̲
             Add directory d̲i̲r̲n̲a̲m̲e̲ to the include path.

     -𝗼 f̲i̲l̲e̲n̲a̲m̲e̲
             Send trace output to f̲i̲l̲e̲n̲a̲m̲e̲.

     -𝐏      Prefix all built-in macros with ‘m4_’.  For example,
             instead of writing 𝗱𝗲𝗳𝗶𝗻𝗲, use 𝗺𝟰_𝗱𝗲𝗳𝗶𝗻𝗲.

     -𝘀      Output line synchronization directives, suitable for
             cpp(1).

     -𝘁 m̲a̲c̲r̲o̲
             Turn tracing on for m̲a̲c̲r̲o̲.

     -𝐔n̲a̲m̲e̲  Undefine the symbol n̲a̲m̲e̲.

𝐒𝐘𝐍𝐓𝐀𝐗
     𝗺𝟰 provides the following built-in macros.  They may be redefined,
     losing their original meaning.  Return values are null unless oth‐
     erwise stated.

     𝗯𝘂𝗶𝗹𝘁𝗶𝗻(n̲a̲m̲e̲)
                  Calls a built-in by its n̲a̲m̲e̲, overriding possible
                  redefinitions.

     𝗰𝗵𝗮𝗻𝗴𝗲𝗰𝗼𝗺(s̲t̲a̲r̲t̲c̲o̲m̲m̲e̲n̲t̲, e̲n̲d̲c̲o̲m̲m̲e̲n̲t̲)
                  Changes the start comment and end comment sequences.
                  Comment sequences may be up to five characters long.
                  The default values are the hash sign and the newline
                  character.

                        # This is a comment

                  With no arguments, comments are turned off.  With one
                  single argument, the end comment sequence is set to
                  the newline character.

     𝗰𝗵𝗮𝗻𝗴𝗲𝗾𝘂𝗼𝘁𝗲(b̲e̲g̲i̲n̲q̲u̲o̲t̲e̲, e̲n̲d̲q̲u̲o̲t̲e̲)
                  Defines the open quote and close quote sequences.
                  Quote sequences may be up to five characters long.
                  The default values are the backquote character and the
                  quote character.

                        `Here is a quoted string'

                  With no arguments, the default quotes are restored.
                  With one single argument, the close quote sequence is
                  set to the newline character.

     𝗱𝗲𝗰𝗿(a̲r̲g̲)    Decrements the argument a̲r̲g̲ by 1.  The argument a̲r̲g̲
                  must be a valid numeric string.

     𝗱𝗲𝗳𝗶𝗻𝗲(n̲a̲m̲e̲, v̲a̲l̲u̲e̲)
                  Define a new macro named by the first argument n̲a̲m̲e̲ to
                  have the value of the second argument v̲a̲l̲u̲e̲.  Each
                  occurrence of ‘$n’ (where n̲ is 0 through 9) is
                  replaced by the n̲'th argument.  ‘$0’ is the name of
                  the calling macro.  Undefined arguments are replaced
                  by a null string.  ‘$#’ is replaced by the number of
                  arguments; ‘$*’ is replaced by all arguments comma
                  separated; ‘$@’ is the same as ‘$*’ but all arguments
                  are quoted against further expansion.

     𝗱𝗲𝗳𝗻(n̲a̲m̲e̲, .̲.̲.̲)
                  Returns the quoted definition for each argument.  This
                  can be used to rename macro definitions (even for
                  built-in macros).

     𝗱𝗶𝘃𝗲𝗿𝘁(n̲u̲m̲)  There are 10 output queues (numbered 0-9).  At the end
                  of processing 𝗺𝟰 concatenates all the queues in numer‐
                  ical order to produce the final output.  Initially the
                  output queue is 0.  The divert macro allows you to
                  select a new output queue (an invalid argument passed
                  to divert causes output to be discarded).

     𝗱𝗶𝘃𝗻𝘂𝗺       Returns the current output queue number.

     𝗱𝗻𝗹          Discard input characters up to and including the next
                  newline.

     𝗱𝘂𝗺𝗽𝗱𝗲𝗳(n̲a̲m̲e̲, .̲.̲.̲)
                  Prints the names and definitions for the named items,
                  or for everything if no arguments are passed.

     𝗲𝗿𝗿𝗽𝗿𝗶𝗻𝘁(m̲s̲g̲)
                  Prints the first argument on the standard error output
                  stream.

     𝗲𝘀𝘆𝘀𝗰𝗺𝗱(c̲m̲d̲)
                  Passes its first argument to a shell and returns the
                  shell's standard output.  Note that the shell shares
                  its standard input and standard error with 𝗺𝟰.

     𝗲𝘃𝗮𝗹(e̲x̲p̲r̲[̲,̲r̲a̲d̲i̲x̲[̲,̲m̲i̲n̲i̲m̲u̲m̲]̲]̲)
                  Computes the first argument as an arithmetic expres‐
                  sion using 32-bit arithmetic.  Operators are the stan‐
                  dard C ternary, arithmetic, logical, shift, rela‐
                  tional, bitwise, and parentheses operators.  You can
                  specify octal, decimal, and hexadecimal numbers as in
                  C.  The optional second argument r̲a̲d̲i̲x̲ specifies the
                  radix for the result and the optional third argument
                  m̲i̲n̲i̲m̲u̲m̲ specifies the minimum number of digits in the
                  result.

     𝗲𝘅𝗽𝗿(e̲x̲p̲r̲)   This is an alias for 𝗲𝘃𝗮𝗹.

     𝗳𝗼𝗿𝗺𝗮𝘁(f̲o̲r̲m̲a̲t̲s̲t̲r̲i̲n̲g̲, a̲r̲g̲1̲, .̲.̲.̲)
                  Returns f̲o̲r̲m̲a̲t̲s̲t̲r̲i̲n̲g̲ with escape sequences substituted
                  with a̲r̲g̲1̲ and following arguments, in a way similar to
                  printf(3).  This built-in is only available in GNU-m4
                  compatibility mode, and the only parameters imple‐
                  mented are there for autoconf compatibility: left-pad‐
                  ding flag, an optional field width, a maximum field
                  width, *-specified field widths, and the %s and %c
                  data type.

     𝗶𝗳𝗱𝗲𝗳(n̲a̲m̲e̲, y̲e̲s̲, n̲o̲)
                  If the macro named by the first argument is defined
                  then return the second argument, otherwise the third.
                  If there is no third argument, the value is NULL.  The
                  word "unix" is predefined.

     𝗶𝗳𝗲𝗹𝘀𝗲(a̲, b̲, y̲e̲s̲, .̲.̲.̲)
                  If the first argument a̲ matches the second argument b̲
                  then 𝗶𝗳𝗲𝗹𝘀𝗲() returns the third argument y̲e̲s̲.  If the
                  match fails the three arguments are discarded and the
                  next three arguments are used until there is zero or
                  one arguments left, either this last argument or NULL
                  is returned if no other matches were found.

     𝗶𝗻𝗰𝗹𝘂𝗱𝗲(n̲a̲m̲e̲)
                  Returns the contents of the file specified in the
                  first argument.  If the file is not found as is, look
                  through the include path: first the directories speci‐
                  fied with -𝐈 on the command line, then the environment
                  variable M4PATH, as a colon-separated list of directo‐
                  ries.  Include aborts with an error message if the
                  file cannot be included.

     𝗶𝗻𝗰𝗿(a̲r̲g̲)    Increments the argument by 1.  The argument must be a
                  valid numeric string.

     𝗶𝗻𝗱𝗲𝘅(s̲t̲r̲i̲n̲g̲, s̲u̲b̲s̲t̲r̲i̲n̲g̲)
                  Returns the index of the second argument in the first
                  argument (e.g., 𝗶𝗻𝗱𝗲𝘅(𝘁𝗵𝗲 𝗾𝘂𝗶𝗰𝗸 𝗯𝗿𝗼𝘄𝗻 𝗳𝗼𝘅 𝗷𝘂𝗺𝗽𝗲𝗱, 𝗳𝗼𝘅)
                  returns 16).  If the second argument is not found
                  index returns -1.

     𝗶𝗻𝗱𝗶𝗿(m̲a̲c̲r̲o̲, a̲r̲g̲1̲, .̲.̲.̲)
                  Indirectly calls the macro whose name is passed as the
                  first argument, with the remaining arguments passed as
                  first, ... arguments.

     𝗹𝗲𝗻(a̲r̲g̲)     Returns the number of characters in the first argu‐
                  ment.  Extra arguments are ignored.

     𝗺𝟰𝗲𝘅𝗶𝘁(c̲o̲d̲e̲)
                  Immediately exits with the return value specified by
                  the first argument, 0 if none.

     𝗺𝟰𝘄𝗿𝗮𝗽(t̲o̲d̲o̲)
                  Allows you to define what happens at the final EOF,
                  usually for cleanup purposes (e.g.,
                  𝗺𝟰𝘄𝗿𝗮𝗽("𝗰𝗹𝗲𝗮𝗻𝘂𝗽(𝘁𝗲𝗺𝗽𝗳𝗶𝗹𝗲)") causes the macro cleanup
                  to be invoked after all other processing is done).

                  Multiple calls to 𝗺𝟰𝘄𝗿𝗮𝗽() get inserted in sequence at
                  the final EOF.

     𝗺𝗮𝗸𝗲𝘁𝗲𝗺𝗽(t̲e̲m̲p̲l̲a̲t̲e̲)
                  Like 𝗺𝗸𝘀𝘁𝗲𝗺𝗽.

     𝗺𝗸𝘀𝘁𝗲𝗺𝗽(t̲e̲m̲p̲l̲a̲t̲e̲)
                  Invokes mkstemp(3) on the first argument, and returns
                  the modified string.  This can be used to create
                  unique temporary file names.

     𝗽𝗮𝘀𝘁𝗲(f̲i̲l̲e̲)  Includes the contents of the file specified by the
                  first argument without any macro processing.  Aborts
                  with an error message if the file cannot be included.

     𝗽𝗮𝘁𝘀𝘂𝗯𝘀𝘁(s̲t̲r̲i̲n̲g̲, r̲e̲g̲e̲x̲p̲, r̲e̲p̲l̲a̲c̲e̲m̲e̲n̲t̲)
                  Substitutes a regular expression in a string with a
                  replacement string.  Usual substitution patterns
                  apply: an ampersand (‘&’) is replaced by the string
                  matching the regular expression.  The string ‘\#’,
                  where ‘#’ is a digit, is replaced by the corresponding
                  back-reference.

     𝗽𝗼𝗽𝗱𝗲𝗳(a̲r̲g̲, .̲.̲.̲)
                  Restores the 𝗽𝘂𝘀𝗵𝗱𝗲𝗳ed definition for each argument.

     𝗽𝘂𝘀𝗵𝗱𝗲𝗳(m̲a̲c̲r̲o̲, d̲e̲f̲)
                  Takes the same arguments as 𝗱𝗲𝗳𝗶𝗻𝗲, but it saves the
                  definition on a stack for later retrieval by 𝗽𝗼𝗽𝗱𝗲𝗳().

     𝗿𝗲𝗴𝗲𝘅𝗽(s̲t̲r̲i̲n̲g̲, r̲e̲g̲e̲x̲p̲, r̲e̲p̲l̲a̲c̲e̲m̲e̲n̲t̲)
                  Finds a regular expression in a string.  If no further
                  arguments are given, it returns the first match posi‐
                  tion or -1 if no match.  If a third argument is pro‐
                  vided, it returns the replacement string, with sub-
                  patterns replaced.

     𝘀𝗵𝗶𝗳𝘁(a̲r̲g̲1̲, .̲.̲.̲)
                  Returns all but the first argument, the remaining
                  arguments are quoted and pushed back with commas in
                  between.  The quoting nullifies the effect of the
                  extra scan that will subsequently be performed.

     𝘀𝗶𝗻𝗰𝗹𝘂𝗱𝗲(f̲i̲l̲e̲)
                  Similar to 𝗶𝗻𝗰𝗹𝘂𝗱𝗲, except it ignores any errors.

     𝘀𝗽𝗮𝘀𝘁𝗲(f̲i̲l̲e̲)
                  Similar to 𝗽𝗮𝘀𝘁𝗲(), except it ignores any errors.

     𝘀𝘂𝗯𝘀𝘁𝗿(s̲t̲r̲i̲n̲g̲, o̲f̲f̲s̲e̲t̲, l̲e̲n̲g̲t̲h̲)
                  Returns a substring of the first argument starting at
                  the offset specified by the second argument and the
                  length specified by the third argument.  If no third
                  argument is present it returns the rest of the string.

     𝘀𝘆𝘀𝗰𝗺𝗱(c̲m̲d̲)  Passes the first argument to the shell.  Nothing is
                  returned.

     𝘀𝘆𝘀𝘃𝗮𝗹       Returns the return value from the last 𝘀𝘆𝘀𝗰𝗺𝗱.

     𝘁𝗿𝗮𝗰𝗲𝗼𝗻(a̲r̲g̲, .̲.̲.̲)
                  Enables tracing of macro expansions for the given
                  arguments, or for all macros if no argument is given.

     𝘁𝗿𝗮𝗰𝗲𝗼𝗳𝗳(a̲r̲g̲, .̲.̲.̲)
                  Disables tracing of macro expansions for the given
                  arguments, or for all macros if no argument is given.

     𝘁𝗿𝗮𝗻𝘀𝗹𝗶𝘁(s̲t̲r̲i̲n̲g̲, m̲a̲p̲f̲r̲o̲m̲, m̲a̲p̲t̲o̲)
                  Transliterate the characters in the first argument
                  from the set given by the second argument to the set
                  given by the third.  You cannot use tr(1) style abbre‐
                  viations.

     𝘂𝗻𝗱𝗲𝗳𝗶𝗻𝗲(n̲a̲m̲e̲1̲, .̲.̲.̲)
                  Removes the definition for the macros specified by its
                  arguments.

     𝘂𝗻𝗱𝗶𝘃𝗲𝗿𝘁(a̲r̲g̲, .̲.̲.̲)
                  Flushes the named output queues (or all queues if no
                  arguments).

     𝘂𝗻𝗶𝘅         A pre-defined macro for testing the OS platform.

     __𝗹𝗶𝗻𝗲__     Returns the current file's line number.

     __𝗳𝗶𝗹𝗲__     Returns the current file's name.

𝐄𝐗𝐈𝐓 𝐒𝐓𝐀𝐓𝐔𝐒
     The 𝗺𝟰 utility exits 0 on success, and >0 if an error occurs.

     But note that the 𝗺𝟰𝗲𝘅𝗶𝘁 macro can modify the exit status, as can
     the -𝐄 flag.

𝐒𝐓𝐀𝐍𝐃𝐀𝐑𝐃𝐒
     The 𝗺𝟰 utility is compliant with the IEEE Std 1003.1-2008
     (“POSIX.1”) specification.

     The flags [-𝗱𝐄𝗴𝐈𝐏𝗼𝘁] and the macros 𝗯𝘂𝗶𝗹𝘁𝗶𝗻, 𝗲𝘀𝘆𝘀𝗰𝗺𝗱, 𝗲𝘅𝗽𝗿, 𝗳𝗼𝗿𝗺𝗮𝘁,
     𝗶𝗻𝗱𝗶𝗿, 𝗽𝗮𝘀𝘁𝗲, 𝗽𝗮𝘁𝘀𝘂𝗯𝘀𝘁, 𝗿𝗲𝗴𝗲𝘅𝗽, 𝘀𝗽𝗮𝘀𝘁𝗲, 𝘂𝗻𝗶𝘅, __𝗹𝗶𝗻𝗲__, and
     __𝗳𝗶𝗹𝗲__ are extensions to that specification.

     𝗺𝗮𝗸𝗲𝘁𝗲𝗺𝗽 is not supposed to be a synonym for 𝗺𝗸𝘀𝘁𝗲𝗺𝗽, but instead
     to be an insecure temporary file name creation function.  It is
     marked by IEEE Std 1003.1-2008 (“POSIX.1”) as being obsolescent and
     should not be used if portability is a concern.

     The output format of 𝘁𝗿𝗮𝗰𝗲𝗼𝗻 and 𝗱𝘂𝗺𝗽𝗱𝗲𝗳 are not specified in any
     standard, are likely to change and should not be relied upon.  The
     current format of tracing is closely modelled on 𝗴𝗻𝘂-𝗺𝟰, to allow
     𝗮𝘂𝘁𝗼𝗰𝗼𝗻𝗳 to work.

     The built-ins 𝗽𝘂𝘀𝗵𝗱𝗲𝗳 and 𝗽𝗼𝗽𝗱𝗲𝗳 handle macro definitions as a
     stack.  However, 𝗱𝗲𝗳𝗶𝗻𝗲 interacts with the stack in an undefined
     way.  In this implementation, 𝗱𝗲𝗳𝗶𝗻𝗲 replaces the top-most defini‐
     tion only.  Other implementations may erase all definitions on the
     stack instead.

     All built-ins do expand without arguments in many other 𝗺𝟰.

     Many other 𝗺𝟰 have dire size limitations with respect to buffer
     sizes.

𝐀𝐔𝐓𝐇𝐎𝐑𝐒
     Ozan Yigit <o̲z̲@̲s̲i̲s̲.̲y̲o̲r̲k̲u̲.̲c̲a̲> and Richard A. O'Keefe
     <o̲k̲@̲g̲o̲a̲n̲n̲a̲.̲c̲s̲.̲r̲m̲i̲t̲.̲O̲Z̲.̲A̲U̲>.

     GNU-m4 compatibility extensions by Marc Espie
     <e̲s̲p̲i̲e̲@̲c̲v̲s̲.̲o̲p̲e̲n̲b̲s̲d̲.̲o̲r̲g̲>.

COSMOPOLITAN                       June 15, 2017                           BSD
