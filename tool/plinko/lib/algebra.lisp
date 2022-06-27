#| plinko - a really fast lisp tarpit
 | Copyright 2022 Justine Alexandra Roberts Tunney
 |
 | Permission to use, copy, modify, and/or distribute this software for
 | any purpose with or without fee is hereby granted, provided that the
 | above copyright notice and this permission notice appear in all copies.
 |
 | THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 | WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 | WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 | AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 | DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 | PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 | TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 | PERFORMANCE OF THIS SOFTWARE.
 |#

(DEFINE EXPLAIN NIL)

(DEFUN SUBST (X A)
  (AND X (IF (ATOM X)
             (LET ((R (ASSOC X A)))
               (IF R (CDR R) X))
           (CONS (SUBST (CAR X) A)
                 (SUBST (CDR X) A)))))

(DEFUN ISCONSTANT (X)
  (EQ X 'K))

(DEFUN ISVARIABLE (X)
  (OR (EQ X 'A)
      (EQ X 'B)
      (EQ X 'C)
      (EQ X 'TODO)))

(DEFUN ISDIGLET (X)
  (AND (>= X '0)
       (<= X '9)))

(DEFUN ISCOMMUTATIVE (X)
  (OR (EQ X 'ADD)
      (EQ X '*)
      (EQ X 'XOR)
      (EQ X 'OR)
      (EQ X 'AND)
      (EQ X 'NAND)
      (EQ X 'EQ)
      (EQ X 'EQUAL)
      (EQ X 'MIN)
      (EQ X 'MAX)))

(DEFUN RIGHTEN (X)
  (IF (ATOM X) X
    (KEEP X (CONS (CAR X)
                  (LET ((A (RIGHTEN (CADR X))))
                    (IF (CDDR X)
                        (LET ((B (RIGHTEN (CADDR X))))
                          (IF (AND (ISCOMMUTATIVE (CAR X))
                                   (< B A))
                              (LIST B A)
                            (LIST A B)))
                      (LIST A)))))))

(DEFUN MATCHES (X P A)
  (COND ((ATOM P)
         (IF (AND P (OR (ISVARIABLE P)
                        (AND (ISCONSTANT P)
                             (ISDIGLET X))))
             (LET ((Y (ASSOC P A)))
               (IF Y (AND (EQUAL X (CDR Y)) A)
                 (CONS (CONS P X) A)))
           (AND (EQ P X) A)))
        ((NOT (ATOM X))
         (LET ((R (MATCHES (CAR X) (CAR P) A)))
           (AND R (MATCHES (CDR X) (CDR P) R))))))

(DEFUN -SIMPNODE (X P)
  (AND P (LET ((R (MATCHES X (CAAR P) '(NIL))))
           (IF R (IF EXPLAIN
                     (PRINT 'REDUCE X
                            'BECAUSE (CAAR P) 'IS (CADAR P)
                            'THEREFORE (RIGHTEN (SUBST (CADAR P) R)))
                   (SUBST (CADAR P) R))
             (-SIMPNODE X (CDR P))))))

(DEFUN SIMPNODE (X P)
  (AND P (LET ((R (AND (EQ (CAR X) (CAAR P))
                       (-SIMPNODE X (CDAR P)))))
           (OR R (SIMPNODE X (CDR P))))))

(DEFUN SIMPTREE (X P H)
  (IF (ATOM X) X
    (IF (NOT (MEMBER X H))
        (LET* ((H (CONS X H))
               (B (SIMPTREE (CADDR X) P H))
               (C (CONS (CAR X)
                        (CONS (SIMPTREE (CADR X) P H)
                              (AND B (CONS B)))))
               (D (SIMPNODE C P)))
          (IF D
              (SIMPTREE D P H)
            (KEEP X C)))
      (ERROR 'SIMPTREE 'CYCLE X 'HISTORY H))))

(DEFUN PERMCOMB (OP ARG1 ARG2 RES)
  (IF ARG2
      (PERMCOMB
       OP ARG1 (CDR ARG2)
       (PEEL
        (LIST OP (CAR ARG2) ARG1)
        (PEEL
         (LIST OP ARG1 (CAR ARG2))
         RES)))
    RES))

(DEFUN PERMCOMA (OP ARG1 ARG2 RES)
  (IF ARG1
      (PERMCOMA
       OP (CDR ARG1) ARG2
       (PERMCOMB
        OP (CAR ARG1) ARG2 RES))
    RES))

(DEFUN PERMCOMM (X)
  (IF (AND (NOT (ATOM X))
           (ISCOMMUTATIVE (CAR X)))
      (PERMCOMA
       (CAR X)
       (PERMCOMM (CADR X))
       (PERMCOMM (CADDR X)))
    (CONS X)))

(DEFUN -RULES (((A B) . P) R)
  (IF A (-RULES P (CONS (REVERSE-DOLIST (E (PERMCOMM (RIGHTEN A))) (LIST E B)) R))
    (REDUCE APPEND (REVERSE R))))

(DEFUN RULES (L R)
  (GROUPBY
   (LAMBDA (X) (CAAR X))
   (-RULES L R)))

(DEFINE LOLGEBRA
  (RULES
   '(((ADD 1 2) 3)
     ((ADD 1 3) 4)
     ((ADD 1 4) 5)
     ((ADD A 0) A)
     ((ADD (SUB A B) B) A)         ;; COMMUTATIVE PROPERTY
     ((ADD A A) (* 2 A))
     ((ADD A (SUB B A)) B)
     ((ADD (SUB A B) B) A)
     ((ADD (ADD A B) (SUB C A)) (ADD C B))
     ((ADD (SUB A B) (SUB C A)) (SUB C B))
     ((ADD (* A B) (* A B)) (* 2 (* A B)))
     ((ADD (* A B) (* B (ADD A C))) (* B (ADD (* 2 A) C)))
     ((ADD (POW A 2) (* (POW A 2) B)) (* (POW A 2) (ADD B 1)))
     ((ADD A (* A K)) (* A (ADD K 1)))
     ((ADD A (ADD K B)) (ADD K (ADD A B)))
     ((ADD A (SUB K B)) (ADD K (SUB A B)))
     ((ADD (* B A) (* C A)) (* A (ADD B C)))
     ((ADD (SUB 0 A) B) (SUB B A))
     ((ADD A (SUB 0 B)) (SUB A B))
     ((ADD (NOT A) 1) (SUB 0 A))
     ((ADD (* (SUB 0 B) C) A) (SUB A (* B C)))
     ((ADD (XOR A (NOT B)) (* 2 (OR A B))) (SUB (ADD A B) 1))
     ((ADD (ADD (ADD A B) 1) (NOT (OR A B))) (OR A B))
     ((ADD (SUB A B) (AND (NOT (* 2 A)) (* 2 B))) (XOR A B))
     ((ADD (SUB 0 B) (AND (NOT (* 2 A)) (* 2 B))) (SUB (XOR A B) A))
     ((ADD (SUB 0 B) (* 2 (AND (NOT A) B))) (SUB (XOR A B) A))
     ((ADD (SUB A B) (* 2 (AND (NOT A) B))) (XOR A B))
     ((ADD (AND A B) (OR A B)) (ADD A B))
     ((ADD (XOR A B) (* 2 (AND A B))) (ADD A B))
     ((ADD (SUB (SUB 0 A) B) (* 2 (OR A B))) (XOR A B))
     ((ADD (* (SUB 0 2) (AND (NOT A) B)) B) (ADD (SUB 0 (XOR A B)) A))
     ((ADD (ADD A B) (NOT (AND A B))) (SUB (AND A B) 1))
     ((ADD (ADD A B) (* 2 (NOT (OR A B)))) (SUB (XOR A B) 2))
     ((ADD (DIV A C) (DIV B C)) (DIV (ADD A B) C))
     ((ADD (AND A B) (XOR A B)) (OR A B))
     ((ADD (AND A B) (OR A B)) (ADD A B))
     ((ADD (SUB (ADD A B) (* 2 (OR A B))) (XOR A B)) 0)
     ((ADD (NOT A) A) (SUB 0 1))
     ((ADD (AND A (NOT B)) (AND (NOT A) B)) (XOR A B))
     ((ADD (SUB (AND A (NOT B)) (AND (NOT A) B)) C) (ADD (SUB A B) C))
     ((ADD (POW A 2)
           (ADD (* 2 (* A B))
                (POW B 2)))
      (POW (ADD A B) 2)) ;; BINOMIAL THEOREM
     ((ADD (ADD (POW A 3)
                (ADD (* 3 (* B (POW A 2)))
                     (* 3 (* A (POW B 2)))))
           (POW B 3))
      (POW (ADD A B) 3)) ;; BINOMIAL THEOREM

     ((SUB 1 1) 0)
     ((SUB 2 1) 1)
     ((SUB 3 1) 2)
     ((SUB 4 1) 3)
     ((SUB 5 1) 4)
     ((SUB 1 2) (SUB 0 1))
     ((SUB 2 2) 0)
     ((SUB 3 2) 1)
     ((SUB 4 2) 2)
     ((SUB 5 2) 3)
     ((SUB A A) 0) ;; UNSAFE WITH NANS
     ((SUB A 0) A)
     ((SUB (ADD A B) B) A)         ;; COMMUTATIVE PROPERTY
     ((SUB 0 (ADD A 1)) (NOT A))
     ((SUB (POW A 2) (POW B 2)) (* (ADD A B) (SUB A B))) ;; DIFFERENCE OF TWO SQUARES
     ((SUB (SUB (* A (ADD B A)) (* A B)) (POW B 2)) (* (ADD A B) (SUB A B))) ;; DIFFERENCE OF TWO SQUARES
     ((SUB (ADD A B) A) B)
     ((SUB (ADD A B) B) A)
     ((SUB A (SUB A B)) B)
     ((SUB A (ADD B A)) (SUB 0 B))
     ((SUB A (ADD A B)) (SUB 0 B))
     ((SUB (SUB A B) A) (SUB 0 B))
     ((SUB (SUB A B) A) (SUB 0 B))
     ((SUB (ADD A B) (SUB A C)) (ADD B C))
     ((SUB (DIV 1 2) 1) (SUB 0 (DIV 1 2))) ;; SQRT DERIVATIVE HACK
     ((SUB A (SUB K B)) (SUB (ADD A B) K))
     ((SUB (ADD K A) B) (ADD K (SUB A B)))
     ((SUB (DIV (LOG A) (LOG C)) (DIV (LOG B) (LOG C))) (DIV (LOG (DIV A B)) (LOG C))) ;; DOMAIN IS 0<C<1 ∨ C>1
     ((SUB 0 (NOT A)) (ADD A 1))
     ((SUB 0 (SUB A B)) (SUB B A))
     ((SUB A (XOR 0 1)) (SUB A 1))
     ((SUB (SUB 0 1) A) (NOT A))
     ((SUB (OR A B) (AND A (NOT B))) B)
     ((SUB (SUB 0 (XOR A (NOT B))) (* 2 (OR A B))) (ADD (SUB (SUB 0 A) B) 1))
     ((SUB (SUB 0 A) (AND (NOT (* 2 A)) (* 2 B))) (SUB (SUB 0 (XOR A B)) B))
     ((SUB (ADD A B) (* 2 (AND A B))) (XOR A B))
     ((SUB (ADD A B) (OR A B)) (AND A B))
     ((SUB (AND A B) (OR (NOT A) B)) (ADD A 1))
     ((SUB (OR A B) (AND A B)) (XOR A B))
     ((SUB (AND (* 2 (NOT A)) (* 2 B)) B) (SUB (XOR A B) A))
     ((SUB (DIV A C) (DIV B C)) (DIV (SUB A B) C))
     ;; ((SUB A (* (DIV A B) B)) (REM A B))  ;; ONLY FOR INTEGER ARITHMETIC
     ((SUB (ADD A B) (OR A B)) (AND A B))
     ((SUB (ADD A B) (AND A B)) (OR A B))
     ((SUB (OR A B) (XOR A B)) (AND A B))
     ((SUB (OR A B) (AND A B)) (XOR A B))
     ((SUB B (AND A B)) (AND (NOT A) B))
     ((SUB A (AND A B)) (AND A (NOT B)))
     ((SUB (SUB 0 (AND A B)) 1) (NOT (AND A B)))
     ((SUB (SUB (AND A B) A) 1) (OR (NOT A) B))
     ((SUB (SUB (AND A B) B) 1) (OR A (NOT B)))
     ((SUB (ADD (SUB (SUB 0 A) B) (AND A B)) 1) (NOT (OR A B)))
     ((SUB (ADD (SUB (SUB 0 A) B) (* 2 (AND A B))) 1) (NOT (XOR A B)))
     ((SUB (SUB 0 A) 1) (NOT A))
     ((SUB (AND A (NOT B)) (AND A B)) (SUB (XOR A B) B))
     ((SUB (AND A B) (AND A (NOT B))) (SUB B (XOR A B)))
     ((SUB (AND A B) (AND A (NOT B))) (SUB B (XOR A B)))

     ((POW A 0) 1)
     ((POW 0 A) 0)
     ((POW 1 A) 1)
     ((POW A 1) A)
     ((POW 2 2) 4)
     ((POW 3 2) 9)
     ((POW 4 (DIV 1 2)) 2)
     ((POW 8 (DIV 1 3)) 2)
     ((POW A (SUB 0 1)) (DIV 1 A))
     ((POW (ABS A) B) (POW A B))
     ((POW A (SUB 0 B) 1) (DIV 1 (POW A B))) ;; THIS NORMALIZATION
     ((POW (POW K A) (DIV 1 B)) (POW (POW K (DIV 1 B)) A)) ;; COMES BEFORE NEXT
     ((POW (POW A B) C) (POW A (* B C)))      ;; COMES AFTER PREV
     ((POW B (DIV (LOG A) (LOG B))) A)
     ((POW (* K K) (DIV 1 2)) K)
     ((POW (POW A 2) (DIV 1 2)) (ABS A))
     ((POW (* K (POW A 2)) (DIV 1 2)) (* (ABS A) (POW K (DIV 1 2)))) ;; DOMAIN K≥0

     ((DIV A 0) :DBZERO)
     ((DIV A A) 1)
     ((DIV A 1) A)
     ((DIV 0 (- 0 K)) (- 0 0)) ;; SIGNED ZERO LOL
     ((DIV 0 A) 0)
     ((DIV (SUB 0 A) A) (SUB 0 1))
     ((DIV A (SUB 0 1)) (SUB 0 A))
     ((DIV (DIV A B) C) (DIV A (* B C)))
     ((DIV A (DIV B C)) (* (DIV A B) C))
     ((DIV A (SUB 0 B)) (DIV (SUB 0 A) B)) ;; SHOULD WE?
     ((DIV (* A 2) 2) A)
     ((DIV (SIN A) (TAN A)) (COS A))
     ((DIV (SIN A) (COS A)) (TAN A))
     ((DIV (COS A) (SIN A)) (DIV 1 (TAN A)))
     ((DIV (TAN A) (SIN A)) (DIV 1 (COS A)))
     ((DIV (POW A C) (POW A B)) (POW A (SUB C B))) ;; EXPONENT RULE
     ((DIV (POW A B) (POW A C)) (POW A (SUB C B))) ;; EXPONENT RULE
     ((DIV (* B (POW A (SUB B 1))) (POW A B)) (DIV B A)) ;; UGLY
     ;; ((DIV A (POW B C)) (* A (POW B (SUB 0 C)))) ;; EXPONENT RULE [CYCLIC]
     ((DIV (POW A B) (POW C B)) (POW (DIV A C) B)) ;; DOMAIN IS APPROXIMATELY ABC≥1
     ((DIV (LOG (POW B A)) (LOG B)) A)
     ((DIV (LOG (POW A B)) (LOG C)) (* B (DIV (LOG A) (LOG C))))
     ((DIV (* A B) (* A C)) (DIV B C)) ;; CANCELLATION
     ((DIV (POW A (DIV 1 2)) (POW B (DIV 1 2))) (POW (DIV A B) (DIV 1 2))) ;; DOMAIN >0

     ((* A 0) 0) ;; UNSAFE WITH NANS
     ((* A 1) A)
     ((* A A) (POW A 2))
     ((* A (SUB 0 1)) (SUB 0 A))
     ((* A (DIV 1 B)) (DIV A B))
     ((* (POW A 2) A) (POW A 3))
     ((* (* K A) (* K B)) (* (* 2 K) (* A B)))
     ((* (* B A) A) (* (POW A 2) B)) ;; SHOULD NOT BE NEEDED
     ((* (* (POW A 2) B) A) (* (* (POW A 2) A) B))
     ;; ((* B (* K (POW A 2))) (* K (* B (POW A 2))))
     ((* (* C (* A B)) A) (* (* C (POW A 2)) B))
     ((* (POW A B) (POW A C)) (POW A (ADD B C))) ;; EXPONENT RULE
     ((* (* TODO (POW A B)) (POW A C)) (* TODO (POW A (ADD B C)))) ;; EXPONENT RULE
     ((* (POW A B) (POW C B)) (POW (* A C) B)) ;; DOMAIN IS APPROXIMATELY ABC≥0
     ((* A (POW A K)) (POW A (ADD K 1))) ;; NOTE: A=0∧K<0 IS DIVIDE ERROR
     ((* (DIV A B) (DIV B C)) (DIV A C)) ;; CANCELLATION
     ((* (DIV B A) A) B)                 ;; CANCELLATION
     ((* (POW A (DIV 1 2)) (POW A (DIV 1 2))) A)
     ((* (POW A (DIV 1 2)) (POW B (DIV 1 2))) (POW (* A B) (DIV 1 2))) ;; DOMAIN ≥0
     ((* (POW C A) (POW C B)) (POW C (ADD A B)))
     ;; ((* (DIV A 2) 2) (AND A (NOT 1)))  ;; ONLY FOR UNSIGNED INTEGER ARITHMETIC
     ;; ((* (DIV A B) C) (DIV (* A C) B))  ;; EVIL

     ((REM 0 A) 0)
     ((REM A A) 0)
     ((REM A 1) 0) ;; INTEGER ONLY
     ((REM A (SUB 0 1)) 0)
     ((REM (REM A B) B) (REM A B))
     ((REM A (SUB 0 B)) (REM A B)) ;; REMAINDER ONLY; NOT MODULUS

     ((AND A A) A)
     ((AND (LT A B) (LT B C)) (LT A C)) ;; TRANSITIVE PROPERTY
     ((AND A 0) 0)
     ((AND A (NOT A)) 0)
     ((AND (ADD (* 2 A) 1) (* 2 B)) (AND (* 2 A) (* B 2)))
     ((AND (NOT A) (NOT B)) (NOT (AND A B))) ;; DE MORGAN'S LAW
     ((AND A (NOT 0)) A)
     ((AND A (NOT (AND A B))) (AND A (NOT B)))
     ((AND (OR A B) (NOT A)) (AND B (NOT A)))
     ((AND (OR A B) (NOT (AND A B))) (XOR A B))
     ((AND (OR A B) (XOR (NOT A) B)) (AND A B))
     ((AND (OR (NOT A) B) (OR A (NOT B))) (NOT (XOR A B)))
     ((AND (NOT A) (NOT B)) (NOT (OR A B)))
     ((AND (XOR A B) B) (AND (NOT A) B))
     ((AND (AND A B) B) (AND A B))
     ((AND (AND A B) (AND A C)) (AND (AND A B) C))
     ((AND (OR A B) (NOT (XOR A B))) (AND A B))

     ((OR A A) A)
     ((OR A 0) A)
     ((OR (NOT A) (NOT B)) (NOT (OR A B))) ;; DE MORGAN'S LAW
     ((OR A (NOT (OR A B))) (OR A (NOT B)))
     ((OR (AND (NOT A) B) (NOT (OR A B))) (NOT A))
     ((OR (AND A B) (NOT (OR A B))) (NOT (XOR A B)))
     ((OR (XOR A B) (NOT (OR A B))) (NOT (AND A B)))
     ((OR (AND A B) (NOT A)) (OR B (NOT A)))
     ((OR (AND A B) (XOR A B)) (OR A B))
     ((OR (NOT A) (NOT B)) (NOT (AND A B)))
     ((OR (OR A B) B) (OR A B))
     ((OR (OR A B) (OR A C)) (OR (OR A B) C))
     ((OR (NOT A) A) (SUB 0 1))
     ((OR A (NOT 0)) (NOT 0))
     ((OR (AND A (NOT B)) (AND (NOT A) B)) (XOR A B))
     ((OR (XOR A B) A) (OR A B))
     ((OR A (NOT (XOR A B))) (OR A (NOT B)))
     ((OR (AND A B) (NOT (XOR A B))) (NOT (XOR A B)))
     ((OR (OR A B) (AND A B)) (OR A B))
     ((OR (OR A B) (XOR A B)) (OR A B))

     ((XOR A A) 0)
     ((XOR (OR A B) (OR A (NOT B))) (NOT A))
     ((XOR (OR (NOT A) B) (XOR A B)) (OR A (NOT B)))
     ((XOR (XOR A B) (OR A B)) (AND A B))
     ((XOR (AND A B) (XOR A B)) (OR A B))
     ((XOR (AND A B) (OR A B)) (XOR A B))
     ((XOR (OR (NOT A) B) (OR A (NOT B))) (XOR A B))
     ((XOR (OR A B) A) (AND B (NOT A)))
     ((XOR (NOT A) (NOT B)) (XOR A B))
     ((XOR (NOT A) B) (XOR A (NOT B)))
     ((XOR (AND A B) B) (AND (NOT A) B))
     ((XOR (XOR A B) B) A)
     ((XOR (XOR A B) (XOR A C)) (XOR B C))
     ((XOR A (NOT 0)) (NOT A))
     ((XOR (NOT A) A) (SUB 0 1))
     ((XOR (AND A (NOT B)) (AND (NOT A) B)) (XOR A B))
     ((XOR (AND A (NOT B)) (NOT A)) (NOT (AND A B)))

     ((NOT (SAR (NOT A) B)) (SAR A B))
     ((NOT (SUB 0 A)) (SUB A 1))
     ((NOT (ADD A (SUB 0 1))) (SUB 0 A))
     ((NOT (ADD (NOT A) B)) (SUB A B))
     ((NOT (AND (NOT A) B)) (OR A (NOT B)))
     ((NOT (OR (NOT A) B)) (AND A (NOT B)))
     ((NOT (ROR (NOT A) B)) (ROR A B))
     ((NOT (ROL (NOT A) B)) (ROL A B))

     ((MIN A A) A)
     ((MIN (MAX A B) B) B)
     ((MIN A (SUB 0 A)) (SUB 0 (ABS A)))
     ((MIN (NOT A) (NOT B)) (NOT (MAX A B)))

     ((MAX A A) A)
     ((MAX (MIN A B) B) B)
     ((MAX A (SUB 0 A)) (ABS A))
     ((MAX (NOT A) (NOT B)) (NOT (MIN A B)))

     ((SIN (ATAN A)) (DIV A (POW (ADD (POW A 2) 1) (DIV 1 2))))
     ((SIN (ASIN A)) A)
     ((COS (SUB 0 A)) (COS A))
     ((COS (ABS A)) (COS A))
     ((COS (ABS A)) (COS A))
     ((COS (ATAN A)) (DIV 1 (POW (ADD (POW A 2) 1) (DIV 1 2))))
     ((COS (ACOS A)) A)
     ((COS (ASIN A)) (POW (SUB 1 (POW A 2)) (DIV 1 2)))
     ((TAN (ATAN A)) A)
     ((SINH (ATANH A)) (DIV A (POW (* (SUB 1 A) (ADD 1 A)) (DIV 1 2))))
     ((COSH (ATANH A)) (DIV 1 (POW (* (SUB 1 A) (ADD 1 A)) (DIV 1 2))))
     ((CONJ (CONJ A)) A)

     ((ABS K) K)
     ((LOG 1) 0)
     ((SHL A 0) A)
     ((SAR A 0) A)
     ((SHR A 0) A)
     ((ROR A 0) A)
     ((ROL A 0) A)
     ((NEG A) (SUB 0 A))
     ((PLUS A B) (ADD A B))
     ((TIMES A B) (* A B))
     ((SQR A) (POW A 2))
     ((EXP A) (POW E A))
     ((EXP2 A) (POW 2 A))
     ((EXPT A B) (POW A B))
     ((LN A) (LOG A))
     ((LOG2 A) (DIV (LOG A) (LOG 2)))
     ((LOG10 A) (DIV (LOG A) (LOG 10)))
     ((LOGN A B) (DIV (LOG A) (LOG B)))
     ((HYPOT A B) (POW (ADD (POW A 2) (POW B 2)) (DIV 1 2)))
     ((ACOSH A) (DIV (LOG (ADD A (POW (SUB (POW A 2) 1) (DIV 1 2)))) (LOG E)))
     ((ASINH A) (DIV (LOG (ADD A (POW (ADD (POW A 2) 1) (DIV 1 2)))) (LOG E)))
     ((ACOSH A) (DIV (LOG (ADD A (POW (SUB (POW A 2) 1) (DIV 1 2)))) (LOG E))) ;; DOMAIN A≥1
     ;; ((ATANH A) (LOGN (* (DIV 1 2) (LOGN (DIV (ADD 1 A) (SUB 1 A)) E))))
     ;; ((DIV A (SHL 1 B)) (SAR A B))  ;; ONLY FOR UNSIGNED INTEGER A
     ((SQRT A) (POW A (DIV 1 2)))
     ((CBRT A) (POW A (DIV 1 3)))
     ((ROOT A B) (POW A (DIV 1 B))))))

(DEFUN SIMPLIFY (X RULES)
  (RIGHTEN (SIMPTREE (IF EXPLAIN
                         (PRINT 'SIMPLIFY (RIGHTEN X))
                       (RIGHTEN X))
                     (OR RULES LOLGEBRA))))

(DEFUN DERIV (X WRT)
  (COND ((EQ X WRT) '1)
        ((ATOM X) '0)
        ((LET ((OP (CAR X))
               (X (CADR X))
               (Y (CADDR X)))
           (COND ((OR (EQ OP 'ADD)
                      (EQ OP 'SUB))
                  `(,OP ,(DERIV X WRT)
                        ,(DERIV Y WRT)))
                 ((EQ OP '*)
                  `(ADD (* ,X ,(DERIV Y WRT))
                        (* ,(DERIV X WRT) ,Y)))
                 ((EQ OP 'DIV)
                  `(SUB (DIV ,(DERIV X WRT) ,Y)
                        (DIV (* ,X ,(DERIV Y WRT)) (POW ,Y 2))))
                 ((EQ OP 'POW)
                  `(ADD (* (POW ,X ,Y)
                             (* (LOG ,X) ,(DERIV Y WRT)))
                        (* ,Y (* (POW ,X (SUB ,Y 1))
                                     ,(DERIV X WRT)))))
                 ((EQ OP 'LOG) `(DIV ,(DERIV X WRT) ,X))
                 ((EQ OP 'SIN) `(* ,(DERIV X WRT) (COS ,X)))
                 ((EQ OP 'COS) `(SUB 0 (* ,(DERIV X WRT) (SIN ,X))))
                 ((EQ OP 'ABS) `(DIV (* ,X ,(DERIV X WRT)) (ABS ,X)))
                 ((ERROR 'HOW (LIST 'DERIV (LIST OP X Y) WRT))))))))

(DEFUN DERIVATIVE (X Y RULES)
  (SIMPLIFY
   (LET ((X (SIMPLIFY X)))
     (IF EXPLAIN
         (PRINT 'DIFFERENTIATE X
                'WRT Y
                'IS (DERIV X Y))
       (DERIV X Y)))
   RULES))
