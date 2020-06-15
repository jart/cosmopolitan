FLEX(1)             Cosmopolitan General Commands Manual              -*-text-*-

𝐍𝐀𝐌𝐄
     𝗳𝗹𝗲𝘅, 𝗳𝗹𝗲𝘅++, 𝗹𝗲𝘅 — fast lexical analyzer generator

𝐒𝐘𝐍𝐎𝐏𝐒𝐈𝐒
     𝗳𝗹𝗲𝘅 [-𝟳𝟴𝐁𝗯𝗱𝐅𝗳𝗵𝐈𝗶𝐋𝗹𝗻𝗽𝘀𝐓𝘁𝐕𝘃𝘄+?] [-𝐂[𝗮𝗲𝐅𝗳𝗺𝗿]] [--𝗵𝗲𝗹𝗽] [--𝘃𝗲𝗿𝘀𝗶𝗼𝗻]
          [-𝗼o̲u̲t̲p̲u̲t̲] [-𝐏p̲r̲e̲f̲i̲x̲] [-𝐒s̲k̲e̲l̲e̲t̲o̲n̲] [f̲i̲l̲e̲ .̲.̲.̲]

𝐃𝐄𝐒𝐂𝐑𝐈𝐏𝐓𝐈𝐎𝐍
     𝗳𝗹𝗲𝘅 is a tool for generating s̲c̲a̲n̲n̲e̲r̲s̲: programs which recognize
     lexical patterns in text.  𝗳𝗹𝗲𝘅 reads the given input files, or its
     standard input if no file names are given, for a description of a
     scanner to generate.  The description is in the form of pairs of
     regular expressions and C code, called r̲u̲l̲e̲s̲.  𝗳𝗹𝗲𝘅 generates as
     output a C source file, l̲e̲x̲.̲y̲y̲.̲c̲, which defines a routine 𝘆𝘆𝗹𝗲𝘅().
     This file is compiled and linked with the -𝗹𝗳𝗹 library to produce
     an executable.  When the executable is run, it analyzes its input
     for occurrences of the regular expressions.  Whenever it finds one,
     it executes the corresponding C code.

     𝗹𝗲𝘅 is a synonym for 𝗳𝗹𝗲𝘅.  𝗳𝗹𝗲𝘅++ is a synonym for 𝗳𝗹𝗲𝘅 -+.

     The manual includes both tutorial and reference sections:

     𝐒𝗼𝗺𝗲 𝐒𝗶𝗺𝗽𝗹𝗲 𝐄𝘅𝗮𝗺𝗽𝗹𝗲𝘀

     𝐅𝗼𝗿𝗺𝗮𝘁 𝗼𝗳 𝘁𝗵𝗲 𝐈𝗻𝗽𝘂𝘁 𝐅𝗶𝗹𝗲

     𝐏𝗮𝘁𝘁𝗲𝗿𝗻𝘀
     The extended regular expressions used by 𝗳𝗹𝗲𝘅.

     𝐇𝗼𝘄 𝘁𝗵𝗲 𝐈𝗻𝗽𝘂𝘁 𝗶𝘀 𝐌𝗮𝘁𝗰𝗵𝗲𝗱
     The rules for determining what has been matched.

     𝐀𝗰𝘁𝗶𝗼𝗻𝘀
     How to specify what to do when a pattern is matched.

     𝐓𝗵𝗲 𝐆𝗲𝗻𝗲𝗿𝗮𝘁𝗲𝗱 𝐒𝗰𝗮𝗻𝗻𝗲𝗿
     Details regarding the scanner that 𝗳𝗹𝗲𝘅 produces; how to control
     the input source.

     𝐒𝘁𝗮𝗿𝘁 𝐂𝗼𝗻𝗱𝗶𝘁𝗶𝗼𝗻𝘀
     Introducing context into scanners, and managing "mini-scanners".

     𝐌𝘂𝗹𝘁𝗶𝗽𝗹𝗲 𝐈𝗻𝗽𝘂𝘁 𝐁𝘂𝗳𝗳𝗲𝗿𝘀
     How to manipulate multiple input sources; how to scan from strings
     instead of files.

     𝐄𝗻𝗱-𝗼𝗳-𝐅𝗶𝗹𝗲 𝐑𝘂𝗹𝗲𝘀
     Special rules for matching the end of the input.

     𝐌𝗶𝘀𝗰𝗲𝗹𝗹𝗮𝗻𝗲𝗼𝘂𝘀 𝐌𝗮𝗰𝗿𝗼𝘀
     A summary of macros available to the actions.

     𝐕𝗮𝗹𝘂𝗲𝘀 𝐀𝘃𝗮𝗶𝗹𝗮𝗯𝗹𝗲 𝘁𝗼 𝘁𝗵𝗲 𝐔𝘀𝗲𝗿
     A summary of values available to the actions.

     𝐈𝗻𝘁𝗲𝗿𝗳𝗮𝗰𝗶𝗻𝗴 𝘄𝗶𝘁𝗵 𝐘𝗮𝗰𝗰
     Connecting flex scanners together with yacc(1) parsers.

     𝐎𝗽𝘁𝗶𝗼𝗻𝘀
     𝗳𝗹𝗲𝘅 command-line options, and the “%option” directive.

     𝐏𝗲𝗿𝗳𝗼𝗿𝗺𝗮𝗻𝗰𝗲 𝐂𝗼𝗻𝘀𝗶𝗱𝗲𝗿𝗮𝘁𝗶𝗼𝗻𝘀
     How to make scanners go as fast as possible.

     𝐆𝗲𝗻𝗲𝗿𝗮𝘁𝗶𝗻𝗴 𝐂++ 𝐒𝗰𝗮𝗻𝗻𝗲𝗿𝘀
     The (experimental) facility for generating C++ scanner classes.

     𝐈𝗻𝗰𝗼𝗺𝗽𝗮𝘁𝗶𝗯𝗶𝗹𝗶𝘁𝗶𝗲𝘀 𝘄𝗶𝘁𝗵 𝐋𝗲𝘅 𝗮𝗻𝗱 𝐏𝐎𝐒𝐈𝐗
     How 𝗳𝗹𝗲𝘅 differs from AT&T UNIX 𝗹𝗲𝘅 and the POSIX 𝗹𝗲𝘅 standard.

     𝐅𝗶𝗹𝗲𝘀
     Files used by 𝗳𝗹𝗲𝘅.

     𝐃𝗶𝗮𝗴𝗻𝗼𝘀𝘁𝗶𝗰𝘀
     Those error messages produced by 𝗳𝗹𝗲𝘅 (or scanners it generates)
     whose meanings might not be apparent.

     𝐒𝗲𝗲 𝐀𝗹𝘀𝗼
     Other documentation, related tools.

     𝐀𝘂𝘁𝗵𝗼𝗿𝘀
     Includes contact information.

     𝐁𝘂𝗴𝘀
     Known problems with 𝗳𝗹𝗲𝘅.

𝐒𝐎𝐌𝐄 𝐒𝐈𝐌𝐏𝐋𝐄 𝐄𝐗𝐀𝐌𝐏𝐋𝐄𝐒
     First some simple examples to get the flavor of how one uses 𝗳𝗹𝗲𝘅.
     The following 𝗳𝗹𝗲𝘅 input specifies a scanner which whenever it
     encounters the string "username" will replace it with the user's
     login name:

           %%
           username    printf("%s", getlogin());

     By default, any text not matched by a 𝗳𝗹𝗲𝘅 scanner is copied to the
     output, so the net effect of this scanner is to copy its input file
     to its output with each occurrence of "username" expanded.  In this
     input, there is just one rule.  "username" is the p̲a̲t̲t̲e̲r̲n̲ and the
     "printf" is the a̲c̲t̲i̲o̲n̲.  The "%%" marks the beginning of the rules.

     Here's another simple example:

           %{
           int num_lines = 0, num_chars = 0;
           %}

           %%
           \n      ++num_lines; ++num_chars;
           .       ++num_chars;

           %%
           main()
           {
                   yylex();
                   printf("# of lines = %d, # of chars = %d\n",
                       num_lines, num_chars);
           }

     This scanner counts the number of characters and the number of
     lines in its input (it produces no output other than the final
     report on the counts).  The first line declares two globals,
     "num_lines" and "num_chars", which are accessible both inside
     𝘆𝘆𝗹𝗲𝘅() and in the 𝗺𝗮𝗶𝗻() routine declared after the second "%%".
     There are two rules, one which matches a newline ("\n") and incre‐
     ments both the line count and the character count, and one which
     matches any character other than a newline (indicated by the "."
     regular expression).

     A somewhat more complicated example:

           /* scanner for a toy Pascal-like language */

           %{
           /* need this for the call to atof() below */
           #include <math.h>
           %}

           DIGIT    [0-9]
           ID       [a-z][a-z0-9]*

           %%

           {DIGIT}+ {
                   printf("An integer: %s (%d)\n", yytext,
                       atoi(yytext));
           }

           {DIGIT}+"."{DIGIT}* {
                   printf("A float: %s (%g)\n", yytext,
                       atof(yytext));
           }

           if|then|begin|end|procedure|function {
                   printf("A keyword: %s\n", yytext);
           }

           {ID}    printf("An identifier: %s\n", yytext);

           "+"|"-"|"*"|"/"   printf("An operator: %s\n", yytext);

           "{"[^}\n]*"}"     /* eat up one-line comments */

           [ \t\n]+          /* eat up whitespace */

           .       printf("Unrecognized character: %s\n", yytext);

           %%

           main(int argc, char *argv[])
           {
                   ++argv; --argc;  /* skip over program name */
                   if (argc > 0)
                           yyin = fopen(argv[0], "r");
                   else
                           yyin = stdin;

                   yylex();
           }

     This is the beginnings of a simple scanner for a language like Pas‐
     cal.  It identifies different types of t̲o̲k̲e̲n̲s̲ and reports on what
     it has seen.

     The details of this example will be explained in the following sec‐
     tions.

𝐅𝐎𝐑𝐌𝐀𝐓 𝐎𝐅 𝐓𝐇𝐄 𝐈𝐍𝐏𝐔𝐓 𝐅𝐈𝐋𝐄
     The 𝗳𝗹𝗲𝘅 input file consists of three sections, separated by a line
     with just "%%" in it:

           definitions
           %%
           rules
           %%
           user code

     The d̲e̲f̲i̲n̲i̲t̲i̲o̲n̲s̲ section contains declarations of simple n̲a̲m̲e̲ defi‐
     nitions to simplify the scanner specification, and declarations of
     s̲t̲a̲r̲t̲ c̲o̲n̲d̲i̲t̲i̲o̲n̲s̲, which are explained in a later section.

     Name definitions have the form:

           name definition

     The "name" is a word beginning with a letter or an underscore (‘_’)
     followed by zero or more letters, digits, ‘_’, or ‘-’ (dash).  The
     definition is taken to begin at the first non-whitespace character
     following the name and continuing to the end of the line.  The def‐
     inition can subsequently be referred to using "{name}", which will
     expand to "(definition)".  For example:

           DIGIT    [0-9]
           ID       [a-z][a-z0-9]*

     This defines "DIGIT" to be a regular expression which matches a
     single digit, and "ID" to be a regular expression which matches a
     letter followed by zero-or-more letters-or-digits.  A subsequent
     reference to

           {DIGIT}+"."{DIGIT}*

     is identical to

           ([0-9])+"."([0-9])*

     and matches one-or-more digits followed by a ‘.’ followed by zero-
     or-more digits.

     The r̲u̲l̲e̲s̲ section of the 𝗳𝗹𝗲𝘅 input contains a series of rules of
     the form:

           pattern   action

     The pattern must be unindented and the action must begin on the
     same line.

     See below for a further description of patterns and actions.

     Finally, the user code section is simply copied to l̲e̲x̲.̲y̲y̲.̲c̲ verba‐
     tim.  It is used for companion routines which call or are called by
     the scanner.  The presence of this section is optional; if it is
     missing, the second "%%" in the input file may be skipped too.

     In the definitions and rules sections, any indented text or text
     enclosed in ‘%{’ and ‘%}’ is copied verbatim to the output (with
     the %{}'s removed).  The %{}'s must appear unindented on lines by
     themselves.

     In the rules section, any indented or %{} text appearing before the
     first rule may be used to declare variables which are local to the
     scanning routine and (after the declarations) code which is to be
     executed whenever the scanning routine is entered.  Other indented
     or %{} text in the rule section is still copied to the output, but
     its meaning is not well-defined and it may well cause compile-time
     errors (this feature is present for POSIX compliance; see below for
     other such features).

     In the definitions section (but not in the rules section), an unin‐
     dented comment (i.e., a line beginning with "/*") is also copied
     verbatim to the output up to the next "*/".

𝐏𝐀𝐓𝐓𝐄𝐑𝐍𝐒
     The patterns in the input are written using an extended set of reg‐
     ular expressions.  These are:

     x         Match the character ‘x’.

     .         Any character (byte) except newline.

     [xyz]     A "character class"; in this case, the pattern matches
               either an ‘x’, a ‘y’, or a ‘z’.

     [abj-oZ]  A "character class" with a range in it; matches an ‘a’, a
               ‘b’, any letter from ‘j’ through ‘o’, or a ‘Z’.

     [^A-Z]    A "negated character class", i.e., any character but
               those in the class.  In this case, any character EXCEPT
               an uppercase letter.

     [^A-Z\n]  Any character EXCEPT an uppercase letter or a newline.

     r*        Zero or more r's, where ‘r’ is any regular expression.

     r+        One or more r's.

     r?        Zero or one r's (that is, "an optional r").

     r{2,5}    Anywhere from two to five r's.

     r{2,}     Two or more r's.

     r{4}      Exactly 4 r's.

     {name}    The expansion of the "name" definition (see above).

     "[xyz]\"foo"
               The literal string: [xyz]"foo.

     \X        If ‘X’ is an ‘a’, ‘b’, ‘f’, ‘n’, ‘r’, ‘t’, or ‘v’, then
               the ANSI-C interpretation of ‘\X’.  Otherwise, a literal
               ‘X’ (used to escape operators such as ‘*’).

     \0        A NUL character (ASCII code 0).

     \123      The character with octal value 123.

     \x2a      The character with hexadecimal value 2a.

     (r)       Match an ‘r’; parentheses are used to override precedence
               (see below).

     rs        The regular expression ‘r’ followed by the regular
               expression ‘s’; called "concatenation".

     r|s       Either an ‘r’ or an ‘s’.

     r/s       An ‘r’, but only if it is followed by an ‘s’.  The text
               matched by ‘s’ is included when determining whether this
               rule is the "longest match", but is then returned to the
               input before the action is executed.  So the action only
               sees the text matched by ‘r’.  This type of pattern is
               called "trailing context".  (There are some combinations
               of r/s that 𝗳𝗹𝗲𝘅 cannot match correctly; see notes in the
               B̲U̲G̲S̲ section below regarding "dangerous trailing
               context".)

     ^r        An ‘r’, but only at the beginning of a line (i.e., just
               starting to scan, or right after a newline has been
               scanned).

     r$        An ‘r’, but only at the end of a line (i.e., just before
               a newline).  Equivalent to "r/\n".

               Note that 𝗳𝗹𝗲𝘅's notion of "newline" is exactly whatever
               the C compiler used to compile 𝗳𝗹𝗲𝘅 interprets ‘\n’ as.

     <s>r      An ‘r’, but only in start condition ‘s’ (see below for
               discussion of start conditions).

     <s1,s2,s3>r
               The same, but in any of start conditions s1, s2, or s3.

     <*>r      An ‘r’ in any start condition, even an exclusive one.

     <<EOF>>   An end-of-file.

     <s1,s2><<EOF>>
               An end-of-file when in start condition s1 or s2.

     Note that inside of a character class, all regular expression oper‐
     ators lose their special meaning except escape (‘\’) and the char‐
     acter class operators, ‘-’, ‘]’, and, at the beginning of the
     class, ‘^’.

     The regular expressions listed above are grouped according to
     precedence, from highest precedence at the top to lowest at the
     bottom.  Those grouped together have equal precedence.  For exam‐
     ple,

           foo|bar*

     is the same as

           (foo)|(ba(r*))

     since the ‘*’ operator has higher precedence than concatenation,
     and concatenation higher than alternation (‘|’).  This pattern
     therefore matches e̲i̲t̲h̲e̲r̲ the string "foo" o̲r̲ the string "ba" fol‐
     lowed by zero-or-more r's.  To match "foo" or zero-or-more "bar"'s,
     use:

           foo|(bar)*

     and to match zero-or-more "foo"'s-or-"bar"'s:

           (foo|bar)*

     In addition to characters and ranges of characters, character
     classes can also contain character class e̲x̲p̲r̲e̲s̲s̲i̲o̲n̲s̲.  These are
     expressions enclosed inside ‘[:’ and ‘:]’ delimiters (which them‐
     selves must appear between the ‘[’ and ‘]’ of the character class;
     other elements may occur inside the character class, too).  The
     valid expressions are:

           [:alnum:] [:alpha:] [:blank:]
           [:cntrl:] [:digit:] [:graph:]
           [:lower:] [:print:] [:punct:]
           [:space:] [:upper:] [:xdigit:]

     These expressions all designate a set of characters equivalent to
     the corresponding standard C 𝗶𝘀𝐗𝐗𝐗() function.  For example,
     [:alnum:] designates those characters for which isalnum(3) returns
     true - i.e., any alphabetic or numeric.  Some systems don't provide
     isblank(3), so 𝗳𝗹𝗲𝘅 defines [:blank:] as a blank or a tab.

     For example, the following character classes are all equivalent:

           [[:alnum:]]
           [[:alpha:][:digit:]]
           [[:alpha:]0-9]
           [a-zA-Z0-9]

     If the scanner is case-insensitive (the -𝗶 flag), then [:upper:]
     and [:lower:] are equivalent to [:alpha:].

     Some notes on patterns:

     -   A negated character class such as the example "[^A-Z]" above
         will match a newline unless "\n" (or an equivalent escape
         sequence) is one of the characters explicitly present in the
         negated character class (e.g., "[^A-Z\n]").  This is unlike how
         many other regular expression tools treat negated character
         classes, but unfortunately the inconsistency is historically
         entrenched.  Matching newlines means that a pattern like
         "[^"]*" can match the entire input unless there's another quote
         in the input.

     -   A rule can have at most one instance of trailing context (the
         ‘/’ operator or the ‘$’ operator).  The start condition, ‘^’,
         and "<<EOF>>" patterns can only occur at the beginning of a
         pattern and, as well as with ‘/’ and ‘$’, cannot be grouped
         inside parentheses.  A ‘^’ which does not occur at the begin‐
         ning of a rule or a ‘$’ which does not occur at the end of a
         rule loses its special properties and is treated as a normal
         character.

     -   The following are illegal:

               foo/bar$
               <sc1>foo<sc2>bar

         Note that the first of these, can be written "foo/bar\n".

     -   The following will result in ‘$’ or ‘^’ being treated as a nor‐
         mal character:

               foo|(bar$)
               foo|^bar

         If what's wanted is a "foo" or a bar-followed-by-a-newline, the
         following could be used (the special ‘|’ action is explained
         below):

               foo      |
               bar$     /* action goes here */

         A similar trick will work for matching a foo or a bar-at-the-
         beginning-of-a-line.

𝐇𝐎𝐖 𝐓𝐇𝐄 𝐈𝐍𝐏𝐔𝐓 𝐈𝐒 𝐌𝐀𝐓𝐂𝐇𝐄𝐃
     When the generated scanner is run, it analyzes its input looking
     for strings which match any of its patterns.  If it finds more than
     one match, it takes the one matching the most text (for trailing
     context rules, this includes the length of the trailing part, even
     though it will then be returned to the input).  If it finds two or
     more matches of the same length, the rule listed first in the 𝗳𝗹𝗲𝘅
     input file is chosen.

     Once the match is determined, the text corresponding to the match
     (called the t̲o̲k̲e̲n̲) is made available in the global character
     pointer y̲y̲t̲e̲x̲t̲, and its length in the global integer y̲y̲l̲e̲n̲g̲.  The
     a̲c̲t̲i̲o̲n̲ corresponding to the matched pattern is then executed (a
     more detailed description of actions follows), and then the remain‐
     ing input is scanned for another match.

     If no match is found, then the default rule is executed: the next
     character in the input is considered matched and copied to the
     standard output.  Thus, the simplest legal 𝗳𝗹𝗲𝘅 input is:

           %%

     which generates a scanner that simply copies its input (one
     character at a time) to its output.

     Note that y̲y̲t̲e̲x̲t̲ can be defined in two different ways: either as a
     character pointer or as a character array.  Which definition 𝗳𝗹𝗲𝘅
     uses can be controlled by including one of the special directives
     “%pointer” or “%array” in the first (definitions) section of flex
     input.  The default is “%pointer”, unless the -𝗹 𝗹𝗲𝘅 compatibility
     option is used, in which case y̲y̲t̲e̲x̲t̲ will be an array.  The advan‐
     tage of using “%pointer” is substantially faster scanning and no
     buffer overflow when matching very large tokens (unless not enough
     dynamic memory is available).  The disadvantage is that actions are
     restricted in how they can modify y̲y̲t̲e̲x̲t̲ (see the next section),
     and calls to the 𝘂𝗻𝗽𝘂𝘁() function destroy the present contents of
     y̲y̲t̲e̲x̲t̲, which can be a considerable porting headache when moving
     between different 𝗹𝗲𝘅 versions.

     The advantage of “%array” is that y̲y̲t̲e̲x̲t̲ can be modified as much as
     wanted, and calls to 𝘂𝗻𝗽𝘂𝘁() do not destroy y̲y̲t̲e̲x̲t̲ (see below).
     Furthermore, existing 𝗹𝗲𝘅 programs sometimes access y̲y̲t̲e̲x̲t̲ exter‐
     nally using declarations of the form:

           extern char yytext[];

     This definition is erroneous when used with “%pointer”, but correct
     for “%array”.

     “%array” defines y̲y̲t̲e̲x̲t̲ to be an array of YYLMAX characters, which
     defaults to a fairly large value.  The size can be changed by sim‐
     ply #define'ing YYLMAX to a different value in the first section of
     𝗳𝗹𝗲𝘅 input.  As mentioned above, with “%pointer” yytext grows
     dynamically to accommodate large tokens.  While this means a
     “%pointer” scanner can accommodate very large tokens (such as
     matching entire blocks of comments), bear in mind that each time
     the scanner must resize y̲y̲t̲e̲x̲t̲ it also must rescan the entire token
     from the beginning, so matching such tokens can prove slow.  y̲y̲t̲e̲x̲t̲
     presently does not dynamically grow if a call to 𝘂𝗻𝗽𝘂𝘁() results in
     too much text being pushed back; instead, a run-time error results.

     Also note that “%array” cannot be used with C++ scanner classes
     (the c++ option; see below).

𝐀𝐂𝐓𝐈𝐎𝐍𝐒
     Each pattern in a rule has a corresponding action, which can be any
     arbitrary C statement.  The pattern ends at the first non-escaped
     whitespace character; the remainder of the line is its action.  If
     the action is empty, then when the pattern is matched the input
     token is simply discarded.  For example, here is the specification
     for a program which deletes all occurrences of "zap me" from its
     input:

           %%
           "zap me"

     (It will copy all other characters in the input to the output since
     they will be matched by the default rule.)

     Here is a program which compresses multiple blanks and tabs down to
     a single blank, and throws away whitespace found at the end of a
     line:

           %%
           [ \t]+        putchar(' ');
           [ \t]+$       /* ignore this token */

     If the action contains a ‘{’, then the action spans till the bal‐
     ancing ‘}’ is found, and the action may cross multiple lines.  𝗳𝗹𝗲𝘅
     knows about C strings and comments and won't be fooled by braces
     found within them, but also allows actions to begin with ‘%{’ and
     will consider the action to be all the text up to the next ‘%}’
     (regardless of ordinary braces inside the action).

     An action consisting solely of a vertical bar (‘|’) means "same as
     the action for the next rule".  See below for an illustration.

     Actions can include arbitrary C code, including return statements
     to return a value to whatever routine called 𝘆𝘆𝗹𝗲𝘅().  Each time
     𝘆𝘆𝗹𝗲𝘅() is called, it continues processing tokens from where it
     last left off until it either reaches the end of the file or exe‐
     cutes a return.

     Actions are free to modify y̲y̲t̲e̲x̲t̲ except for lengthening it (adding
     characters to its end - these will overwrite later characters in
     the input stream).  This, however, does not apply when using
     “%array” (see above); in that case, y̲y̲t̲e̲x̲t̲ may be freely modified
     in any way.

     Actions are free to modify y̲y̲l̲e̲n̲g̲ except they should not do so if
     the action also includes use of 𝘆𝘆𝗺𝗼𝗿𝗲() (see below).

     There are a number of special directives which can be included
     within an action:

     ECHO    Copies y̲y̲t̲e̲x̲t̲ to the scanner's output.

     BEGIN   Followed by the name of a start condition, places the scan‐
             ner in the corresponding start condition (see below).

     REJECT  Directs the scanner to proceed on to the "second best" rule
             which matched the input (or a prefix of the input).  The
             rule is chosen as described above in H̲O̲W̲ T̲H̲E̲ I̲N̲P̲U̲T̲ I̲S̲
             M̲A̲T̲C̲H̲E̲D̲, and y̲y̲t̲e̲x̲t̲ and y̲y̲l̲e̲n̲g̲ set up appropriately.  It
             may either be one which matched as much text as the origi‐
             nally chosen rule but came later in the 𝗳𝗹𝗲𝘅 input file, or
             one which matched less text.  For example, the following
             will both count the words in the input and call the routine
             𝘀𝗽𝗲𝗰𝗶𝗮𝗹() whenever "frob" is seen:

                   int word_count = 0;
                   %%

                   frob        special(); REJECT;
                   [^ \t\n]+   ++word_count;

             Without the R̲E̲J̲E̲C̲T̲, any "frob"'s in the input would not be
             counted as words, since the scanner normally executes only
             one action per token.  Multiple R̲E̲J̲E̲C̲T̲'s are allowed, each
             one finding the next best choice to the currently active
             rule.  For example, when the following scanner scans the
             token "abcd", it will write "abcdabcaba" to the output:

                   %%
                   a        |
                   ab       |
                   abc      |
                   abcd     ECHO; REJECT;
                   .|\n     /* eat up any unmatched character */

             (The first three rules share the fourth's action since they
             use the special ‘|’ action.)  R̲E̲J̲E̲C̲T̲ is a particularly
             expensive feature in terms of scanner performance; if it is
             used in any of the scanner's actions it will slow down all
             of the scanner's matching.  Furthermore, R̲E̲J̲E̲C̲T̲ cannot be
             used with the -𝐂𝗳 or -𝐂𝐅 options (see below).

             Note also that unlike the other special actions, R̲E̲J̲E̲C̲T̲ is
             a b̲r̲a̲n̲c̲h̲; code immediately following it in the action will
             not be executed.

     yymore()
             Tells the scanner that the next time it matches a rule, the
             corresponding token should be appended onto the current
             value of y̲y̲t̲e̲x̲t̲ rather than replacing it.  For example,
             given the input "mega-kludge" the following will write
             "mega-mega-kludge" to the output:

                   %%
                   mega-    ECHO; yymore();
                   kludge   ECHO;

             First "mega-" is matched and echoed to the output.  Then
             "kludge" is matched, but the previous "mega-" is still
             hanging around at the beginning of y̲y̲t̲e̲x̲t̲ so the E̲C̲H̲O̲ for
             the "kludge" rule will actually write "mega-kludge".

             Two notes regarding use of 𝘆𝘆𝗺𝗼𝗿𝗲(): First, 𝘆𝘆𝗺𝗼𝗿𝗲()
             depends on the value of y̲y̲l̲e̲n̲g̲ correctly reflecting the
             size of the current token, so y̲y̲l̲e̲n̲g̲ must not be modified
             when using 𝘆𝘆𝗺𝗼𝗿𝗲().  Second, the presence of 𝘆𝘆𝗺𝗼𝗿𝗲() in
             the scanner's action entails a minor performance penalty in
             the scanner's matching speed.

     yyless(n)
             Returns all but the first n̲ characters of the current token
             back to the input stream, where they will be rescanned when
             the scanner looks for the next match.  y̲y̲t̲e̲x̲t̲ and y̲y̲l̲e̲n̲g̲
             are adjusted appropriately (e.g., y̲y̲l̲e̲n̲g̲ will now be equal
             to n̲).  For example, on the input "foobar" the following
             will write out "foobarbar":

                   %%
                   foobar    ECHO; yyless(3);
                   [a-z]+    ECHO;

             An argument of 0 to y̲y̲l̲e̲s̲s̲ will cause the entire current
             input string to be scanned again.  Unless how the scanner
             will subsequently process its input has been changed (using
             B̲E̲G̲I̲N̲, for example), this will result in an endless loop.

             Note that y̲y̲l̲e̲s̲s̲ is a macro and can only be used in the
             𝗳𝗹𝗲𝘅 input file, not from other source files.

     unput(c)
             Puts the character c̲ back into the input stream.  It will
             be the next character scanned.  The following action will
             take the current token and cause it to be rescanned
             enclosed in parentheses.

                   {
                           int i;
                           char *yycopy;

                           /* Copy yytext because unput() trashes yytext */
                           if ((yycopy = strdup(yytext)) == NULL)
                                   err(1, NULL);
                           unput(')');
                           for (i = yyleng - 1; i >= 0; --i)
                                   unput(yycopy[i]);
                           unput('(');
                           free(yycopy);
                   }

             Note that since each 𝘂𝗻𝗽𝘂𝘁() puts the given character back
             at the beginning of the input stream, pushing back strings
             must be done back-to-front.

             An important potential problem when using 𝘂𝗻𝗽𝘂𝘁() is that
             if using “%pointer” (the default), a call to 𝘂𝗻𝗽𝘂𝘁()
             destroys the contents of y̲y̲t̲e̲x̲t̲, starting with its right‐
             most character and devouring one character to the left with
             each call.  If the value of y̲y̲t̲e̲x̲t̲ should be preserved
             after a call to 𝘂𝗻𝗽𝘂𝘁() (as in the above example), it must
             either first be copied elsewhere, or the scanner must be
             built using “%array” instead (see H̲O̲W̲ T̲H̲E̲ I̲N̲P̲U̲T̲ I̲S̲
             M̲A̲T̲C̲H̲E̲D̲).

             Finally, note that EOF cannot be put back to attempt to
             mark the input stream with an end-of-file.

     input()
             Reads the next character from the input stream.  For exam‐
             ple, the following is one way to eat up C comments:

                   %%
                   "/*" {
                           int c;

                           for (;;) {
                                   while ((c = input()) != '*' && c != EOF)
                                           ; /* eat up text of comment */

                                   if (c == '*') {
                                           while ((c = input()) == '*')
                                                   ;
                                           if (c == '/')
                                                   break; /* found the end */
                                   }

                                   if (c == EOF) {
                                           errx(1, "EOF in comment");
                                           break;
                                   }
                           }
                   }

             (Note that if the scanner is compiled using C++, then
             𝗶𝗻𝗽𝘂𝘁() is instead referred to as 𝘆𝘆𝗶𝗻𝗽𝘂𝘁(), in order to
             avoid a name clash with the C++ stream by the name of
             input.)

     YY_FLUSH_BUFFER
             Flushes the scanner's internal buffer so that the next time
             the scanner attempts to match a token, it will first refill
             the buffer using YY_INPUT (see T̲H̲E̲ G̲E̲N̲E̲R̲A̲T̲E̲D̲ S̲C̲A̲N̲N̲E̲R̲,
             below).  This action is a special case of the more general
             𝘆𝘆_𝗳𝗹𝘂𝘀𝗵_𝗯𝘂𝗳𝗳𝗲𝗿() function, described below in the section
             M̲U̲L̲T̲I̲P̲L̲E̲ I̲N̲P̲U̲T̲ B̲U̲F̲F̲E̲R̲S̲.

     yyterminate()
             Can be used in lieu of a return statement in an action.  It
             terminates the scanner and returns a 0 to the scanner's
             caller, indicating "all done".  By default, 𝘆𝘆𝘁𝗲𝗿𝗺𝗶𝗻𝗮𝘁𝗲()
             is also called when an end-of-file is encountered.  It is a
             macro and may be redefined.

𝐓𝐇𝐄 𝐆𝐄𝐍𝐄𝐑𝐀𝐓𝐄𝐃 𝐒𝐂𝐀𝐍𝐍𝐄𝐑
     The output of 𝗳𝗹𝗲𝘅 is the file l̲e̲x̲.̲y̲y̲.̲c̲, which contains the scan‐
     ning routine 𝘆𝘆𝗹𝗲𝘅(), a number of tables used by it for matching
     tokens, and a number of auxiliary routines and macros.  By default,
     𝘆𝘆𝗹𝗲𝘅() is declared as follows:

           int yylex()
           {
               ... various definitions and the actions in here ...
           }

     (If the environment supports function prototypes, then it will be
     "int yylex(void)".)  This definition may be changed by defining the
     YY_DECL macro.  For example:

           #define YY_DECL float lexscan(a, b) float a, b;

     would give the scanning routine the name l̲e̲x̲s̲c̲a̲n̲, returning a
     float, and taking two floats as arguments.  Note that if arguments
     are given to the scanning routine using a K&R-style/non-prototyped
     function declaration, the definition must be terminated with a
     semi-colon (‘;’).

     Whenever 𝘆𝘆𝗹𝗲𝘅() is called, it scans tokens from the global input
     file y̲y̲i̲n̲ (which defaults to stdin).  It continues until it either
     reaches an end-of-file (at which point it returns the value 0) or
     one of its actions executes a r̲e̲t̲u̲r̲n̲ statement.

     If the scanner reaches an end-of-file, subsequent calls are unde‐
     fined unless either y̲y̲i̲n̲ is pointed at a new input file (in which
     case scanning continues from that file), or 𝘆𝘆𝗿𝗲𝘀𝘁𝗮𝗿𝘁() is called.
     𝘆𝘆𝗿𝗲𝘀𝘁𝗮𝗿𝘁() takes one argument, a F̲I̲L̲E̲ *̲ pointer (which can be nil,
     if YY_INPUT has been set up to scan from a source other than y̲y̲i̲n̲),
     and initializes y̲y̲i̲n̲ for scanning from that file.  Essentially
     there is no difference between just assigning y̲y̲i̲n̲ to a new input
     file or using 𝘆𝘆𝗿𝗲𝘀𝘁𝗮𝗿𝘁() to do so; the latter is available for
     compatibility with previous versions of 𝗳𝗹𝗲𝘅, and because it can be
     used to switch input files in the middle of scanning.  It can also
     be used to throw away the current input buffer, by calling it with
     an argument of y̲y̲i̲n̲; but better is to use YY_FLUSH_BUFFER (see
     above).  Note that 𝘆𝘆𝗿𝗲𝘀𝘁𝗮𝗿𝘁() does not reset the start condition
     to I̲N̲I̲T̲I̲A̲L̲ (see S̲T̲A̲R̲T̲ C̲O̲N̲D̲I̲T̲I̲O̲N̲S̲, below).

     If 𝘆𝘆𝗹𝗲𝘅() stops scanning due to executing a r̲e̲t̲u̲r̲n̲ statement in
     one of the actions, the scanner may then be called again and it
     will resume scanning where it left off.

     By default (and for purposes of efficiency), the scanner uses
     block-reads rather than simple getc(3) calls to read characters
     from y̲y̲i̲n̲.  The nature of how it gets its input can be controlled
     by defining the YY_INPUT macro.  YY_INPUT's calling sequence is
     "YY_INPUT(buf,result,max_size)".  Its action is to place up to
     max_size characters in the character array b̲u̲f̲ and return in the
     integer variable r̲e̲s̲u̲l̲t̲ either the number of characters read or the
     constant YY_NULL (0 on UNIX systems) to indicate EOF.  The default
     YY_INPUT reads from the global file-pointer "yyin".

     A sample definition of YY_INPUT (in the definitions section of the
     input file):

           %{
           #define YY_INPUT(buf,result,max_size) \
           { \
                   int c = getchar(); \
                   result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
           }
           %}

     This definition will change the input processing to occur one char‐
     acter at a time.

     When the scanner receives an end-of-file indication from YY_INPUT,
     it then checks the 𝘆𝘆𝘄𝗿𝗮𝗽() function.  If 𝘆𝘆𝘄𝗿𝗮𝗽() returns false
     (zero), then it is assumed that the function has gone ahead and set
     up y̲y̲i̲n̲ to point to another input file, and scanning continues.  If
     it returns true (non-zero), then the scanner terminates, returning
     0 to its caller.  Note that in either case, the start condition
     remains unchanged; it does not revert to I̲N̲I̲T̲I̲A̲L̲.

     If you do not supply your own version of 𝘆𝘆𝘄𝗿𝗮𝗽(), then you must
     either use “%option noyywrap” (in which case the scanner behaves as
     though 𝘆𝘆𝘄𝗿𝗮𝗽() returned 1), or you must link with -𝗹𝗳𝗹 to obtain
     the default version of the routine, which always returns 1.

     Three routines are available for scanning from in-memory buffers
     rather than files: 𝘆𝘆_𝘀𝗰𝗮𝗻_𝘀𝘁𝗿𝗶𝗻𝗴(), 𝘆𝘆_𝘀𝗰𝗮𝗻_𝗯𝘆𝘁𝗲𝘀(), and
     𝘆𝘆_𝘀𝗰𝗮𝗻_𝗯𝘂𝗳𝗳𝗲𝗿().  See the discussion of them below in the section
     M̲U̲L̲T̲I̲P̲L̲E̲ I̲N̲P̲U̲T̲ B̲U̲F̲F̲E̲R̲S̲.

     The scanner writes its E̲C̲H̲O̲ output to the y̲y̲o̲u̲t̲ global (default,
     stdout), which may be redefined by the user simply by assigning it
     to some other F̲I̲L̲E̲ pointer.

𝐒𝐓𝐀𝐑𝐓 𝐂𝐎𝐍𝐃𝐈𝐓𝐈𝐎𝐍𝐒
     𝗳𝗹𝗲𝘅 provides a mechanism for conditionally activating rules.  Any
     rule whose pattern is prefixed with "⟨sc⟩" will only be active when
     the scanner is in the start condition named "sc".  For example,

           <STRING>[^"]* { /* eat up the string body ... */
                   ...
           }

     will be active only when the scanner is in the "STRING" start con‐
     dition, and

           <INITIAL,STRING,QUOTE>\. { /* handle an escape ... */
                   ...
           }

     will be active only when the current start condition is either
     "INITIAL", "STRING", or "QUOTE".

     Start conditions are declared in the definitions (first) section of
     the input using unindented lines beginning with either ‘%s’ or ‘%x’
     followed by a list of names.  The former declares i̲n̲c̲l̲u̲s̲i̲v̲e̲ start
     conditions, the latter e̲x̲c̲l̲u̲s̲i̲v̲e̲ start conditions.  A start condi‐
     tion is activated using the B̲E̲G̲I̲N̲ action.  Until the next B̲E̲G̲I̲N̲
     action is executed, rules with the given start condition will be
     active and rules with other start conditions will be inactive.  If
     the start condition is inclusive, then rules with no start condi‐
     tions at all will also be active.  If it is exclusive, then only
     rules qualified with the start condition will be active.  A set of
     rules contingent on the same exclusive start condition describe a
     scanner which is independent of any of the other rules in the 𝗳𝗹𝗲𝘅
     input.  Because of this, exclusive start conditions make it easy to
     specify "mini-scanners" which scan portions of the input that are
     syntactically different from the rest (e.g., comments).

     If the distinction between inclusive and exclusive start conditions
     is still a little vague, here's a simple example illustrating the
     connection between the two.  The set of rules:

           %s example
           %%

           <example>foo   do_something();

           bar            something_else();

     is equivalent to

           %x example
           %%

           <example>foo   do_something();

           <INITIAL,example>bar    something_else();

     Without the ⟨INITIAL,example⟩ qualifier, the “bar” pattern in the
     second example wouldn't be active (i.e., couldn't match) when in
     start condition “example”.  If we just used ⟨example⟩ to qualify
     “bar”, though, then it would only be active in “example” and not in
     I̲N̲I̲T̲I̲A̲L̲, while in the first example it's active in both, because in
     the first example the “example” start condition is an inclusive
     (‘%s’) start condition.

     Also note that the special start-condition specifier ‘⟨*⟩’ matches
     every start condition.  Thus, the above example could also have
     been written:

           %x example
           %%

           <example>foo   do_something();

           <*>bar         something_else();

     The default rule (to E̲C̲H̲O̲ any unmatched character) remains active
     in start conditions.  It is equivalent to:

           <*>.|\n     ECHO;

     “BEGIN(0)” returns to the original state where only the rules with
     no start conditions are active.  This state can also be referred to
     as the start-condition I̲N̲I̲T̲I̲A̲L̲, so “BEGIN(INITIAL)” is equivalent
     to “BEGIN(0)”.  (The parentheses around the start condition name
     are not required but are considered good style.)

     B̲E̲G̲I̲N̲ actions can also be given as indented code at the beginning
     of the rules section.  For example, the following will cause the
     scanner to enter the "SPECIAL" start condition whenever 𝘆𝘆𝗹𝗲𝘅() is
     called and the global variable e̲n̲t̲e̲r̲_s̲p̲e̲c̲i̲a̲l̲ is true:

           int enter_special;

           %x SPECIAL
           %%
                   if (enter_special)
                           BEGIN(SPECIAL);

           <SPECIAL>blahblahblah
           ...more rules follow...

     To illustrate the uses of start conditions, here is a scanner which
     provides two different interpretations of a string like "123.456".
     By default it will treat it as three tokens: the integer "123", a
     dot (‘.’), and the integer "456".  But if the string is preceded
     earlier in the line by the string "expect-floats" it will treat it
     as a single token, the floating-point number 123.456:

           %{
           #include <math.h>
           %}
           %s expect

           %%
           expect-floats        BEGIN(expect);

           <expect>[0-9]+"."[0-9]+ {
                   printf("found a float, = %f\n",
                       atof(yytext));
           }
           <expect>\n {
                   /*
                    * That's the end of the line, so
                    * we need another "expect-number"
                    * before we'll recognize any more
                    * numbers.
                    */
                   BEGIN(INITIAL);
           }

           [0-9]+ {
                   printf("found an integer, = %d\n",
                       atoi(yytext));
           }

           "."     printf("found a dot\n");

     Here is a scanner which recognizes (and discards) C comments while
     maintaining a count of the current input line:

           %x comment
           %%
           int line_num = 1;

           "/*"                    BEGIN(comment);

           <comment>[^*\n]*        /* eat anything that's not a '*' */
           <comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
           <comment>\n             ++line_num;
           <comment>"*"+"/"        BEGIN(INITIAL);

     This scanner goes to a bit of trouble to match as much text as pos‐
     sible with each rule.  In general, when attempting to write a high-
     speed scanner try to match as much as possible in each rule, as
     it's a big win.

     Note that start-condition names are really integer values and can
     be stored as such.  Thus, the above could be extended in the fol‐
     lowing fashion:

           %x comment foo
           %%
           int line_num = 1;
           int comment_caller;

           "/*" {
                   comment_caller = INITIAL;
                   BEGIN(comment);
           }

           ...

           <foo>"/*" {
                   comment_caller = foo;
                   BEGIN(comment);
           }

           <comment>[^*\n]*        /* eat anything that's not a '*' */
           <comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
           <comment>\n             ++line_num;
           <comment>"*"+"/"        BEGIN(comment_caller);

     Furthermore, the current start condition can be accessed by using
     the integer-valued YY_START macro.  For example, the above assign‐
     ments to c̲o̲m̲m̲e̲n̲t̲_c̲a̲l̲l̲e̲r̲ could instead be written

           comment_caller = YY_START;

     Flex provides YYSTATE as an alias for YY_START (since that is
     what's used by AT&T UNIX 𝗹𝗲𝘅).

     Note that start conditions do not have their own name-space; %s's
     and %x's declare names in the same fashion as #define's.

     Finally, here's an example of how to match C-style quoted strings
     using exclusive start conditions, including expanded escape
     sequences (but not including checking for a string that's too
     long):

           %x str

           %%
           #define MAX_STR_CONST 1024
           char string_buf[MAX_STR_CONST];
           char *string_buf_ptr;

           \"      string_buf_ptr = string_buf; BEGIN(str);

           <str>\" { /* saw closing quote - all done */
                   BEGIN(INITIAL);
                   *string_buf_ptr = '\0';
                   /*
                    * return string constant token type and
                    * value to parser
                    */
           }

           <str>\n {
                   /* error - unterminated string constant */
                   /* generate error message */
           }

           <str>\\[0-7]{1,3} {
                   /* octal escape sequence */
                   int result;

                   (void) sscanf(yytext + 1, "%o", &result);

                   if (result > 0xff) {
                           /* error, constant is out-of-bounds */
                   } else
                           *string_buf_ptr++ = result;
           }

           <str>\\[0-9]+ {
                   /*
                    * generate error - bad escape sequence; something
                    * like '\48' or '\0777777'
                    */
           }

           <str>\\n  *string_buf_ptr++ = '\n';
           <str>\\t  *string_buf_ptr++ = '\t';
           <str>\\r  *string_buf_ptr++ = '\r';
           <str>\\b  *string_buf_ptr++ = '\b';
           <str>\\f  *string_buf_ptr++ = '\f';

           <str>\\(.|\n)  *string_buf_ptr++ = yytext[1];

           <str>[^\\\n\"]+ {
                   char *yptr = yytext;

                   while (*yptr)
                           *string_buf_ptr++ = *yptr++;
           }

     Often, such as in some of the examples above, a whole bunch of
     rules are all preceded by the same start condition(s).  𝗳𝗹𝗲𝘅 makes
     this a little easier and cleaner by introducing a notion of start
     condition s̲c̲o̲p̲e̲.  A start condition scope is begun with:

           <SCs>{

     where “SCs” is a list of one or more start conditions.  Inside the
     start condition scope, every rule automatically has the prefix
     ⟨SCs⟩ applied to it, until a ‘}’ which matches the initial ‘{’.
     So, for example,

           <ESC>{
               "\\n"   return '\n';
               "\\r"   return '\r';
               "\\f"   return '\f';
               "\\0"   return '\0';
           }

     is equivalent to:

           <ESC>"\\n"  return '\n';
           <ESC>"\\r"  return '\r';
           <ESC>"\\f"  return '\f';
           <ESC>"\\0"  return '\0';

     Start condition scopes may be nested.

     Three routines are available for manipulating stacks of start con‐
     ditions:

     void yy_push_state(int new_state)
             Pushes the current start condition onto the top of the
             start condition stack and switches to n̲e̲w̲_s̲t̲a̲t̲e̲ as though
             “BEGIN new_state” had been used (recall that start
             condition names are also integers).

     void yy_pop_state()
             Pops the top of the stack and switches to it via B̲E̲G̲I̲N̲.

     int yy_top_state()
             Returns the top of the stack without altering the stack's
             contents.

     The start condition stack grows dynamically and so has no built-in
     size limitation.  If memory is exhausted, program execution aborts.

     To use start condition stacks, scanners must include a “%option
     stack” directive (see O̲P̲T̲I̲O̲N̲S̲ below).

𝐌𝐔𝐋𝐓𝐈𝐏𝐋𝐄 𝐈𝐍𝐏𝐔𝐓 𝐁𝐔𝐅𝐅𝐄𝐑𝐒
     Some scanners (such as those which support "include" files) require
     reading from several input streams.  As 𝗳𝗹𝗲𝘅 scanners do a large
     amount of buffering, one cannot control where the next input will
     be read from by simply writing a YY_INPUT which is sensitive to the
     scanning context.  YY_INPUT is only called when the scanner reaches
     the end of its buffer, which may be a long time after scanning a
     statement such as an "include" which requires switching the input
     source.

     To negotiate these sorts of problems, 𝗳𝗹𝗲𝘅 provides a mechanism for
     creating and switching between multiple input buffers.  An input
     buffer is created by using:

           YY_BUFFER_STATE yy_create_buffer(FILE *file, int size)

     which takes a F̲I̲L̲E̲ pointer and a s̲i̲z̲e̲ and creates a buffer associ‐
     ated with the given file and large enough to hold s̲i̲z̲e̲ characters
     (when in doubt, use YY_BUF_SIZE for the size).  It returns a
     YY_BUFFER_STATE handle, which may then be passed to other routines
     (see below).  The YY_BUFFER_STATE type is a pointer to an opaque
     “struct yy_buffer_state” structure, so YY_BUFFER_STATE variables
     may be safely initialized to “((YY_BUFFER_STATE) 0)” if desired,
     and the opaque structure can also be referred to in order to cor‐
     rectly declare input buffers in source files other than that of
     scanners.  Note that the F̲I̲L̲E̲ pointer in the call to
     𝘆𝘆_𝗰𝗿𝗲𝗮𝘁𝗲_𝗯𝘂𝗳𝗳𝗲𝗿() is only used as the value of y̲y̲i̲n̲ seen by
     YY_INPUT; if YY_INPUT is redefined so that it no longer uses y̲y̲i̲n̲,
     then a nil F̲I̲L̲E̲ pointer can safely be passed to 𝘆𝘆_𝗰𝗿𝗲𝗮𝘁𝗲_𝗯𝘂𝗳𝗳𝗲𝗿().
     To select a particular buffer to scan:

           void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer)

     It switches the scanner's input buffer so subsequent tokens will
     come from n̲e̲w̲_b̲u̲f̲f̲e̲r̲.  Note that 𝘆𝘆_𝘀𝘄𝗶𝘁𝗰𝗵_𝘁𝗼_𝗯𝘂𝗳𝗳𝗲𝗿() may be used
     by 𝘆𝘆𝘄𝗿𝗮𝗽() to set things up for continued scanning, instead of
     opening a new file and pointing y̲y̲i̲n̲ at it.  Note also that switch‐
     ing input sources via either 𝘆𝘆_𝘀𝘄𝗶𝘁𝗰𝗵_𝘁𝗼_𝗯𝘂𝗳𝗳𝗲𝗿() or 𝘆𝘆𝘄𝗿𝗮𝗽() does
     not change the start condition.

           void yy_delete_buffer(YY_BUFFER_STATE buffer)

     is used to reclaim the storage associated with a buffer.  (b̲u̲f̲f̲e̲r̲
     can be nil, in which case the routine does nothing.)  To clear the
     current contents of a buffer:

           void yy_flush_buffer(YY_BUFFER_STATE buffer)

     This function discards the buffer's contents, so the next time the
     scanner attempts to match a token from the buffer, it will first
     fill the buffer anew using YY_INPUT.

     𝘆𝘆_𝗻𝗲𝘄_𝗯𝘂𝗳𝗳𝗲𝗿() is an alias for 𝘆𝘆_𝗰𝗿𝗲𝗮𝘁𝗲_𝗯𝘂𝗳𝗳𝗲𝗿(), provided for
     compatibility with the C++ use of n̲e̲w̲ and d̲e̲l̲e̲t̲e̲ for creating and
     destroying dynamic objects.

     Finally, the YY_CURRENT_BUFFER macro returns a YY_BUFFER_STATE han‐
     dle to the current buffer.

     Here is an example of using these features for writing a scanner
     which expands include files (the ⟨⟨EOF⟩⟩ feature is discussed
     below):

           /*
            * the "incl" state is used for picking up the name
            * of an include file
            */
           %x incl

           %{
           #define MAX_INCLUDE_DEPTH 10
           YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
           int include_stack_ptr = 0;
           %}

           %%
           include             BEGIN(incl);

           [a-z]+              ECHO;
           [^a-z\n]*\n?        ECHO;

           <incl>[ \t]*        /* eat the whitespace */
           <incl>[^ \t\n]+ {   /* got the include file name */
                   if (include_stack_ptr >= MAX_INCLUDE_DEPTH)
                           errx(1, "Includes nested too deeply");

                   include_stack[include_stack_ptr++] =
                       YY_CURRENT_BUFFER;

                   yyin = fopen(yytext, "r");

                   if (yyin == NULL)
                           err(1, NULL);

                   yy_switch_to_buffer(
                       yy_create_buffer(yyin, YY_BUF_SIZE));

                   BEGIN(INITIAL);
           }

           <<EOF>> {
                   if (--include_stack_ptr < 0)
                           yyterminate();
                   else {
                           yy_delete_buffer(YY_CURRENT_BUFFER);
                           yy_switch_to_buffer(
                               include_stack[include_stack_ptr]);
                  }
           }

     Three routines are available for setting up input buffers for scan‐
     ning in-memory strings instead of files.  All of them create a new
     input buffer for scanning the string, and return a corresponding
     YY_BUFFER_STATE handle (which should be deleted afterwards using
     𝘆𝘆_𝗱𝗲𝗹𝗲𝘁𝗲_𝗯𝘂𝗳𝗳𝗲𝗿()).  They also switch to the new buffer using
     𝘆𝘆_𝘀𝘄𝗶𝘁𝗰𝗵_𝘁𝗼_𝗯𝘂𝗳𝗳𝗲𝗿(), so the next call to 𝘆𝘆𝗹𝗲𝘅() will start scan‐
     ning the string.

     yy_scan_string(const char *str)
             Scans a NUL-terminated string.

     yy_scan_bytes(const char *bytes, int len)
             Scans l̲e̲n̲ bytes (including possibly NUL's) starting at
             location b̲y̲t̲e̲s̲.

     Note that both of these functions create and scan a copy of the
     string or bytes.  (This may be desirable, since 𝘆𝘆𝗹𝗲𝘅() modifies
     the contents of the buffer it is scanning.)  The copy can be
     avoided by using:

     yy_scan_buffer(char *base, yy_size_t size)
             Which scans the buffer starting at b̲a̲s̲e̲, consisting of s̲i̲z̲e̲
             bytes, the last two bytes of which must be
             YY_END_OF_BUFFER_CHAR (ASCII NUL).  These last two bytes
             are not scanned; thus, scanning consists of base[0] through
             base[size-2], inclusive.

             If b̲a̲s̲e̲ is not set up in this manner (i.e., forget the
             final two YY_END_OF_BUFFER_CHAR bytes), then
             𝘆𝘆_𝘀𝗰𝗮𝗻_𝗯𝘂𝗳𝗳𝗲𝗿() returns a nil pointer instead of creating
             a new input buffer.

             The type y̲y̲_s̲i̲z̲e̲_t̲ is an integral type which can be cast to
             an integer expression reflecting the size of the buffer.

𝐄𝐍𝐃-𝐎𝐅-𝐅𝐈𝐋𝐄 𝐑𝐔𝐋𝐄𝐒
     The special rule "⟨⟨EOF⟩⟩" indicates actions which are to be taken
     when an end-of-file is encountered and 𝘆𝘆𝘄𝗿𝗮𝗽() returns non-zero
     (i.e., indicates no further files to process).  The action must
     finish by doing one of four things:

     -   Assigning y̲y̲i̲n̲ to a new input file (in previous versions of
         𝗳𝗹𝗲𝘅, after doing the assignment, it was necessary to call the
         special action YY_NEW_FILE; this is no longer necessary).

     -   Executing a r̲e̲t̲u̲r̲n̲ statement.

     -   Executing the special 𝘆𝘆𝘁𝗲𝗿𝗺𝗶𝗻𝗮𝘁𝗲() action.

     -   Switching to a new buffer using 𝘆𝘆_𝘀𝘄𝗶𝘁𝗰𝗵_𝘁𝗼_𝗯𝘂𝗳𝗳𝗲𝗿() as shown
         in the example above.

     ⟨⟨EOF⟩⟩ rules may not be used with other patterns; they may only be
     qualified with a list of start conditions.  If an unqualified
     ⟨⟨EOF⟩⟩ rule is given, it applies to all start conditions which do
     not already have ⟨⟨EOF⟩⟩ actions.  To specify an ⟨⟨EOF⟩⟩ rule for
     only the initial start condition, use

           <INITIAL><<EOF>>

     These rules are useful for catching things like unclosed comments.
     An example:

           %x quote
           %%

           ...other rules for dealing with quotes...

           <quote><<EOF>> {
                    error("unterminated quote");
                    yyterminate();
           }
           <<EOF>> {
                    if (*++filelist)
                            yyin = fopen(*filelist, "r");
                    else
                            yyterminate();
           }

𝐌𝐈𝐒𝐂𝐄𝐋𝐋𝐀𝐍𝐄𝐎𝐔𝐒 𝐌𝐀𝐂𝐑𝐎𝐒
     The macro YY_USER_ACTION can be defined to provide an action which
     is always executed prior to the matched rule's action.  For exam‐
     ple, it could be #define'd to call a routine to convert yytext to
     lower-case.  When YY_USER_ACTION is invoked, the variable y̲y̲_a̲c̲t̲
     gives the number of the matched rule (rules are numbered starting
     with 1).  For example, to profile how often each rule is matched,
     the following would do the trick:

           #define YY_USER_ACTION ++ctr[yy_act]

     where c̲t̲r̲ is an array to hold the counts for the different rules.
     Note that the macro YY_NUM_RULES gives the total number of rules
     (including the default rule, even if -𝘀 is used), so a correct dec‐
     laration for c̲t̲r̲ is:

           int ctr[YY_NUM_RULES];

     The macro YY_USER_INIT may be defined to provide an action which is
     always executed before the first scan (and before the scanner's
     internal initializations are done).  For example, it could be used
     to call a routine to read in a data table or open a logging file.

     The macro yy_set_interactive(is_interactive) can be used to control
     whether the current buffer is considered i̲n̲t̲e̲r̲a̲c̲t̲i̲v̲e̲.  An interac‐
     tive buffer is processed more slowly, but must be used when the
     scanner's input source is indeed interactive to avoid problems due
     to waiting to fill buffers (see the discussion of the -𝐈 flag
     below).  A non-zero value in the macro invocation marks the buffer
     as interactive, a zero value as non-interactive.  Note that use of
     this macro overrides “%option always-interactive” or “%option
     never-interactive” (see O̲P̲T̲I̲O̲N̲S̲ below).  𝘆𝘆_𝘀𝗲𝘁_𝗶𝗻𝘁𝗲𝗿𝗮𝗰𝘁𝗶𝘃𝗲() must
     be invoked prior to beginning to scan the buffer that is (or is
     not) to be considered interactive.

     The macro yy_set_bol(at_bol) can be used to control whether the
     current buffer's scanning context for the next token match is done
     as though at the beginning of a line.  A non-zero macro argument
     makes rules anchored with ‘^’ active, while a zero argument makes
     ‘^’ rules inactive.

     The macro YY_AT_BOL returns true if the next token scanned from the
     current buffer will have ‘^’ rules active, false otherwise.

     In the generated scanner, the actions are all gathered in one large
     switch statement and separated using YY_BREAK, which may be rede‐
     fined.  By default, it is simply a "break", to separate each rule's
     action from the following rules.  Redefining YY_BREAK allows, for
     example, C++ users to “#define YY_BREAK” to do nothing (while being
     very careful that every rule ends with a "break" or a "return"!)
     to avoid suffering from unreachable statement warnings where
     because a rule's action ends with “return”, the YY_BREAK is inac‐
     cessible.

𝐕𝐀𝐋𝐔𝐄𝐒 𝐀𝐕𝐀𝐈𝐋𝐀𝐁𝐋𝐄 𝐓𝐎 𝐓𝐇𝐄 𝐔𝐒𝐄𝐑
     This section summarizes the various values available to the user in
     the rule actions.

     char *yytext
             Holds the text of the current token.  It may be modified
             but not lengthened (characters cannot be appended to the
             end).

             If the special directive “%array” appears in the first sec‐
             tion of the scanner description, then y̲y̲t̲e̲x̲t̲ is instead
             declared “char yytext[YYLMAX]”, where YYLMAX is a macro
             definition that can be redefined in the first section to
             change the default value (generally 8KB).  Using “%array”
             results in somewhat slower scanners, but the value of
             y̲y̲t̲e̲x̲t̲ becomes immune to calls to 𝗶𝗻𝗽𝘂𝘁() and 𝘂𝗻𝗽𝘂𝘁(),
             which potentially destroy its value when y̲y̲t̲e̲x̲t̲ is a char‐
             acter pointer.  The opposite of “%array” is “%pointer”,
             which is the default.

             “%array” cannot be used when generating C++ scanner classes
             (the -+ flag).

     int yyleng
             Holds the length of the current token.

     FILE *yyin
             Is the file which by default 𝗳𝗹𝗲𝘅 reads from.  It may be
             redefined, but doing so only makes sense before scanning
             begins or after an EOF has been encountered.  Changing it
             in the midst of scanning will have unexpected results since
             𝗳𝗹𝗲𝘅 buffers its input; use 𝘆𝘆𝗿𝗲𝘀𝘁𝗮𝗿𝘁() instead.  Once
             scanning terminates because an end-of-file has been seen,
             y̲y̲i̲n̲ can be assigned as the new input file and the scanner
             can be called again to continue scanning.

     void yyrestart(FILE *new_file)
             May be called to point y̲y̲i̲n̲ at the new input file.  The
             switch-over to the new file is immediate (any previously
             buffered-up input is lost).  Note that calling 𝘆𝘆𝗿𝗲𝘀𝘁𝗮𝗿𝘁()
             with y̲y̲i̲n̲ as an argument thus throws away the current input
             buffer and continues scanning the same input file.

     FILE *yyout
             Is the file to which E̲C̲H̲O̲ actions are done.  It can be
             reassigned by the user.

     YY_CURRENT_BUFFER
             Returns a YY_BUFFER_STATE handle to the current buffer.

     YY_START
             Returns an integer value corresponding to the current start
             condition.  This value can subsequently be used with B̲E̲G̲I̲N̲
             to return to that start condition.

𝐈𝐍𝐓𝐄𝐑𝐅𝐀𝐂𝐈𝐍𝐆 𝐖𝐈𝐓𝐇 𝐘𝐀𝐂𝐂
     One of the main uses of 𝗳𝗹𝗲𝘅 is as a companion to the yacc(1)
     parser-generator.  yacc parsers expect to call a routine named
     𝘆𝘆𝗹𝗲𝘅() to find the next input token.  The routine is supposed to
     return the type of the next token as well as putting any associated
     value in the global y̲y̲l̲v̲a̲l̲, which is defined externally, and can be
     a union or any other complex data structure.  To use 𝗳𝗹𝗲𝘅 with
     yacc, one specifies the -𝗱 option to yacc to instruct it to gener‐
     ate the file y̲.̲t̲a̲b̲.̲h̲ containing definitions of all the “%tokens”
     appearing in the yacc input.  This file is then included in the
     𝗳𝗹𝗲𝘅 scanner.  For example, if one of the tokens is "TOK_NUMBER",
     part of the scanner might look like:

           %{
           #include "y.tab.h"
           %}

           %%

           [0-9]+        yylval = atoi(yytext); return TOK_NUMBER;

𝐎𝐏𝐓𝐈𝐎𝐍𝐒
     𝗳𝗹𝗲𝘅 has the following options:

     -𝟳      Instructs 𝗳𝗹𝗲𝘅 to generate a 7-bit scanner, i.e., one which
             can only recognize 7-bit characters in its input.  The
             advantage of using -𝟳 is that the scanner's tables can be
             up to half the size of those generated using the -𝟴 option
             (see below).  The disadvantage is that such scanners often
             hang or crash if their input contains an 8-bit character.

             Note, however, that unless generating a scanner using the
             -𝐂𝗳 or -𝐂𝐅 table compression options, use of -𝟳 will save
             only a small amount of table space, and make the scanner
             considerably less portable.  𝗳𝗹𝗲𝘅's default behavior is to
             generate an 8-bit scanner unless -𝐂𝗳 or -𝐂𝐅 is specified,
             in which case 𝗳𝗹𝗲𝘅 defaults to generating 7-bit scanners
             unless it was configured to generate 8-bit scanners (as
             will often be the case with non-USA sites).  It is possible
             tell whether 𝗳𝗹𝗲𝘅 generated a 7-bit or an 8-bit scanner by
             inspecting the flag summary in the -𝘃 output as described
             below.

             Note that if -𝐂𝗳𝗲 or -𝐂𝐅𝗲 are used (the table compression
             options, but also using equivalence classes as discussed
             below), 𝗳𝗹𝗲𝘅 still defaults to generating an 8-bit scanner,
             since usually with these compression options full 8-bit
             tables are not much more expensive than 7-bit tables.

     -𝟴      Instructs 𝗳𝗹𝗲𝘅 to generate an 8-bit scanner, i.e., one
             which can recognize 8-bit characters.  This flag is only
             needed for scanners generated using -𝐂𝗳 or -𝐂𝐅, as other‐
             wise 𝗳𝗹𝗲𝘅 defaults to generating an 8-bit scanner anyway.

             See the discussion of -𝟳 above for 𝗳𝗹𝗲𝘅's default behavior
             and the tradeoffs between 7-bit and 8-bit scanners.

     -𝐁      Instructs 𝗳𝗹𝗲𝘅 to generate a b̲a̲t̲c̲h̲ scanner, the opposite of
             i̲n̲t̲e̲r̲a̲c̲t̲i̲v̲e̲ scanners generated by -𝐈 (see below).  In gen‐
             eral, -𝐁 is used when the scanner will never be used inter‐
             actively, and you want to squeeze a little more performance
             out of it.  If the aim is instead to squeeze out a lot more
             performance, use the -𝐂𝗳 or -𝐂𝐅 options (discussed below),
             which turn on -𝐁 automatically anyway.

     -𝗯      Generate backing-up information to l̲e̲x̲.̲b̲a̲c̲k̲u̲p̲.  This is a
             list of scanner states which require backing up and the
             input characters on which they do so.  By adding rules one
             can remove backing-up states.  If all backing-up states are
             eliminated and -𝐂𝗳 or -𝐂𝐅 is used, the generated scanner
             will run faster (see the -𝗽 flag).  Only users who wish to
             squeeze every last cycle out of their scanners need worry
             about this option.  (See the section on P̲E̲R̲F̲O̲R̲M̲A̲N̲C̲E̲
             C̲O̲N̲S̲I̲D̲E̲R̲A̲T̲I̲O̲N̲S̲ below.)

     -𝐂[𝗮𝗲𝐅𝗳𝗺𝗿]
             Controls the degree of table compression and, more gener‐
             ally, trade-offs between small scanners and fast scanners.

             -𝐂𝗮     Instructs 𝗳𝗹𝗲𝘅 to trade off larger tables in the
                     generated scanner for faster performance because
                     the elements of the tables are better aligned for
                     memory access and computation.  On some RISC archi‐
                     tectures, fetching and manipulating longwords is
                     more efficient than with smaller-sized units such
                     as shortwords.  This option can double the size of
                     the tables used by the scanner.

             -𝐂𝗲     Directs 𝗳𝗹𝗲𝘅 to construct e̲q̲u̲i̲v̲a̲l̲e̲n̲c̲e̲ c̲l̲a̲s̲s̲e̲s̲,
                     i.e., sets of characters which have identical lexi‐
                     cal properties (for example, if the only appearance
                     of digits in the 𝗳𝗹𝗲𝘅 input is in the character
                     class "[0-9]" then the digits ‘0’, ‘1’, ‘...’, ‘9’
                     will all be put in the same equivalence class).
                     Equivalence classes usually give dramatic reduc‐
                     tions in the final table/object file sizes
                     (typically a factor of 2-5) and are pretty cheap
                     performance-wise (one array look-up per character
                     scanned).

             -𝐂𝐅     Specifies that the alternate fast scanner represen‐
                     tation (described below under the -𝐅 option) should
                     be used.  This option cannot be used with -+.

             -𝐂𝗳     Specifies that the f̲u̲l̲l̲ scanner tables should be
                     generated - 𝗳𝗹𝗲𝘅 should not compress the tables by
                     taking advantage of similar transition functions
                     for different states.

             -𝐂𝗺     Directs 𝗳𝗹𝗲𝘅 to construct m̲e̲t̲a̲-̲e̲q̲u̲i̲v̲a̲l̲e̲n̲c̲e̲ c̲l̲a̲s̲s̲e̲s̲,
                     which are sets of equivalence classes (or charac‐
                     ters, if equivalence classes are not being used)
                     that are commonly used together.  Meta-equivalence
                     classes are often a big win when using compressed
                     tables, but they have a moderate performance impact
                     (one or two "if" tests and one array look-up per
                     character scanned).

             -𝐂𝗿     Causes the generated scanner to b̲y̲p̲a̲s̲s̲ use of the
                     standard I/O library (stdio) for input.  Instead of
                     calling fread(3) or getc(3), the scanner will use
                     the read(2) system call, resulting in a performance
                     gain which varies from system to system, but in
                     general is probably negligible unless -𝐂𝗳 or -𝐂𝐅
                     are being used.  Using -𝐂𝗿 can cause strange behav‐
                     ior if, for example, reading from y̲y̲i̲n̲ using stdio
                     prior to calling the scanner (because the scanner
                     will miss whatever text previous reads left in the
                     stdio input buffer).

                     -𝐂𝗿 has no effect if YY_INPUT is defined (see T̲H̲E̲
                     G̲E̲N̲E̲R̲A̲T̲E̲D̲ S̲C̲A̲N̲N̲E̲R̲ above).

             A lone -𝐂 specifies that the scanner tables should be com‐
             pressed but neither equivalence classes nor meta-equiva‐
             lence classes should be used.

             The options -𝐂𝗳 or -𝐂𝐅 and -𝐂𝗺 do not make sense together -
             there is no opportunity for meta-equivalence classes if the
             table is not being compressed.  Otherwise the options may
             be freely mixed, and are cumulative.

             The default setting is -𝐂𝗲𝗺 which specifies that 𝗳𝗹𝗲𝘅
             should generate equivalence classes and meta-equivalence
             classes.  This setting provides the highest degree of table
             compression.  It is possible to trade off faster-executing
             scanners at the cost of larger tables with the following
             generally being true:

                   slowest & smallest
                         -Cem
                         -Cm
                         -Ce
                         -C
                         -C{f,F}e
                         -C{f,F}
                         -C{f,F}a
                   fastest & largest

             Note that scanners with the smallest tables are usually
             generated and compiled the quickest, so during development
             the default is usually best, maximal compression.

             -𝐂𝗳𝗲 is often a good compromise between speed and size for
             production scanners.

     -𝗱      Makes the generated scanner run in debug mode.  Whenever a
             pattern is recognized and the global y̲y̲_f̲l̲e̲x̲_d̲e̲b̲u̲g̲ is non-
             zero (which is the default), the scanner will write to
             stderr a line of the form:

                   --accepting rule at line 53 ("the matched text")

             The line number refers to the location of the rule in the
             file defining the scanner (i.e., the file that was fed to
             𝗳𝗹𝗲𝘅).  Messages are also generated when the scanner backs
             up, accepts the default rule, reaches the end of its input
             buffer (or encounters a NUL; at this point, the two look
             the same as far as the scanner's concerned), or reaches an
             end-of-file.

     -𝐅      Specifies that the fast scanner table representation should
             be used (and stdio bypassed).  This representation is about
             as fast as the full table representation (-𝗳), and for some
             sets of patterns will be considerably smaller (and for
             others, larger).  In general, if the pattern set contains
             both "keywords" and a catch-all, "identifier" rule, such as
             in the set:

                   "case"    return TOK_CASE;
                   "switch"  return TOK_SWITCH;
                   ...
                   "default" return TOK_DEFAULT;
                   [a-z]+    return TOK_ID;

             then it's better to use the full table representation.  If
             only the "identifier" rule is present and a hash table or
             some such is used to detect the keywords, it's better to
             use -𝐅.

             This option is equivalent to -𝐂𝐅𝗿 (see above).  It cannot
             be used with -+.

     -𝗳      Specifies f̲a̲s̲t̲ s̲c̲a̲n̲n̲e̲r̲.  No table compression is done and
             stdio is bypassed.  The result is large but fast.  This
             option is equivalent to -𝐂𝗳𝗿 (see above).

     -𝗵      Generates a help summary of 𝗳𝗹𝗲𝘅's options to stdout and
             then exits.  -? and --𝗵𝗲𝗹𝗽 are synonyms for -𝗵.

     -𝐈      Instructs 𝗳𝗹𝗲𝘅 to generate an i̲n̲t̲e̲r̲a̲c̲t̲i̲v̲e̲ scanner.  An
             interactive scanner is one that only looks ahead to decide
             what token has been matched if it absolutely must.  It
             turns out that always looking one extra character ahead,
             even if the scanner has already seen enough text to disam‐
             biguate the current token, is a bit faster than only look‐
             ing ahead when necessary.  But scanners that always look
             ahead give dreadful interactive performance; for example,
             when a user types a newline, it is not recognized as a new‐
             line token until they enter a̲n̲o̲t̲h̲e̲r̲ token, which often
             means typing in another whole line.

             𝗳𝗹𝗲𝘅 scanners default to i̲n̲t̲e̲r̲a̲c̲t̲i̲v̲e̲ unless -𝐂𝗳 or -𝐂𝐅 ta‐
             ble-compression options are specified (see above).  That's
             because if high-performance is most important, one of these
             options should be used, so if they weren't, 𝗳𝗹𝗲𝘅 assumes it
             is preferable to trade off a bit of run-time performance
             for intuitive interactive behavior.  Note also that -𝐈 can‐
             not be used in conjunction with -𝐂𝗳 or -𝐂𝐅.  Thus, this
             option is not really needed; it is on by default for all
             those cases in which it is allowed.

             A scanner can be forced to not be interactive by using -𝐁
             (see above).

     -𝗶      Instructs 𝗳𝗹𝗲𝘅 to generate a case-insensitive scanner.  The
             case of letters given in the 𝗳𝗹𝗲𝘅 input patterns will be
             ignored, and tokens in the input will be matched regardless
             of case.  The matched text given in y̲y̲t̲e̲x̲t̲ will have the
             preserved case (i.e., it will not be folded).

     -𝐋      Instructs 𝗳𝗹𝗲𝘅 not to generate “#line” directives.  Without
             this option, 𝗳𝗹𝗲𝘅 peppers the generated scanner with #line
             directives so error messages in the actions will be cor‐
             rectly located with respect to either the original 𝗳𝗹𝗲𝘅
             input file (if the errors are due to code in the input
             file), or l̲e̲x̲.̲y̲y̲.̲c̲ (if the errors are 𝗳𝗹𝗲𝘅's fault - these
             sorts of errors should be reported to the email address
             given below).

     -𝗹      Turns on maximum compatibility with the original AT&T UNIX
             𝗹𝗲𝘅 implementation.  Note that this does not mean full com‐
             patibility.  Use of this option costs a considerable amount
             of performance, and it cannot be used with the -+, -𝗳, -𝐅,
             -𝐂𝗳, or -𝐂𝐅 options.  For details on the compatibilities it
             provides, see the section I̲N̲C̲O̲M̲P̲A̲T̲I̲B̲I̲L̲I̲T̲I̲E̲S̲ W̲I̲T̲H̲ L̲E̲X̲ A̲N̲D̲
             P̲O̲S̲I̲X̲ below.  This option also results in the name
             YY_FLEX_LEX_COMPAT being #define'd in the generated scan‐
             ner.

     -𝗻      Another do-nothing, deprecated option included only for
             POSIX compliance.

     -𝗼o̲u̲t̲p̲u̲t̲
             Directs 𝗳𝗹𝗲𝘅 to write the scanner to the file o̲u̲t̲p̲u̲t̲
             instead of l̲e̲x̲.̲y̲y̲.̲c̲.  If -𝗼 is combined with the -𝘁 option,
             then the scanner is written to stdout but its “#line”
             directives (see the -𝐋 option above) refer to the file
             o̲u̲t̲p̲u̲t̲.

     -𝐏p̲r̲e̲f̲i̲x̲
             Changes the default "yy" prefix used by 𝗳𝗹𝗲𝘅 for all glob‐
             ally visible variable and function names to instead be
             p̲r̲e̲f̲i̲x̲.  For example, -𝐏f̲o̲o̲ changes the name of y̲y̲t̲e̲x̲t̲ to
             f̲o̲o̲t̲e̲x̲t̲.  It also changes the name of the default output
             file from l̲e̲x̲.̲y̲y̲.̲c̲ to l̲e̲x̲.̲f̲o̲o̲.̲c̲.  Here are all of the names
             affected:

                   yy_create_buffer
                   yy_delete_buffer
                   yy_flex_debug
                   yy_init_buffer
                   yy_flush_buffer
                   yy_load_buffer_state
                   yy_switch_to_buffer
                   yyin
                   yyleng
                   yylex
                   yylineno
                   yyout
                   yyrestart
                   yytext
                   yywrap

             (If using a C++ scanner, then only y̲y̲w̲r̲a̲p̲ and y̲y̲F̲l̲e̲x̲L̲e̲x̲e̲r̲
             are affected.)  Within the scanner itself, it is still pos‐
             sible to refer to the global variables and functions using
             either version of their name; but externally, they have the
             modified name.

             This option allows multiple 𝗳𝗹𝗲𝘅 programs to be easily
             linked together into the same executable.  Note, though,
             that using this option also renames 𝘆𝘆𝘄𝗿𝗮𝗽(), so now either
             an (appropriately named) version of the routine for the
             scanner must be supplied, or “%option noyywrap” must be
             used, as linking with -𝗹𝗳𝗹 no longer provides one by
             default.

     -𝗽      Generates a performance report to stderr.  The report con‐
             sists of comments regarding features of the 𝗳𝗹𝗲𝘅 input file
             which will cause a serious loss of performance in the
             resulting scanner.  If the flag is specified twice, com‐
             ments regarding features that lead to minor performance
             losses will also be reported>

             Note that the use of R̲E̲J̲E̲C̲T̲, “%option yylineno”, and vari‐
             able trailing context (see the B̲U̲G̲S̲ section below) entails
             a substantial performance penalty; use of 𝘆𝘆𝗺𝗼𝗿𝗲(), the ‘^’
             operator, and the -𝐈 flag entail minor performance penal‐
             ties.

     -𝐒s̲k̲e̲l̲e̲t̲o̲n̲
             Overrides the default skeleton file from which 𝗳𝗹𝗲𝘅 con‐
             structs its scanners.  This option is needed only for 𝗳𝗹𝗲𝘅
             maintenance or development.

     -𝘀      Causes the default rule (that unmatched scanner input is
             echoed to stdout) to be suppressed.  If the scanner encoun‐
             ters input that does not match any of its rules, it aborts
             with an error.  This option is useful for finding holes in
             a scanner's rule set.

     -𝐓      Makes 𝗳𝗹𝗲𝘅 run in t̲r̲a̲c̲e̲ mode.  It will generate a lot of
             messages to stderr concerning the form of the input and the
             resultant non-deterministic and deterministic finite autom‐
             ata.  This option is mostly for use in maintaining 𝗳𝗹𝗲𝘅.

     -𝘁      Instructs 𝗳𝗹𝗲𝘅 to write the scanner it generates to stan‐
             dard output instead of l̲e̲x̲.̲y̲y̲.̲c̲.

     -𝐕      Prints the version number to stdout and exits.  --𝘃𝗲𝗿𝘀𝗶𝗼𝗻
             is a synonym for -𝐕.

     -𝘃      Specifies that 𝗳𝗹𝗲𝘅 should write to stderr a summary of
             statistics regarding the scanner it generates.  Most of the
             statistics are meaningless to the casual 𝗳𝗹𝗲𝘅 user, but the
             first line identifies the version of 𝗳𝗹𝗲𝘅 (same as reported
             by -𝐕), and the next line the flags used when generating
             the scanner, including those that are on by default.

     -𝘄      Suppresses warning messages.

     -+      Specifies that 𝗳𝗹𝗲𝘅 should generate a C++ scanner class.
             See the section on G̲E̲N̲E̲R̲A̲T̲I̲N̲G̲ C̲+̲+̲ S̲C̲A̲N̲N̲E̲R̲S̲ below for
             details.

     𝗳𝗹𝗲𝘅 also provides a mechanism for controlling options within the
     scanner specification itself, rather than from the 𝗳𝗹𝗲𝘅 command
     line.  This is done by including “%option” directives in the first
     section of the scanner specification.  Multiple options can be
     specified with a single “%option” directive, and multiple direc‐
     tives in the first section of the 𝗳𝗹𝗲𝘅 input file.

     Most options are given simply as names, optionally preceded by the
     word "no" (with no intervening whitespace) to negate their meaning.
     A number are equivalent to 𝗳𝗹𝗲𝘅 flags or their negation:

           7bit            -7 option
           8bit            -8 option
           align           -Ca option
           backup          -b option
           batch           -B option
           c++             -+ option

           caseful or
           case-sensitive  opposite of -i (default)

           case-insensitive or
           caseless        -i option

           debug           -d option
           default         opposite of -s option
           ecs             -Ce option
           fast            -F option
           full            -f option
           interactive     -I option
           lex-compat      -l option
           meta-ecs        -Cm option
           perf-report     -p option
           read            -Cr option
           stdout          -t option
           verbose         -v option
           warn            opposite of -w option
                           (use "%option nowarn" for -w)

           array           equivalent to "%array"
           pointer         equivalent to "%pointer" (default)

     Some %option's provide features otherwise not available:

     always-interactive
             Instructs 𝗳𝗹𝗲𝘅 to generate a scanner which always considers
             its input "interactive".  Normally, on each new input file
             the scanner calls 𝗶𝘀𝗮𝘁𝘁𝘆() in an attempt to determine
             whether the scanner's input source is interactive and thus
             should be read a character at a time.  When this option is
             used, however, no such call is made.

     main    Directs 𝗳𝗹𝗲𝘅 to provide a default 𝗺𝗮𝗶𝗻() program for the
             scanner, which simply calls 𝘆𝘆𝗹𝗲𝘅().  This option implies
             “noyywrap” (see below).

     never-interactive
             Instructs 𝗳𝗹𝗲𝘅 to generate a scanner which never considers
             its input "interactive" (again, no call made to 𝗶𝘀𝗮𝘁𝘁𝘆()).
             This is the opposite of “always-interactive”.

     stack   Enables the use of start condition stacks (see S̲T̲A̲R̲T̲
             C̲O̲N̲D̲I̲T̲I̲O̲N̲S̲ above).

     stdinit
             If set (i.e., “%option stdinit”), initializes y̲y̲i̲n̲ and
             y̲y̲o̲u̲t̲ to stdin and stdout, instead of the default of “nil”.
             Some existing 𝗹𝗲𝘅 programs depend on this behavior, even
             though it is not compliant with ANSI C, which does not
             require stdin and stdout to be compile-time constant.

     yylineno
             Directs 𝗳𝗹𝗲𝘅 to generate a scanner that maintains the num‐
             ber of the current line read from its input in the global
             variable y̲y̲l̲i̲n̲e̲n̲o̲.  This option is implied by “%option
             lex-compat”.

     yywrap  If unset (i.e., “%option noyywrap”), makes the scanner not
             call 𝘆𝘆𝘄𝗿𝗮𝗽() upon an end-of-file, but simply assume that
             there are no more files to scan (until the user points y̲y̲i̲n̲
             at a new file and calls 𝘆𝘆𝗹𝗲𝘅() again).

     𝗳𝗹𝗲𝘅 scans rule actions to determine whether the R̲E̲J̲E̲C̲T̲ or 𝘆𝘆𝗺𝗼𝗿𝗲()
     features are being used.  The “reject” and “yymore” options are
     available to override its decision as to whether to use the
     options, either by setting them (e.g., “%option reject”) to indi‐
     cate the feature is indeed used, or unsetting them to indicate it
     actually is not used (e.g., “%option noyymore”).

     Three options take string-delimited values, offset with ‘=’:

           %option outfile="ABC"

     is equivalent to -𝗼A̲B̲C̲, and

           %option prefix="XYZ"

     is equivalent to -𝐏X̲Y̲Z̲.  Finally,

           %option yyclass="foo"

     only applies when generating a C++ scanner (-+ option).  It informs
     𝗳𝗹𝗲𝘅 that “foo” has been derived as a subclass of yyFlexLexer, so
     𝗳𝗹𝗲𝘅 will place actions in the member function “foo::yylex()”
     instead of “yyFlexLexer::yylex()”.  It also generates a
     “yyFlexLexer::yylex()” member function that emits a run-time error
     (by invoking “yyFlexLexer::LexerError()”) if called.  See
     G̲E̲N̲E̲R̲A̲T̲I̲N̲G̲ C̲+̲+̲ S̲C̲A̲N̲N̲E̲R̲S̲, below, for additional information.

     A number of options are available for lint purists who want to sup‐
     press the appearance of unneeded routines in the generated scanner.
     Each of the following, if unset (e.g., “%option nounput”), results
     in the corresponding routine not appearing in the generated scan‐
     ner:

           input, unput
           yy_push_state, yy_pop_state, yy_top_state
           yy_scan_buffer, yy_scan_bytes, yy_scan_string

     (though 𝘆𝘆_𝗽𝘂𝘀𝗵_𝘀𝘁𝗮𝘁𝗲() and friends won't appear anyway unless
     “%option stack” is being used).

𝐏𝐄𝐑𝐅𝐎𝐑𝐌𝐀𝐍𝐂𝐄 𝐂𝐎𝐍𝐒𝐈𝐃𝐄𝐑𝐀𝐓𝐈𝐎𝐍𝐒
     The main design goal of 𝗳𝗹𝗲𝘅 is that it generate high-performance
     scanners.  It has been optimized for dealing well with large sets
     of rules.  Aside from the effects on scanner speed of the table
     compression -𝐂 options outlined above, there are a number of
     options/actions which degrade performance.  These are, from most
     expensive to least:

           REJECT
           %option yylineno
           arbitrary trailing context

           pattern sets that require backing up
           %array
           %option interactive
           %option always-interactive

           '^' beginning-of-line operator
           yymore()

     with the first three all being quite expensive and the last two
     being quite cheap.  Note also that 𝘂𝗻𝗽𝘂𝘁() is implemented as a rou‐
     tine call that potentially does quite a bit of work, while 𝘆𝘆𝗹𝗲𝘀𝘀()
     is a quite-cheap macro; so if just putting back some excess text,
     use 𝘆𝘆𝗹𝗲𝘀𝘀().

     R̲E̲J̲E̲C̲T̲ should be avoided at all costs when performance is impor‐
     tant.  It is a particularly expensive option.

     Getting rid of backing up is messy and often may be an enormous
     amount of work for a complicated scanner.  In principal, one begins
     by using the -𝗯 flag to generate a l̲e̲x̲.̲b̲a̲c̲k̲u̲p̲ file.  For example,
     on the input

           %%
           foo        return TOK_KEYWORD;
           foobar     return TOK_KEYWORD;

     the file looks like:

           State #6 is non-accepting -
            associated rule line numbers:
                  2       3
            out-transitions: [ o ]
            jam-transitions: EOF [ \001-n  p-\177 ]

           State #8 is non-accepting -
            associated rule line numbers:
                  3
            out-transitions: [ a ]
            jam-transitions: EOF [ \001-`  b-\177 ]

           State #9 is non-accepting -
            associated rule line numbers:
                  3
            out-transitions: [ r ]
            jam-transitions: EOF [ \001-q  s-\177 ]

           Compressed tables always back up.

     The first few lines tell us that there's a scanner state in which
     it can make a transition on an ‘o’ but not on any other character,
     and that in that state the currently scanned text does not match
     any rule.  The state occurs when trying to match the rules found at
     lines 2 and 3 in the input file.  If the scanner is in that state
     and then reads something other than an ‘o’, it will have to back up
     to find a rule which is matched.  With a bit of headscratching one
     can see that this must be the state it's in when it has seen ‘fo’.
     When this has happened, if anything other than another ‘o’ is seen,
     the scanner will have to back up to simply match the ‘f’ (by the
     default rule).

     The comment regarding State #8 indicates there's a problem when
     "foob" has been scanned.  Indeed, on any character other than an
     ‘a’, the scanner will have to back up to accept "foo".  Similarly,
     the comment for State #9 concerns when "fooba" has been scanned and
     an ‘r’ does not follow.

     The final comment reminds us that there's no point going to all the
     trouble of removing backing up from the rules unless we're using
     -𝐂𝗳 or -𝐂𝐅, since there's no performance gain doing so with com‐
     pressed scanners.

     The way to remove the backing up is to add "error" rules:

           %%
           foo    return TOK_KEYWORD;
           foobar return TOK_KEYWORD;

           fooba  |
           foob   |
           fo {
                   /* false alarm, not really a keyword */
                   return TOK_ID;
           }

     Eliminating backing up among a list of keywords can also be done
     using a "catch-all" rule:

           %%
           foo    return TOK_KEYWORD;
           foobar return TOK_KEYWORD;

           [a-z]+ return TOK_ID;

     This is usually the best solution when appropriate.

     Backing up messages tend to cascade.  With a complicated set of
     rules it's not uncommon to get hundreds of messages.  If one can
     decipher them, though, it often only takes a dozen or so rules to
     eliminate the backing up (though it's easy to make a mistake and
     have an error rule accidentally match a valid token; a possible
     future 𝗳𝗹𝗲𝘅 feature will be to automatically add rules to eliminate
     backing up).

     It's important to keep in mind that the benefits of eliminating
     backing up are gained only if e̲v̲e̲r̲y̲ instance of backing up is elim‐
     inated.  Leaving just one gains nothing.

     V̲a̲r̲i̲a̲b̲l̲e̲ trailing context (where both the leading and trailing
     parts do not have a fixed length) entails almost the same perfor‐
     mance loss as R̲E̲J̲E̲C̲T̲ (i.e., substantial).  So when possible a rule
     like:

           %%
           mouse|rat/(cat|dog)   run();

     is better written:

           %%
           mouse/cat|dog         run();
           rat/cat|dog           run();

     or as

           %%
           mouse|rat/cat         run();
           mouse|rat/dog         run();

     Note that here the special ‘|’ action does not provide any savings,
     and can even make things worse (see B̲U̲G̲S̲ below).

     Another area where the user can increase a scanner's performance
     (and one that's easier to implement) arises from the fact that the
     longer the tokens matched, the faster the scanner will run.  This
     is because with long tokens the processing of most input characters
     takes place in the (short) inner scanning loop, and does not often
     have to go through the additional work of setting up the scanning
     environment (e.g., y̲y̲t̲e̲x̲t̲) for the action.  Recall the scanner for
     C comments:

           %x comment
           %%
           int line_num = 1;

           "/*"                    BEGIN(comment);

           <comment>[^*\n]*
           <comment>"*"+[^*/\n]*
           <comment>\n             ++line_num;
           <comment>"*"+"/"        BEGIN(INITIAL);

     This could be sped up by writing it as:

           %x comment
           %%
           int line_num = 1;

           "/*"                    BEGIN(comment);

           <comment>[^*\n]*
           <comment>[^*\n]*\n      ++line_num;
           <comment>"*"+[^*/\n]*
           <comment>"*"+[^*/\n]*\n ++line_num;
           <comment>"*"+"/"        BEGIN(INITIAL);

     Now instead of each newline requiring the processing of another
     action, recognizing the newlines is "distributed" over the other
     rules to keep the matched text as long as possible.  Note that
     adding rules does n̲o̲t̲ slow down the scanner!  The speed of the
     scanner is independent of the number of rules or (modulo the con‐
     siderations given at the beginning of this section) how complicated
     the rules are with regard to operators such as ‘*’ and ‘|’.

     A final example in speeding up a scanner: scan through a file con‐
     taining identifiers and keywords, one per line and with no other
     extraneous characters, and recognize all the keywords.  A natural
     first approach is:

           %%
           asm      |
           auto     |
           break    |
           ... etc ...
           volatile |
           while    /* it's a keyword */

           .|\n     /* it's not a keyword */

     To eliminate the back-tracking, introduce a catch-all rule:

           %%
           asm      |
           auto     |
           break    |
           ... etc ...
           volatile |
           while    /* it's a keyword */

           [a-z]+   |
           .|\n     /* it's not a keyword */

     Now, if it's guaranteed that there's exactly one word per line,
     then we can reduce the total number of matches by a half by merging
     in the recognition of newlines with that of the other tokens:

           %%
           asm\n      |
           auto\n     |
           break\n    |
           ... etc ...
           volatile\n |
           while\n    /* it's a keyword */

           [a-z]+\n   |
           .|\n       /* it's not a keyword */

     One has to be careful here, as we have now reintroduced backing up
     into the scanner.  In particular, while we know that there will
     never be any characters in the input stream other than letters or
     newlines, 𝗳𝗹𝗲𝘅 can't figure this out, and it will plan for possibly
     needing to back up when it has scanned a token like "auto" and then
     the next character is something other than a newline or a letter.
     Previously it would then just match the "auto" rule and be done,
     but now it has no "auto" rule, only an "auto\n" rule.  To eliminate
     the possibility of backing up, we could either duplicate all rules
     but without final newlines or, since we never expect to encounter
     such an input and therefore don't how it's classified, we can
     introduce one more catch-all rule, this one which doesn't include a
     newline:

           %%
           asm\n      |
           auto\n     |
           break\n    |
           ... etc ...
           volatile\n |
           while\n    /* it's a keyword */

           [a-z]+\n   |
           [a-z]+     |
           .|\n       /* it's not a keyword */

     Compiled with -𝐂𝗳, this is about as fast as one can get a 𝗳𝗹𝗲𝘅
     scanner to go for this particular problem.

     A final note: 𝗳𝗹𝗲𝘅 is slow when matching NUL's, particularly when a
     token contains multiple NUL's.  It's best to write rules which
     match short amounts of text if it's anticipated that the text will
     often include NUL's.

     Another final note regarding performance: as mentioned above in the
     section H̲O̲W̲ T̲H̲E̲ I̲N̲P̲U̲T̲ I̲S̲ M̲A̲T̲C̲H̲E̲D̲, dynamically resizing y̲y̲t̲e̲x̲t̲ to
     accommodate huge tokens is a slow process because it presently
     requires that the (huge) token be rescanned from the beginning.
     Thus if performance is vital, it is better to attempt to match
     "large" quantities of text but not "huge" quantities, where the
     cutoff between the two is at about 8K characters/token.

𝐆𝐄𝐍𝐄𝐑𝐀𝐓𝐈𝐍𝐆 𝐂++ 𝐒𝐂𝐀𝐍𝐍𝐄𝐑𝐒
     𝗳𝗹𝗲𝘅 provides two different ways to generate scanners for use with
     C++.  The first way is to simply compile a scanner generated by
     𝗳𝗹𝗲𝘅 using a C++ compiler instead of a C compiler.  This should not
     generate any compilation errors (please report any found to the
     email address given in the A̲U̲T̲H̲O̲R̲S̲ section below).  C++ code can
     then be used in rule actions instead of C code.  Note that the
     default input source for scanners remains y̲y̲i̲n̲, and default echoing
     is still done to y̲y̲o̲u̲t̲.  Both of these remain F̲I̲L̲E̲ *̲ variables and
     not C++ streams.

     𝗳𝗹𝗲𝘅 can also be used to generate a C++ scanner class, using the -+
     option (or, equivalently, “%option c++”), which is automatically
     specified if the name of the flex executable ends in a ‘+’, such as
     𝗳𝗹𝗲𝘅++.  When using this option, 𝗳𝗹𝗲𝘅 defaults to generating the
     scanner to the file l̲e̲x̲.̲y̲y̲.̲c̲c̲ instead of l̲e̲x̲.̲y̲y̲.̲c̲.  The generated
     scanner includes the header file <g̲+̲+̲/̲F̲l̲e̲x̲L̲e̲x̲e̲r̲.̲h̲>, which defines
     the interface to two C++ classes.

     The first class, F̲l̲e̲x̲L̲e̲x̲e̲r̲, provides an abstract base class defin‐
     ing the general scanner class interface.  It provides the following
     member functions:

     const char* YYText()
             Returns the text of the most recently matched token, the
             equivalent of y̲y̲t̲e̲x̲t̲.

     int YYLeng()
             Returns the length of the most recently matched token, the
             equivalent of y̲y̲l̲e̲n̲g̲.

     int lineno() const
             Returns the current input line number (see “%option
             yylineno”), or 1 if “%option yylineno” was not used.

     void set_debug(int flag)
             Sets the debugging flag for the scanner, equivalent to
             assigning to y̲y̲_f̲l̲e̲x̲_d̲e̲b̲u̲g̲ (see the O̲P̲T̲I̲O̲N̲S̲ section above).
             Note that the scanner must be built using “%option debug”
             to include debugging information in it.

     int debug() const
             Returns the current setting of the debugging flag.

     Also provided are member functions equivalent to
     𝘆𝘆_𝘀𝘄𝗶𝘁𝗰𝗵_𝘁𝗼_𝗯𝘂𝗳𝗳𝗲𝗿(), 𝘆𝘆_𝗰𝗿𝗲𝗮𝘁𝗲_𝗯𝘂𝗳𝗳𝗲𝗿() (though the first argu‐
     ment is an s̲t̲d̲:̲:̲i̲s̲t̲r̲e̲a̲m̲*̲ object pointer and not a F̲I̲L̲E̲*̲),
     𝘆𝘆_𝗳𝗹𝘂𝘀𝗵_𝗯𝘂𝗳𝗳𝗲𝗿(), 𝘆𝘆_𝗱𝗲𝗹𝗲𝘁𝗲_𝗯𝘂𝗳𝗳𝗲𝗿(), and 𝘆𝘆𝗿𝗲𝘀𝘁𝗮𝗿𝘁() (again, the
     first argument is an s̲t̲d̲:̲:̲i̲s̲t̲r̲e̲a̲m̲*̲ object pointer).

     The second class defined in <g̲+̲+̲/̲F̲l̲e̲x̲L̲e̲x̲e̲r̲.̲h̲> is y̲y̲F̲l̲e̲x̲L̲e̲x̲e̲r̲, which
     is derived from F̲l̲e̲x̲L̲e̲x̲e̲r̲.  It defines the following additional
     member functions:

     yyFlexLexer(std::istream* arg_yyin = 0, std::ostream* arg_yyout =
             0)
             Constructs a y̲y̲F̲l̲e̲x̲L̲e̲x̲e̲r̲ object using the given streams for
             input and output.  If not specified, the streams default to
             c̲i̲n̲ and c̲o̲u̲t̲, respectively.

     virtual int yylex()
             Performs the same role as 𝘆𝘆𝗹𝗲𝘅() does for ordinary flex
             scanners: it scans the input stream, consuming tokens,
             until a rule's action returns a value.  If subclass ‘S’ is
             derived from y̲y̲F̲l̲e̲x̲L̲e̲x̲e̲r̲, in order to access the member
             functions and variables of ‘S’ inside 𝘆𝘆𝗹𝗲𝘅(), use “%option
             yyclass="S"” to inform 𝗳𝗹𝗲𝘅 that the ‘S’ subclass will be
             used instead of y̲y̲F̲l̲e̲x̲L̲e̲x̲e̲r̲.  In this case, rather than
             generating “yyFlexLexer::yylex()”, 𝗳𝗹𝗲𝘅 generates
             “S::yylex()” (and also generates a dummy
             “yyFlexLexer::yylex()” that calls
             “yyFlexLexer::LexerError()” if called).

     virtual void switch_streams(std::istream* new_in = 0, std::ostream*
             new_out = 0)
             Reassigns y̲y̲i̲n̲ to n̲e̲w̲_i̲n̲ (if non-nil) and y̲y̲o̲u̲t̲ to n̲e̲w̲_o̲u̲t̲
             (ditto), deleting the previous input buffer if y̲y̲i̲n̲ is
             reassigned.

     int yylex(std::istream* new_in, std::ostream* new_out = 0)
             First switches the input streams via
             “switch_streams(new_in, new_out)” and then returns the
             value of 𝘆𝘆𝗹𝗲𝘅().

     In addition, y̲y̲F̲l̲e̲x̲L̲e̲x̲e̲r̲ defines the following protected virtual
     functions which can be redefined in derived classes to tailor the
     scanner:

     virtual int LexerInput(char* buf, int max_size)
             Reads up to m̲a̲x̲_s̲i̲z̲e̲ characters into b̲u̲f̲ and returns the
             number of characters read.  To indicate end-of-input,
             return 0 characters.  Note that "interactive" scanners (see
             the -𝐁 and -𝐈 flags) define the macro YY_INTERACTIVE.  If
             𝐋𝗲𝘅𝗲𝗿𝐈𝗻𝗽𝘂𝘁() has been redefined, and it's necessary to take
             different actions depending on whether or not the scanner
             might be scanning an interactive input source, it's possi‐
             ble to test for the presence of this name via “#ifdef”.

     virtual void LexerOutput(const char* buf, int size)
             Writes out s̲i̲z̲e̲ characters from the buffer b̲u̲f̲, which,
             while NUL-terminated, may also contain "internal" NUL's if
             the scanner's rules can match text with NUL's in them.

     virtual void LexerError(const char* msg)
             Reports a fatal error message.  The default version of this
             function writes the message to the stream c̲e̲r̲r̲ and exits.

     Note that a y̲y̲F̲l̲e̲x̲L̲e̲x̲e̲r̲ object contains its entire scanning state.
     Thus such objects can be used to create reentrant scanners.  Multi‐
     ple instances of the same y̲y̲F̲l̲e̲x̲L̲e̲x̲e̲r̲ class can be instantiated,
     and multiple C++ scanner classes can be combined in the same pro‐
     gram using the -𝐏 option discussed above.

     Finally, note that the “%array” feature is not available to C++
     scanner classes; “%pointer” must be used (the default).

     Here is an example of a simple C++ scanner:

           // An example of using the flex C++ scanner class.

           %{
           #include <errno.h>
           int mylineno = 0;
           %}

           string  \"[^\n"]+\"

           ws      [ \t]+

           alpha   [A-Za-z]
           dig     [0-9]
           name    ({alpha}|{dig}|\$)({alpha}|{dig}|[_.\-/$])*
           num1    [-+]?{dig}+\.?([eE][-+]?{dig}+)?
           num2    [-+]?{dig}*\.{dig}+([eE][-+]?{dig}+)?
           number  {num1}|{num2}

           %%

           {ws}    /* skip blanks and tabs */

           "/*" {
                   int c;

                   while ((c = yyinput()) != 0) {
                           if(c == '\n')
                               ++mylineno;
                           else if(c == '*') {
                               if ((c = yyinput()) == '/')
                                   break;
                               else
                                   unput(c);
                           }
                   }
           }

           {number}  cout << "number " << YYText() << '\n';

           \n        mylineno++;

           {name}    cout << "name " << YYText() << '\n';

           {string}  cout << "string " << YYText() << '\n';

           %%

           int main(int /* argc */, char** /* argv */)
           {
                   FlexLexer* lexer = new yyFlexLexer;
                   while(lexer->yylex() != 0)
                       ;
                   return 0;
           }

     To create multiple (different) lexer classes, use the -𝐏 flag (or
     the “prefix=” option) to rename each y̲y̲F̲l̲e̲x̲L̲e̲x̲e̲r̲ to some other
     x̲x̲F̲l̲e̲x̲L̲e̲x̲e̲r̲.  <g̲+̲+̲/̲F̲l̲e̲x̲L̲e̲x̲e̲r̲.̲h̲> can then be included in other
     sources once per lexer class, first renaming y̲y̲F̲l̲e̲x̲L̲e̲x̲e̲r̲ as fol‐
     lows:

           #undef yyFlexLexer
           #define yyFlexLexer xxFlexLexer
           #include <g++/FlexLexer.h>

           #undef yyFlexLexer
           #define yyFlexLexer zzFlexLexer
           #include <g++/FlexLexer.h>

     If, for example, “%option prefix="xx"” is used for one scanner and
     “%option prefix="zz"” is used for the other.

     𝐈𝐌𝐏𝐎𝐑𝐓𝐀𝐍𝐓: the present form of the scanning class is experimental
     and may change considerably between major releases.

𝐈𝐍𝐂𝐎𝐌𝐏𝐀𝐓𝐈𝐁𝐈𝐋𝐈𝐓𝐈𝐄𝐒 𝐖𝐈𝐓𝐇 𝐋𝐄𝐗 𝐀𝐍𝐃 𝐏𝐎𝐒𝐈𝐗
     𝗳𝗹𝗲𝘅 is a rewrite of the AT&T UNIX 𝗹𝗲𝘅 tool (the two implementa‐
     tions do not share any code, though), with some extensions and
     incompatibilities, both of which are of concern to those who wish
     to write scanners acceptable to either implementation.  𝗳𝗹𝗲𝘅 is
     fully compliant with the POSIX 𝗹𝗲𝘅 specification, except that when
     using “%pointer” (the default), a call to 𝘂𝗻𝗽𝘂𝘁() destroys the con‐
     tents of y̲y̲t̲e̲x̲t̲, which is counter to the POSIX specification.

     In this section we discuss all of the known areas of incompatibil‐
     ity between 𝗳𝗹𝗲𝘅, AT&T UNIX 𝗹𝗲𝘅, and the POSIX specification.

     𝗳𝗹𝗲𝘅's -𝗹 option turns on maximum compatibility with the original
     AT&T UNIX 𝗹𝗲𝘅 implementation, at the cost of a major loss in the
     generated scanner's performance.  We note below which incompatibil‐
     ities can be overcome using the -𝗹 option.

     𝗳𝗹𝗲𝘅 is fully compatible with 𝗹𝗲𝘅 with the following exceptions:

     -   The undocumented 𝗹𝗲𝘅 scanner internal variable y̲y̲l̲i̲n̲e̲n̲o̲ is not
         supported unless -𝗹 or “%option yylineno” is used.

         y̲y̲l̲i̲n̲e̲n̲o̲ should be maintained on a per-buffer basis, rather
         than a per-scanner (single global variable) basis.

         y̲y̲l̲i̲n̲e̲n̲o̲ is not part of the POSIX specification.

     -   The 𝗶𝗻𝗽𝘂𝘁() routine is not redefinable, though it may be called
         to read characters following whatever has been matched by a
         rule.  If 𝗶𝗻𝗽𝘂𝘁() encounters an end-of-file, the normal
         𝘆𝘆𝘄𝗿𝗮𝗽() processing is done.  A “real” end-of-file is returned
         by 𝗶𝗻𝗽𝘂𝘁() as EOF.

         Input is instead controlled by defining the YY_INPUT macro.

         The 𝗳𝗹𝗲𝘅 restriction that 𝗶𝗻𝗽𝘂𝘁() cannot be redefined is in
         accordance with the POSIX specification, which simply does not
         specify any way of controlling the scanner's input other than
         by making an initial assignment to y̲y̲i̲n̲.

     -   The 𝘂𝗻𝗽𝘂𝘁() routine is not redefinable.  This restriction is in
         accordance with POSIX.

     -   𝗳𝗹𝗲𝘅 scanners are not as reentrant as 𝗹𝗲𝘅 scanners.  In partic‐
         ular, if a scanner is interactive and an interrupt handler
         long-jumps out of the scanner, and the scanner is subsequently
         called again, the following error message may be displayed:

               fatal flex scanner internal error--end of buffer missed

         To reenter the scanner, first use

               yyrestart(yyin);

         Note that this call will throw away any buffered input; usually
         this isn't a problem with an interactive scanner.

         Also note that flex C++ scanner classes are reentrant, so if
         using C++ is an option , they should be used instead.  See
         G̲E̲N̲E̲R̲A̲T̲I̲N̲G̲ C̲+̲+̲ S̲C̲A̲N̲N̲E̲R̲S̲ above for details.

     -   𝗼𝘂𝘁𝗽𝘂𝘁() is not supported.  Output from the E̲C̲H̲O̲ macro is done
         to the file-pointer y̲y̲o̲u̲t̲ (default stdout).

         𝗼𝘂𝘁𝗽𝘂𝘁() is not part of the POSIX specification.

     -   𝗹𝗲𝘅 does not support exclusive start conditions (%x), though
         they are in the POSIX specification.

     -   When definitions are expanded, 𝗳𝗹𝗲𝘅 encloses them in parenthe‐
         ses.  With 𝗹𝗲𝘅, the following:

               NAME    [A-Z][A-Z0-9]*
               %%
               foo{NAME}?      printf("Found it\n");
               %%

         will not match the string "foo" because when the macro is
         expanded the rule is equivalent to "foo[A-Z][A-Z0-9]*?" and the
         precedence is such that the ‘?’ is associated with "[A-Z0-9]*".
         With 𝗳𝗹𝗲𝘅, the rule will be expanded to "foo([A-Z][A-Z0-9]*)?"
         and so the string "foo" will match.

         Note that if the definition begins with ‘^’ or ends with ‘$’
         then it is not expanded with parentheses, to allow these opera‐
         tors to appear in definitions without losing their special
         meanings.  But the ‘⟨s⟩’, ‘/’, and ⟨⟨EOF⟩⟩ operators cannot be
         used in a 𝗳𝗹𝗲𝘅 definition.

         Using -𝗹 results in the 𝗹𝗲𝘅 behavior of no parentheses around
         the definition.

         The POSIX specification is that the definition be enclosed in
         parentheses.

     -   Some implementations of 𝗹𝗲𝘅 allow a rule's action to begin on a
         separate line, if the rule's pattern has trailing whitespace:

               %%
               foo|bar<space here>
                 { foobar_action(); }

         𝗳𝗹𝗲𝘅 does not support this feature.

     -   The 𝗹𝗲𝘅 ‘%r’ (generate a Ratfor scanner) option is not sup‐
         ported.  It is not part of the POSIX specification.

     -   After a call to 𝘂𝗻𝗽𝘂𝘁(), y̲y̲t̲e̲x̲t̲ is undefined until the next
         token is matched, unless the scanner was built using “%array”.
         This is not the case with 𝗹𝗲𝘅 or the POSIX specification.  The
         -𝗹 option does away with this incompatibility.

     -   The precedence of the ‘{}’ (numeric range) operator is differ‐
         ent.  𝗹𝗲𝘅 interprets "abc{1,3}" as match one, two, or three
         occurrences of ‘abc’, whereas 𝗳𝗹𝗲𝘅 interprets it as match ‘ab’
         followed by one, two, or three occurrences of ‘c’.  The latter
         is in agreement with the POSIX specification.

     -   The precedence of the ‘^’ operator is different.  𝗹𝗲𝘅 inter‐
         prets "^foo|bar" as match either ‘foo’ at the beginning of a
         line, or ‘bar’ anywhere, whereas 𝗳𝗹𝗲𝘅 interprets it as match
         either ‘foo’ or ‘bar’ if they come at the beginning of a line.
         The latter is in agreement with the POSIX specification.

     -   The special table-size declarations such as ‘%a’ supported by
         𝗹𝗲𝘅 are not required by 𝗳𝗹𝗲𝘅 scanners; 𝗳𝗹𝗲𝘅 ignores them.

     -   The name FLEX_SCANNER is #define'd so scanners may be written
         for use with either 𝗳𝗹𝗲𝘅 or 𝗹𝗲𝘅.  Scanners also include
         YY_FLEX_MAJOR_VERSION and YY_FLEX_MINOR_VERSION indicating
         which version of 𝗳𝗹𝗲𝘅 generated the scanner (for example, for
         the 2.5 release, these defines would be 2 and 5, respectively).

     The following 𝗳𝗹𝗲𝘅 features are not included in 𝗹𝗲𝘅 or the POSIX
     specification:

           C++ scanners
           %option
           start condition scopes
           start condition stacks
           interactive/non-interactive scanners
           yy_scan_string() and friends
           yyterminate()
           yy_set_interactive()
           yy_set_bol()
           YY_AT_BOL()
           <<EOF>>
           <*>
           YY_DECL
           YY_START
           YY_USER_ACTION
           YY_USER_INIT
           #line directives
           %{}'s around actions
           multiple actions on a line

     plus almost all of the 𝗳𝗹𝗲𝘅 flags.  The last feature in the list
     refers to the fact that with 𝗳𝗹𝗲𝘅 multiple actions can be placed on
     the same line, separated with semi-colons, while with 𝗹𝗲𝘅, the fol‐
     lowing

           foo handle_foo(); ++num_foos_seen;

     is (rather surprisingly) truncated to

           foo handle_foo();

     𝗳𝗹𝗲𝘅 does not truncate the action.  Actions that are not enclosed
     in braces are simply terminated at the end of the line.

𝐅𝐈𝐋𝐄𝐒
     flex.skl           Skeleton scanner.  This file is only used when
                        building flex, not when 𝗳𝗹𝗲𝘅 executes.

     lex.backup         Backing-up information for the -𝗯 flag (called
                        l̲e̲x̲.̲b̲c̲k̲ on some systems).

     lex.yy.c           Generated scanner (called l̲e̲x̲y̲y̲.̲c̲ on some sys‐
                        tems).

     lex.yy.cc          Generated C++ scanner class, when using -+.

     <g++/FlexLexer.h>  Header file defining the C++ scanner base class,
                        F̲l̲e̲x̲L̲e̲x̲e̲r̲, and its derived class, y̲y̲F̲l̲e̲x̲L̲e̲x̲e̲r̲.

     /usr/lib/libl.*    𝗳𝗹𝗲𝘅 libraries.  The /̲u̲s̲r̲/̲l̲i̲b̲/̲l̲i̲b̲f̲l̲.̲*̲ libraries
                        are links to these.  Scanners must be linked
                        using either -𝗹𝗹 or -𝗹𝗳𝗹.

𝐄𝐗𝐈𝐓 𝐒𝐓𝐀𝐓𝐔𝐒
     The 𝗳𝗹𝗲𝘅 utility exits 0 on success, and >0 if an error occurs.

𝐃𝐈𝐀𝐆𝐍𝐎𝐒𝐓𝐈𝐂𝐒
     𝘄𝗮𝗿𝗻𝗶𝗻𝗴, 𝗿𝘂𝗹𝗲 𝗰𝗮𝗻𝗻𝗼𝘁 𝗯𝗲 𝗺𝗮𝘁𝗰𝗵𝗲𝗱  Indicates that the given rule can‐
     not be matched because it follows other rules that will always
     match the same text as it.  For example, in the following “foo”
     cannot be matched because it comes after an identifier "catch-all"
     rule:

           [a-z]+    got_identifier();
           foo       got_foo();

     Using R̲E̲J̲E̲C̲T̲ in a scanner suppresses this warning.

     𝘄𝗮𝗿𝗻𝗶𝗻𝗴, -𝘀 𝗼𝗽𝘁𝗶𝗼𝗻 𝗴𝗶𝘃𝗲𝗻 𝗯𝘂𝘁 𝗱𝗲𝗳𝗮𝘂𝗹𝘁 𝗿𝘂𝗹𝗲 𝗰𝗮𝗻 𝗯𝗲 𝗺𝗮𝘁𝗰𝗵𝗲𝗱  Means
     that it is possible (perhaps only in a particular start condition)
     that the default rule (match any single character) is the only one
     that will match a particular input.  Since -𝘀 was given, presumably
     this is not intended.

     𝗿𝗲𝗷𝗲𝗰𝘁_𝘂𝘀𝗲𝗱_𝗯𝘂𝘁_𝗻𝗼𝘁_𝗱𝗲𝘁𝗲𝗰𝘁𝗲𝗱 𝘂𝗻𝗱𝗲𝗳𝗶𝗻𝗲𝗱
     𝘆𝘆𝗺𝗼𝗿𝗲_𝘂𝘀𝗲𝗱_𝗯𝘂𝘁_𝗻𝗼𝘁_𝗱𝗲𝘁𝗲𝗰𝘁𝗲𝗱 𝘂𝗻𝗱𝗲𝗳𝗶𝗻𝗲𝗱  These errors can occur at
     compile time.  They indicate that the scanner uses R̲E̲J̲E̲C̲T̲ or
     𝘆𝘆𝗺𝗼𝗿𝗲() but that 𝗳𝗹𝗲𝘅 failed to notice the fact, meaning that 𝗳𝗹𝗲𝘅
     scanned the first two sections looking for occurrences of these
     actions and failed to find any, but somehow they snuck in (via an
     #include file, for example).  Use “%option reject” or “%option
     yymore” to indicate to 𝗳𝗹𝗲𝘅 that these features are really needed.

     𝗳𝗹𝗲𝘅 𝘀𝗰𝗮𝗻𝗻𝗲𝗿 𝗷𝗮𝗺𝗺𝗲𝗱  A scanner compiled with -𝘀 has encountered an
     input string which wasn't matched by any of its rules.  This error
     can also occur due to internal problems.

     𝘁𝗼𝗸𝗲𝗻 𝘁𝗼𝗼 𝗹𝗮𝗿𝗴𝗲, 𝗲𝘅𝗰𝗲𝗲𝗱𝘀 𝐘𝐘𝐋𝐌𝐀𝐗  The scanner uses “%array” and one
     of its rules matched a string longer than the YYLMAX constant (8K
     bytes by default).  The value can be increased by #define'ing
     YYLMAX in the definitions section of 𝗳𝗹𝗲𝘅 input.

     𝘀𝗰𝗮𝗻𝗻𝗲𝗿 𝗿𝗲𝗾𝘂𝗶𝗿𝗲𝘀 -𝟴 𝗳𝗹𝗮𝗴 𝘁𝗼 𝘂𝘀𝗲 𝘁𝗵𝗲 𝗰𝗵𝗮𝗿𝗮𝗰𝘁𝗲𝗿 '𝘅'  The scanner
     specification includes recognizing the 8-bit character ‘x’ and the
     -𝟴 flag was not specified, and defaulted to 7-bit because the -𝐂𝗳
     or -𝐂𝐅 table compression options were used.  See the discussion of
     the -𝟳 flag for details.

     𝗳𝗹𝗲𝘅 𝘀𝗰𝗮𝗻𝗻𝗲𝗿 𝗽𝘂𝘀𝗵-𝗯𝗮𝗰𝗸 𝗼𝘃𝗲𝗿𝗳𝗹𝗼𝘄  unput() was used to push back so
     much text that the scanner's buffer could not hold both the pushed-
     back text and the current token in y̲y̲t̲e̲x̲t̲.  Ideally the scanner
     should dynamically resize the buffer in this case, but at present
     it does not.

     𝗶𝗻𝗽𝘂𝘁 𝗯𝘂𝗳𝗳𝗲𝗿 𝗼𝘃𝗲𝗿𝗳𝗹𝗼𝘄, 𝗰𝗮𝗻'𝘁 𝗲𝗻𝗹𝗮𝗿𝗴𝗲 𝗯𝘂𝗳𝗳𝗲𝗿 𝗯𝗲𝗰𝗮𝘂𝘀𝗲 𝘀𝗰𝗮𝗻𝗻𝗲𝗿 𝘂𝘀𝗲𝘀
     𝐑𝐄𝐉𝐄𝐂𝐓  The scanner was working on matching an extremely large
     token and needed to expand the input buffer.  This doesn't work
     with scanners that use R̲E̲J̲E̲C̲T̲.

     𝗳𝗮𝘁𝗮𝗹 𝗳𝗹𝗲𝘅 𝘀𝗰𝗮𝗻𝗻𝗲𝗿 𝗶𝗻𝘁𝗲𝗿𝗻𝗮𝗹 𝗲𝗿𝗿𝗼𝗿--𝗲𝗻𝗱 𝗼𝗳 𝗯𝘂𝗳𝗳𝗲𝗿 𝗺𝗶𝘀𝘀𝗲𝗱  This can
     occur in an scanner which is reentered after a long-jump has jumped
     out (or over) the scanner's activation frame.  Before reentering
     the scanner, use:

           yyrestart(yyin);

     or, as noted above, switch to using the C++ scanner class.

     𝘁𝗼𝗼 𝗺𝗮𝗻𝘆 𝘀𝘁𝗮𝗿𝘁 𝗰𝗼𝗻𝗱𝗶𝘁𝗶𝗼𝗻𝘀 𝗶𝗻 <> 𝗰𝗼𝗻𝘀𝘁𝗿𝘂𝗰𝘁!  More start conditions
     than exist were listed in a <> construct (so at least one of them
     must have been listed twice).

𝐒𝐄𝐄 𝐀𝐋𝐒𝐎
     awk(1), sed(1), yacc(1)

     John Levine, Tony Mason, and Doug Brown, L̲e̲x̲ &̲ Y̲a̲c̲c̲, O̲'̲R̲e̲i̲l̲l̲y̲ a̲n̲d̲
     A̲s̲s̲o̲c̲i̲a̲t̲e̲s̲, 2nd edition.

     Alfred Aho, Ravi Sethi, and Jeffrey Ullman, C̲o̲m̲p̲i̲l̲e̲r̲s̲:̲ P̲r̲i̲n̲c̲i̲p̲l̲e̲s̲,̲
     T̲e̲c̲h̲n̲i̲q̲u̲e̲s̲ a̲n̲d̲ T̲o̲o̲l̲s̲, A̲d̲d̲i̲s̲o̲n̲-̲W̲e̲s̲l̲e̲y̲, 1986, Describes the pattern-
     matching techniques used by flex (deterministic finite automata).

𝐒𝐓𝐀𝐍𝐃𝐀𝐑𝐃𝐒
     The 𝗹𝗲𝘅 utility is compliant with the IEEE Std 1003.1-2008
     (“POSIX.1”) specification, though its presence is optional.

     The flags [-𝟳𝟴𝐁𝗯𝐂𝗱𝐅𝗳𝗵𝐈𝗶𝐋𝗹𝗼𝐏𝗽𝐒𝘀𝐓𝐕𝘄+?], [--𝗵𝗲𝗹𝗽], and [--𝘃𝗲𝗿𝘀𝗶𝗼𝗻] are
     extensions to that specification.

     See also the I̲N̲C̲O̲M̲P̲A̲T̲I̲B̲I̲L̲I̲T̲I̲E̲S̲ W̲I̲T̲H̲ L̲E̲X̲ A̲N̲D̲ P̲O̲S̲I̲X̲ section, above.

𝐀𝐔𝐓𝐇𝐎𝐑𝐒
     Vern Paxson, with the help of many ideas and much inspiration from
     Van Jacobson.  Original version by Jef Poskanzer.  The fast table
     representation is a partial implementation of a design done by Van
     Jacobson.  The implementation was done by Kevin Gong and Vern Pax‐
     son.

     Thanks to the many 𝗳𝗹𝗲𝘅 beta-testers, feedbackers, and contribu‐
     tors, especially Francois Pinard, Casey Leedom, Robert Abramovitz,
     Stan Adermann, Terry Allen, David Barker-Plummer, John Basrai, Neal
     Becker, Nelson H.F. Beebe, b̲e̲n̲s̲o̲n̲@̲o̲d̲i̲.̲c̲o̲m̲, Karl Berry, Peter A.
     Bigot, Simon Blanchard, Keith Bostic, Frederic Brehm, Ian Brock‐
     bank, Kin Cho, Nick Christopher, Brian Clapper, J.T. Conklin, Jason
     Coughlin, Bill Cox, Nick Cropper, Dave Curtis, Scott David Daniels,
     Chris G. Demetriou, Theo de Raadt, Mike Donahue, Chuck Doucette,
     Tom Epperly, Leo Eskin, Chris Faylor, Chris Flatters, Jon Forrest,
     Jeffrey Friedl, Joe Gayda, Kaveh R. Ghazi, Wolfgang Glunz, Eric
     Goldman, Christopher M. Gould, Ulrich Grepel, Peer Griebel, Jan
     Hajic, Charles Hemphill, NORO Hideo, Jarkko Hietaniemi, Scott Hof‐
     mann, Jeff Honig, Dana Hudes, Eric Hughes, John Interrante, Ceriel
     Jacobs, Michal Jaegermann, Sakari Jalovaara, Jeffrey R. Jones,
     Henry Juengst, Klaus Kaempf, Jonathan I. Kamens, Terrence O Kane,
     Amir Katz, k̲e̲n̲@̲k̲e̲n̲.̲h̲i̲l̲c̲o̲.̲c̲o̲m̲, Kevin B. Kenny, Steve Kirsch, Win‐
     fried Koenig, Marq Kole, Ronald Lamprecht, Greg Lee, Rohan Lenard,
     Craig Leres, John Levine, Steve Liddle, David Loffredo, Mike Long,
     Mohamed el Lozy, Brian Madsen, Malte, Joe Marshall, Bengt Martens‐
     son, Chris Metcalf, Luke Mewburn, Jim Meyering, R. Alexander
     Milowski, Erik Naggum, G.T. Nicol, Landon Noll, James Nordby, Marc
     Nozell, Richard Ohnemus, Karsten Pahnke, Sven Panne, Roland Pesch,
     Walter Pelissero, Gaumond Pierre, Esmond Pitt, Jef Poskanzer, Joe
     Rahmeh, Jarmo Raiha, Frederic Raimbault, Pat Rankin, Rick Richard‐
     son, Kevin Rodgers, Kai Uwe Rommel, Jim Roskind, Alberto Santini,
     Andreas Scherer, Darrell Schiebel, Raf Schietekat, Doug Schmidt,
     Philippe Schnoebelen, Andreas Schwab, Larry Schwimmer, Alex Siegel,
     Eckehard Stolz, Jan-Erik Strvmquist, Mike Stump, Paul Stuart, Dave
     Tallman, Ian Lance Taylor, Chris Thewalt, Richard M. Timoney, Jodi
     Tsai, Paul Tuinenga, Gary Weik, Frank Whaley, Gerhard Wilhelms,
     Kent Williams, Ken Yap, Ron Zellar, Nathan Zelle, David Zuhn, and
     those whose names have slipped my marginal mail-archiving skills
     but whose contributions are appreciated all the same.

     Thanks to Keith Bostic, Jon Forrest, Noah Friedman, John Gilmore,
     Craig Leres, John Levine, Bob Mulcahy, G.T.  Nicol, Francois
     Pinard, Rich Salz, and Richard Stallman for help with various dis‐
     tribution headaches.

     Thanks to Esmond Pitt and Earle Horton for 8-bit character support;
     to Benson Margulies and Fred Burke for C++ support; to Kent
     Williams and Tom Epperly for C++ class support; to Ove Ewerlid for
     support of NUL's; and to Eric Hughes for support of multiple buf‐
     fers.

     This work was primarily done when I was with the Real Time Systems
     Group at the Lawrence Berkeley Laboratory in Berkeley, CA.  Many
     thanks to all there for the support I received.

     Send comments to ⟨v̲e̲r̲n̲@̲e̲e̲.̲l̲b̲l̲.̲g̲o̲v̲⟩.

𝐁𝐔𝐆𝐒
     Some trailing context patterns cannot be properly matched and gen‐
     erate warning messages (dangerous trailing context).  These are
     patterns where the ending of the first part of the rule matches the
     beginning of the second part, such as "zx*/xy*", where the ‘x*’
     matches the ‘x’ at the beginning of the trailing context.  (Note
     that the POSIX draft states that the text matched by such patterns
     is undefined.)

     For some trailing context rules, parts which are actually fixed-
     length are not recognized as such, leading to the above mentioned
     performance loss.  In particular, parts using ‘|’ or ‘{n}’ (such as
     "foo{3}") are always considered variable-length.

     Combining trailing context with the special ‘|’ action can result
     in fixed trailing context being turned into the more expensive
     variable trailing context.  For example, in the following:

           %%
           abc      |
           xyz/def

     Use of 𝘂𝗻𝗽𝘂𝘁() invalidates yytext and yyleng, unless the “%array”
     directive or the -𝗹 option has been used.

     Pattern-matching of NUL's is substantially slower than matching
     other characters.

     Dynamic resizing of the input buffer is slow, as it entails rescan‐
     ning all the text matched so far by the current (generally huge)
     token.

     Due to both buffering of input and read-ahead, it is not possible
     to intermix calls to <s̲t̲d̲i̲o̲.̲h̲> routines, such as, for example,
     𝗴𝗲𝘁𝗰𝗵𝗮𝗿(), with 𝗳𝗹𝗲𝘅 rules and expect it to work.  Call 𝗶𝗻𝗽𝘂𝘁()
     instead.

     The total table entries listed by the -𝘃 flag excludes the number
     of table entries needed to determine what rule has been matched.
     The number of entries is equal to the number of DFA states if the
     scanner does not use R̲E̲J̲E̲C̲T̲, and somewhat greater than the number
     of states if it does.

     R̲E̲J̲E̲C̲T̲ cannot be used with the -𝗳 or -𝐅 options.

     The 𝗳𝗹𝗲𝘅 internal algorithms need documentation.

COSMOPOLITAN                September 21, 2015                          BSD
