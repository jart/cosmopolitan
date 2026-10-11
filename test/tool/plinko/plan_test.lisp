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

;; tests for the planner, which specializes expressions the first time
;; they're evaluated. plinko_test runs this both normally and with -P,
;; which disables those optimizations, so both paths must agree

;; a local binding must win over a global of the same name, whether
;; it's stored in p1, p2, or the association list
(DEFINE PT-F (LAMBDA (X) 'GLOBAL))
(TEST 'GLOBAL (PT-F 'A))
(DEFINE PT-P1 (LAMBDA (PT-F X) (PT-F X)))
(TEST 'LOCAL (PT-P1 (LAMBDA (X) 'LOCAL) 'A))
(DEFINE PT-P2 (LAMBDA (X PT-F) (PT-F X)))
(TEST 'LOCAL (PT-P2 'A (LAMBDA (X) 'LOCAL)))
(DEFINE PT-P3 (LAMBDA (X Y PT-F) (PT-F X)))
(TEST 'LOCAL (PT-P3 'A 'B (LAMBDA (X) 'LOCAL)))
(TEST 'LOCAL ((LAMBDA (PT-F) (PT-F 'A)) (LAMBDA (X) 'LOCAL)))
(TEST 'LOCAL (LET ((PT-F (LAMBDA (X) 'LOCAL))) (PT-F 'A)))
(TEST 'GLOBAL (PT-F 'A))

;; a local closure must not be baked in, since each call can pass a
;; different function, including ones that are global definitions
(DEFINE PT-G1 (LAMBDA (X) 'ONE))
(DEFINE PT-G2 (LAMBDA (X) 'TWO))
(DEFINE PT-APPLY (LAMBDA (F X) (F X)))
(TEST 'ONE (PT-APPLY PT-G1 'A))
(TEST 'TWO (PT-APPLY PT-G2 'A))
(DEFINE PT-APPLY2 (LAMBDA (X Y F) (F X)))
(TEST 'ONE (PT-APPLY2 'A 'B PT-G1))
(TEST 'TWO (PT-APPLY2 'A 'B PT-G2))

;; closures capture local functions
(DEFINE PT-MAKE (LAMBDA (F) (LAMBDA (X) (F X))))
(TEST 'ONE ((PT-MAKE PT-G1) 'A))
(TEST 'TWO ((PT-MAKE PT-G2) 'A))

;; the reader shares code like (F) between unrelated places
(DEFINE F (LAMBDA () 'GLOBAL))
(DEFINE PT-CALLF (LAMBDA () (F)))
(TEST 'GLOBAL (PT-CALLF))
(DEFINE PT-CALLLOCAL (LAMBDA (F) (F)))
(TEST 'LOCAL (PT-CALLLOCAL (LAMBDA () 'LOCAL)))
(TEST 'GLOBAL (PT-CALLF))

;; ((λ ⊥ 𝑦)) must evaluate 𝑦
(TEST '(A . B) ((LAMBDA () (CONS 'A 'B))))
(TEST '(Q . Q) ((LAMBDA (X) ((LAMBDA () (CONS X X)))) 'Q))

;; ((λ (𝑣) 𝑦) 𝑧) with shadowing
(TEST '(A . A) ((LAMBDA (X) (CONS X X)) 'A))
(TEST 'INNER ((LAMBDA (X) ((LAMBDA (X) X) 'INNER)) 'OUTER))
(TEST '(INNER . OUTER)
      ((LAMBDA (X Y) ((LAMBDA (X) (CONS X Y)) 'INNER)) 'IGNORED 'OUTER))

;; lambdas with zero, one, two, three, variadic, and destructured args
(DEFINE PT-A0 (LAMBDA () 'ZERO))
(DEFINE PT-A1 (LAMBDA (X) (CONS X)))
(DEFINE PT-A2 (LAMBDA (X Y) (CONS X Y)))
(DEFINE PT-A3 (LAMBDA (X Y Z) (LIST X Y Z)))
(DEFINE PT-AV (LAMBDA A A))
(DEFINE PT-AR (LAMBDA (X . R) (CONS R X)))
(DEFINE PT-AD (LAMBDA ((X . Y) Z) (LIST Y X Z)))
(TEST 'ZERO (PT-A0))
(TEST '(A) (PT-A1 'A))
(TEST '(A . B) (PT-A2 'A 'B))
(TEST '(A B C) (PT-A3 'A 'B 'C))
(TEST '(A B C) (PT-AV 'A 'B 'C))
(TEST '((B C) . A) (PT-AR 'A 'B 'C))
(TEST '(B A C) (PT-AD '(A . B) 'C))

;; a function keeps the global definitions it was defined with
(DEFINE PT-H (LAMBDA (X) 'OLD))
(DEFINE PT-USEH (LAMBDA (X) (PT-H X)))
(TEST 'OLD (PT-USEH 'A))
(DEFINE PT-H (LAMBDA (X) 'NEW))
(TEST 'OLD (PT-USEH 'A))
(TEST 'NEW (PT-H 'A))

;; recursion through the y combinator with one, two, and three args
(DEFUN PT-LEN1 (X)
  (AND X (CONS 'I (PT-LEN1 (CDR X)))))
(DEFUN PT-REV2 (X R)
  (IF X (PT-REV2 (CDR X) (CONS (CAR X) R)) R))
(DEFUN PT-ZIP3 (X Y R)
  (IF (AND X Y)
      (PT-ZIP3 (CDR X) (CDR Y) (CONS (CONS (CAR X) (CAR Y)) R))
    (REVERSE R)))
(TEST '(I I I) (PT-LEN1 '(A B C)))
(TEST '(C B A) (PT-REV2 '(A B C)))
(TEST '((A . X) (B . Y)) (PT-ZIP3 '(A B C) '(X Y)))

;; recursive function whose parameter shadows a global function
(DEFUN PT-MAPF (PT-F X)
  (AND X (CONS (PT-F (CAR X)) (PT-MAPF PT-F (CDR X)))))
(TEST '(LOCAL LOCAL) (PT-MAPF (LAMBDA (X) 'LOCAL) '(A B)))

;; two clause conds get turned into if statements
(DEFUN PT-IF (X)
  (COND ((ATOM X) 'ATOM)
        ('PAIR)))
(TEST 'ATOM (PT-IF 'A))
(TEST 'PAIR (PT-IF '(A)))
(DEFUN PT-COND (X)
  (COND ((CDR X))
        ((CAR X))))
(TEST '(B) (PT-COND '(A B)))
(TEST 'A (PT-COND '(A)))
(TEST NIL (PT-COND NIL))

;; car and cdr chains, including on atoms and nil
(TEST NIL (CAR (CDR (CAR NIL))))
(TEST NIL (CDR (CDR (CDR (CDR NIL)))))
(TEST 'C (CAR (CDR (CDR '(A B C)))))
(TEST 'B (CAR (CAR (CDR '(A (B C))))))
