;; ╔──────────────────────────────────────────────────────────────────╗
;; │ To the extent possible under law, Justine Tunney has waived      │
;; │ all copyright and related or neighboring rights to this file,    │
;; │ as it is written in the following disclaimers:                   │
;; │   • http://unlicense.org/                                        │
;; │   • http://creativecommons.org/publicdomain/zero/1.0/            │
;; ╚──────────────────────────────────────────────────────────────────╝
;; Reconfigures GNU Emacs syntax highlighting for GNU Assembler syntax.

(require 'asm-mode)
(require 'cosmo-cpp-constants)

(defun cosmo-regexpify (x)
  (let ((join (lambda (sep lis)
                (mapconcat 'identity lis sep))))
    (cond ((vectorp x)
           (funcall join "" (mapcar 'cosmo-regexpify x)))
          ((listp x)
           (concat "\\(?:"
                   (funcall join "\\|" (mapcar 'cosmo-regexpify x))
                   "\\)"))
          ('t x))))

(defun cosmo-fontify (faces regexp limit predicate)
  "Set FACES on REGEXP matches until LIMIT that satisfy PREDICATE."
  (while (re-search-forward regexp limit t)
    (let (i (face (save-excursion
                    (backward-char)
                    (face-at-point)))
            (syntax (syntax-ppss)))
      (when (funcall predicate face syntax)
        (dolist (i faces)
          (put-text-property (match-beginning (car i))
                             (match-end (car i))
                             'face
                             (cdr i)))))))

(defconst asm-mode-gas-qualifiers-regexp
  (cosmo-regexpify
   [("comdat"
     ":req"
     ":vararg"
     ["@" (;; calculates offset relative to tls block end
           ;; load: mov %fs:boop@tpoff,%eax
           ;; addr: mov %fs:0,%rax
           ;;       lea x1@tpoff(%rax),%rax
           ;; addr: mov %fs:0,%rax
           ;;       add $x1@tpoff,%rax
           "tpoff"
           ;; allocates TLS_index structure
           ;; lea x@tlsgd(,%rbx,1),%rdi
           ;; call __tls_get_addr@plt
           "tlsgd"
           "tlsld"
           "dtpmod"
           "dtpoff"
           "gottpoff"
           "function"
           "object"
           "notype"
           "got"
           "size"
           "gotoff"
           "plt"
           "pltoff"
           "gotpcrel"
           "progbits"
           "nobits"
           "init_array"
           "fini_array"
           "gnu_indirect_function")])
    "\\>"])
  "GNU Assembler section, relocation, macro param qualifiers.")

(defconst asm-mode-doctag-regexp
  (cosmo-regexpify
   '(["<"
      ("p" "br"
       "b" "/b"
       "i" "/i"
       "pre" "/pre"
       "h3" "h4" "/h3" "/h4"
       "ul" "ol" "li" "/ol" "/ul"
       "dl" "dt" "dd" "/dl"
       "table" "tr" "td" "th" "td" "/table")
      ">"]
     ["@"
      ("param"
       "return"
       "define"
       "see"
       "throws"
       "returnstwice"
       "fileoverview"
       "nullable"
       "noreturn"
       "domain"
       "clob"
       "since"
       "forcealignargpointer"
       "mode"
       "speed"
       "cost"
       "todo"
       "assume"
       "define"
       "domain"
       "code"
       "note"
       "protip"
       "nxbitsafe"
       "vforksafe"
       "threadsafe"
       "preinitsafe"
       "asyncsignalsafe"
       "notasyncsignalsafe"
       "isa"
       "norestart"
       "mayalias"
       "sideffect")
      "\\>"]))
  "Assembly docstring highlighting in Google Java Style.")

(defconst asm-mode-x86-prefix-ops-regexp
  (cosmo-regexpify
   [(["[lL][oO][cC][kK]\\>[ \t]*"
      ("[mM][oO][vV][sS]"
       "[aA][dD][dDcC]"
       "[sS][uUbB][bB]"
       "[oO][rR]"
       "[aA][nN][dD]"
       "[xX][oO][rR]"
       "[nN][oO][tT]"
       "[nN][eE][gG]"
       "[iI][nN][cC]"
       "[dD][eE][cC]"
       "[bB][tT][sSrRcC]?"
       "[xX][aA][dD][dD]"
       "[xX][cC][hH][gG]"
       ["[cC][mM][pP][xX][cC][hH][gG]" ("8[bB]" "16[bB]") "?"])]
     ["[rR][eE][pP]" ("[eEzZ]" "[nN][eEzZ]") "?\\>[ \t]*"
      ("[mM][oO][vV][sS]"
       "[sS][tT][oO][sS]"
       "[sS][cC][aA][sS]"
       "[cC][mM][pP][sS]"
       "[nN][oO][pP]"
       "[rR][eE][tT]"
       "[iI][nN][sS]"
       "[oO][uU][tT][sS]")])
    "[bBwWlLqQ]?\\>"])
  "Legal high-level 80x86 prefix instruction combinations.")

(defconst cosmo-asm-font-lock-keywords
  (append
   `(;; AT&T Fortran-Style Assembler Comment
     ;;
     ;;   - Valid
     ;;
     ;;     * /heyho
     ;;     * //heyho
     ;;
     ;;   - Ignored
     ;;
     ;;     *  /heyho
     ;;     * code code //heyho
     ;;
     ("^/.*$" . font-lock-comment-face)

     ;; Immediate Argument
     ;;
     ;;   - Valid
     ;;
     ;;     * mov $2,%eax
     ;;     * mov $~2+0x2ul,%eax
     ;;     * mov $'c,%eax
     ;;     * mov $'\n,%eax
     ;;
     ("[ \t,]\\$\\(\\(?:'\\(?:'\\|\\s\"\\|\\s\\.\\|.\\)\\|\\(?:0x[[:xdigit:]]+\\|0b[01]+\\|[1-9][0-9]*\\|0[0-7]*\\)\\(?:[fb]\\|u?l?l?\\)\\|[-*/&^|()%<>~+]\\|[_.$[:alpha:]][-_.[:alnum:]]*\\)+\\)"
      1 font-lock-constant-face)

     (cosmo-asm-doctag-keywords)

     ;; Static Branch Prediction
     ;;
     ;;   - Valid
     ;;
     ;;     * jnz,pt 1f
     ;;     * jnz,pn 1b
     ;;
     ;;   - Traditionally Implied
     ;;
     ;;     * jnz,pn 1f
     ;;     * jnz,pt 1b
     ;;
     (",p[tn]" . font-lock-keyword-face)

     ("\\\\\\(?:@\\|()\\)" . font-lock-function-name-face)
     ("\\\\\\(\\sw\\|_\\|\\.\\)+\\>" . font-lock-variable-name-face)
     (,asm-mode-x86-prefix-ops-regexp . font-lock-keyword-face)
     (,(concat "^\\(\\(?:\\sw\\|\\s_\\|\\.\\)+\\):[ \t]*"
               "\\(\\(?:\\sw\\|\\.\\)+\\)?")
      (1 font-lock-function-name-face)
      (2 font-lock-keyword-face nil t))
     ("^\\((\\sw+)\\)?\\s +\\(\\(\\.?\\sw\\|\\s_\\)+\\(\\.\\sw+\\)*\\)"
      2 font-lock-keyword-face)
     ("^\\(\\.\\(\\sw\\|\\s_\\|\\.\\)+\\)\\>[^:]?"
      1 font-lock-keyword-face)
     ("^.*?\\*/\\(\\.\\(\\sw\\|\\s_\\|\\.\\)+\\)\\>[^:]?"
      1 font-lock-keyword-face)

     ;; it's complicated
     (,asm-mode-gas-qualifiers-regexp . font-lock-type-face))

   cpp-font-lock-keywords

   `(("'\\(\\\\?.\\)\\>" 1 font-lock-constant-face)

     ;; Register Value
     ;;
     ;;   - Valid
     ;;
     ;;     * %al
     ;;     * %eax
     ;;
     ;;   - Ignored
     ;;
     ;;     * %%al
     ;;
     ("%\\sw+" . font-lock-variable-name-face)

     ;; Hexadecimal Literal
     ;;
     ;;   - Valid
     ;;
     ;;     * 0x0123456789abcdef
     ;;     * 0XDEADBEEFu
     ;;     * -0xdeadbeefULL
     ;;
     ;;   - Ignored
     ;;
     ;;     * 0x0123456789abcdefg
     ;;
     ("\\b\\(0[xX]\\)[[:xdigit:]]+\\([ulUL]*\\)\\b"
      (1 font-lock-constant-face)
      (2 font-lock-constant-face))

     ;; Binary Literal
     ;;
     ;;   - Valid
     ;;
     ;;     * 0b0101101101
     ;;     * 0B0101101101u
     ;;     * -0b0101101101ULL
     ;;
     ;;   - Ignored
     ;;
     ;;     * 0b012
     ;;
     ("\\b\\(0[bB]\\)[01]+\\([ulUL]*\\)\\b"
      (1 font-lock-constant-face)
      (2 font-lock-constant-face))

     ;; Octal Literal
     ;;
     ;;   - Valid
     ;;
     ;;     * 01234567
     ;;     * 01234567l
     ;;     * -01234567ULL
     ;;
     ;;   - Ignored
     ;;
     ;;     * 012345678
     ;;
     ("\\b\\(0\\)[0-7]+\\([ulUL]*\\)\\b"
      (1 font-lock-constant-face)
      (2 font-lock-constant-face))

     ;; Builtin Constants
     ;;
     ;;   - Valid
     ;;
     ;;     * 123456789
     ;;     * 123456789l
     ;;     * -01234567ULL
     ;;
     ;;   - Ignored
     ;;
     ;;     * 123456789a
     ;;     * 123456789aul
     ;;
     ("\\b[1-9][0-9]+\\([ulUL]*\\)\\b"
      1 font-lock-constant-face)

     ;; AT&T Fortran-Style Assembler Comment
     ;;
     ;;   - Valid
     ;;
     ;;     * /heyho
     ;;     * //heyho
     ;;
     ;;   - Ignored
     ;;
     ;;     *  /heyho
     ;;     * code code //heyho
     ;;
     ("^/.*$" . font-lock-comment-face)

     ;; AT&T-Style Directional Label
     ;;
     ;;   - Valid
     ;;
     ;;     * 1f
     ;;     * 99b
     ;;
     ("\\b\\([0-9]+\\)[fb]\\b" 1 font-lock-function-name-face)))
  "Additional expressions to highlight in Assembler mode.")

(defun cosmo-asm-doctag-keywords (limit)
  (cosmo-fontify '((0 . font-lock-constant-face))
                asm-mode-doctag-regexp
                limit
                (lambda (face syntax)
                  (or (memq face '(font-lock-comment-face))
                      (and syntax
                           (nth 4 syntax))))))

(defun cosmo-asm-supplemental-hook ()
  "GNU Assembly in Bell Laboratories Style."
  (setq asm-comment-char ?¯) ;; Was ESR using TASM?
  (font-lock-add-keywords 'asm-mode
                          '((cosmo-asm-doctag-keywords))
                          'append)
  (set (make-local-variable 'require-final-newline) nil)
  (set (make-local-variable 'indent-tabs-mode) t)
  (set (make-local-variable 'tab-width) 8))

(progn
  (add-hook 'asm-mode-hook 'cosmo-asm-supplemental-hook)
  (setq asm-font-lock-keywords cosmo-asm-font-lock-keywords))

;; Make -*-unix-assembly-*- mode line work correctly like GitHub.
(define-derived-mode unix-assembly-mode asm-mode "UNIX Assembly")

(provide 'cosmo-asm-mode)
