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

;; miniKanren: relational programming for plinko
;;
;; relations are goals which run forwards, backwards, and sideways. for
;; example (APPENDO X Y '(A B C)) finds every way to split a list.
;;
;; - logic variables are (? . G0) conses with a fresh gensym
;; - a substitution is an alist of (var . value) ending in the atom T,
;;   so NIL can be used to mean the unification failed
;; - a goal is a function from a substitution to a stream
;; - a stream is NIL, (substitution . stream), or (DELAY . thunk)

(DEFUN VAR ()
  (CONS '? (GENSYM)))

(DEFUN ISVAR (X)
  (AND (EQ (ATOM X)) (EQ (CAR X) '?)))

(DEFUN ASSV (V S)
  (AND (EQ (ATOM S))
       (IF (EQ (CDR V) (CDR (CAR (CAR S))))
           (CAR S)
         (ASSV V (CDR S)))))

(DEFUN WALK (U S)
  (LET ((P (AND (ISVAR U) (ASSV U S))))
    (IF P (WALK (CDR P) S) U)))

(DEFUN UNIFY (U V S)
  (LET ((U (WALK U S))
        (V (WALK V S)))
    (COND ((ISVAR U)
           (IF (AND (ISVAR V) (EQ (CDR U) (CDR V)))
               S
             (CONS (CONS U V) S)))
          ((ISVAR V)
           (CONS (CONS V U) S))
          ((ATOM U)
           (AND (EQ U V) S))
          ((ATOM V) NIL)
          ((LET ((S (UNIFY (CAR U) (CAR V) S)))
             (AND S (UNIFY (CDR U) (CDR V) S)))))))

(DEFUN == (U V)
  (LAMBDA (S)
    (LET ((S (UNIFY U V S)))
      (AND S (CONS S)))))

(DEFUN SUCCEED (S)
  (CONS S))

(DEFUN FAIL (S)
  NIL)

(DEFUN MPLUS (A B)
  (COND ((EQ A) B)
        ((EQ (CAR A) 'DELAY)
         (CONS 'DELAY (LAMBDA () (MPLUS B ((CDR A))))))
        ((CONS (CAR A) (MPLUS (CDR A) B)))))

(DEFUN BIND (A G)
  (COND ((EQ A) NIL)
        ((EQ (CAR A) 'DELAY)
         (CONS 'DELAY (LAMBDA () (BIND ((CDR A)) G))))
        ((MPLUS (G (CAR A)) (BIND (CDR A) G)))))

(DEFUN DISJ2 (G1 G2)
  (LAMBDA (S)
    (MPLUS (G1 S) (G2 S))))

(DEFUN CONJ2 (G1 G2)
  (LAMBDA (S)
    (BIND (G1 S) G2)))

;; (CONJ* g ...) succeeds when every goal succeeds
(DEFMACRO CONJ* G
  (COND ((EQ G) 'SUCCEED)
        ((EQ (CDR G)) (CAR G))
        (`(CONJ2 ,(CAR G) (CONJ* . ,(CDR G))))))

;; (DISJ* g ...) succeeds when any goal succeeds
(DEFMACRO DISJ* G
  (COND ((EQ G) 'FAIL)
        ((EQ (CDR G)) (CAR G))
        (`(DISJ2 ,(CAR G) (DISJ* . ,(CDR G))))))

;; wraps goal in a delay, so recursive relations don't loop forever
(DEFMACRO ZZZ (G)
  `(LAMBDA (-S)
     (CONS 'DELAY (LAMBDA () (,G -S)))))

;; returns (x (VAR)) bindings for each variable name in VS
(DEFUN -FRESH (VS)
  (MAPCAR (LAMBDA (V) (LIST V '(VAR))) VS))

;; (FRESH (x ...) g ...) introduces new logic variables
(DEFMACRO FRESH (VS . GS)
  `(ZZZ (LET ,(-FRESH VS) (CONJ* . ,GS))))

;; (CONDE (g ...) ...) tries each clause, interleaving their answers
(DEFMACRO CONDE CS
  `(ZZZ (DISJ* . ,(MAPCAR (LAMBDA (C) (CONS 'CONJ* C)) CS))))

(DEFUN PULL (A)
  (IF (AND A (EQ (CAR A) 'DELAY))
      (PULL ((CDR A)))
    A))

(DEFUN TAKE-ALL (A)
  (LET ((A (PULL A)))
    (AND A (CONS (CAR A) (TAKE-ALL (CDR A))))))

;; takes as many answers as there are elements in the list N
(DEFUN TAKE (N A)
  (AND N (LET ((A (PULL A)))
           (AND A (CONS (CAR A) (TAKE (CDR N) (CDR A)))))))

(DEFUN WALK* (V S)
  (LET ((V (WALK V S)))
    (IF (OR (ATOM V) (ISVAR V))
        V
      (CONS (WALK* (CAR V) S)
            (WALK* (CDR V) S)))))

;; names unbound variables _0, _1, etc. in order of appearance
;; R is (substitution . unused-names)
(DEFUN -REIFY (V R)
  (LET ((V (WALK V (CAR R))))
    (COND ((ISVAR V)
           (CONS (CONS (CONS V (CAR (CDR R))) (CAR R))
                 (CDR (CDR R))))
          ((ATOM V) R)
          ((-REIFY (CDR V) (-REIFY (CAR V) R))))))

(DEFUN REIFY (V S)
  (LET ((V (WALK* V S)))
    (WALK* V (CAR (-REIFY V (CONS T '(_0 _1 _2 _3 _4 _5 _6 _7 _8 _9
                                     _10 _11 _12 _13 _14 _15)))))))

(DEFUN -RUN-QUERY (VS)
  (IF (CDR VS) (CONS 'LIST VS) (CAR VS)))

;; (RUN* (q ...) g ...) returns every answer
(DEFMACRO RUN* (VS . GS)
  `(LET ,(-FRESH VS)
     (MAPCAR (LAMBDA (-S) (REIFY ,(-RUN-QUERY VS) -S))
             (TAKE-ALL ((CONJ* . ,GS) T)))))

;; (RUN n (q ...) g ...) returns the first n answers, where n is 1-9
(DEFMACRO RUN (N VS . GS)
  `(LET ,(-FRESH VS)
     (MAPCAR (LAMBDA (-S) (REIFY ,(-RUN-QUERY VS) -S))
             (TAKE ',(CDR (ASSOC N '((1 T) (2 T T) (3 T T T) (4 T T T T)
                                     (5 T T T T T) (6 T T T T T T)
                                     (7 T T T T T T T) (8 T T T T T T T T)
                                     (9 T T T T T T T T T))))
                   ((CONJ* . ,GS) T)))))

;; (_) is a fresh logic variable, for parts of a term we don't care about
(DEFUN _ ()
  (VAR))

;; L appended to S is O
(DEFUN APPENDO (L S O)
  (CONDE ((== L NIL) (== S O))
         ((FRESH (A D R)
            (== (CONS A D) L)
            (== (CONS A R) O)
            (APPENDO D S R)))))

;; X is an element of L
(DEFUN MEMBERO (X L)
  (FRESH (A D)
    (== (CONS A D) L)
    (CONDE ((== A X))
           ((MEMBERO X D)))))

;; X comes immediately before Y in L
(DEFUN LEFTO (X Y L)
  (FRESH (A D)
    (== (CONS A D) L)
    (CONDE ((== A X) (FRESH (E) (== (CONS Y E) D)))
           ((LEFTO X Y D)))))

;; X and Y are adjacent in L
(DEFUN NEXTO (X Y L)
  (CONDE ((LEFTO X Y L))
         ((LEFTO Y X L))))
