(defconst ctest-operators
  '("+"
    "-"
    "*"
    "/"
    "%"
    "//"
    "**"
    "~"
    "^"
    "|"
    "&"
    ">>"
    "<<"
    "!"
    "||"
    "&&"
    "="
    "!="
    "<"
    ">"
    "<="
    ">="))

(defconst ctest-consts
  '("false"
    "true"
    "e"
    "pi"
    "epsilon"
    "inf"
    "nan"
    "l2t"
    "lg2"
    "ln2"
    "l2e"))

(defconst ctest-builtins
  '("emit"
    "exit"
    "meminfo"
    "isnan"
    "isinf"
    "signbit"
    "isfinite"
    "isnormal"
    "fpclassify"))

(defconst ctest-functions
  '("emit"
    "cr"
    "key"
    "dup"
    "swap"
    "over"
    "drop"
    "expect"
    "assert"
    "abs"
    "min"
    "max"))

(defconst ctest-functions-libm
  '("mod"
    "rem"
    "gray"
    "ungray"
    "popcnt"
    "sqrt"
    "exp"
    "exp2"
    "exp10"
    "ldexp"
    "log"
    "log2"
    "log10"
    "sin"
    "cos"
    "tan"
    "asin"
    "acos"
    "atan"
    "atan2"
    "round"
    "trunc"
    "rint"
    "nearbyint"
    "ceil"
    "floor"
    "rand"
    "rand32"
    "rand64"))

(defun ctest--make-regex (words)
  (concat "\\_<" (regexp-opt words) "\\_>"))

(defconst ctest-highlights
  `(("\\(#.*\\)$" 1 font-lock-comment-face)
    ("\\b0\\([xX]\\)[[:xdigit:]]+\\([ulUL]*\\)\\b"
     (1 font-lock-constant-face)
     (2 font-lock-constant-face))
    ("\\b0\\([bB]\\)[01]+\\([ulUL]*\\)\\b"
     (1 font-lock-constant-face)
     (2 font-lock-constant-face))
    (,(concat "\\_<" (regexp-opt ctest-consts) "\\_>") .
     font-lock-constant-face)
    (,(concat "\\_<" (regexp-opt ctest-builtins) "\\_>") .
     font-lock-builtin-face)))

(define-derived-mode ctest-mode fundamental-mode "CALCULATOR.COM"
  "Major mode for editing CALCULATOR.COM smoke tests."
  (setq font-lock-defaults '(ctest-highlights)))

(add-to-list 'auto-mode-alist '("\\.ctest$" . ctest-mode))

(provide 'ctest-mode)
