#| plinko - a lisp tarpit
 |
 | To the extent possible under law, Justin Tunney has waived
 | all copyright and related or neighboring rights to this file,
 | as it is written in the following disclaimers:
 |
 |   - http://unlicense.org/
 |   - http://creativecommons.org/publicdomain/zero/1.0/
 |
 |#

;; tests for macro expansion

;; macros receive their arguments as written
(DEFMACRO EX-QUOTE (X) (LIST 'QUOTE X))
(TEST '(AND A B) (EX-QUOTE (AND A B)))
(TEST '(EX-QUOTE X) (EX-QUOTE (EX-QUOTE X)))

;; so backquote templates may contain macros
(DEFMACRO EX-LET1 (V X B)
  `(LET ((,V ,X)) ,B))
(TEST '(A . A) (EX-LET1 Q 'A (CONS Q Q)))
(DEFMACRO EX-LETN (BS B)
  `(LET ,BS ,B))
(TEST '(A . B) (EX-LETN ((X 'A) (Y 'B)) (CONS X Y)))
(DEFMACRO EX-WHEN (P . B)
  `(AND ,P (PROGN . ,B)))
(TEST 'YES (EX-WHEN T 'YES))
(TEST NIL (EX-WHEN NIL 'YES))

;; whatever a macro returns gets expanded too
(DEFMACRO EX-TWICE (X) `(EX-LET1 Y ,X (CONS Y Y)))
(TEST '(B . B) (EX-TWICE 'B))

;; lambda parameters shadow global macros within their bodies
(DEFINE EX-P (LAMBDA (AND) (AND 'X)))
(TEST '(X . X) (EX-P (LAMBDA (X) (CONS X X))))
(TEST 'C (LET ((IF (LAMBDA (A B C) C))) (IF 'A 'B 'C)))
(DEFINE EX-D (LAMBDA ((AND . OR) X) (OR (AND X))))
(TEST '((Q)) (EX-D (CONS (LAMBDA (X) (CONS X)) (LAMBDA (X) (CONS X))) 'Q))
(DEFINE EX-V (LAMBDA AND AND))
(TEST '(A B) (EX-V 'A 'B))

;; but only within their bodies
(DEFINE EX-OUT (LAMBDA (X) (LIST ((LAMBDA (AND) (AND X)) CONS) (AND X 'Y))))
(TEST '((Z) Y) (EX-OUT 'Z))

;; cond clauses are lists of expressions, not function calls
(DEFINE EX-C (LAMBDA (AND) (COND (AND 'YES) ('NO))))
(TEST 'YES (EX-C T))
(TEST 'NO (EX-C NIL))

;; quoted data is left alone
(TEST '(LET ((X 1)) X) '(LET ((X 1)) X))
