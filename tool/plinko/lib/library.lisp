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

(DEFINE YCOMBINATOR
  (LAMBDA (F)
    ((LAMBDA (G)
       (G G))
     (LAMBDA (G)
       (F (LAMBDA A
            ((G G) . A)))))))

(DEFINE REVERSE
  (YCOMBINATOR
   (LAMBDA (REVERSE)
     (LAMBDA (X Y)
       (COND (X (REVERSE (CDR X) (CONS (CAR X) Y)))
             (Y))))))

(DEFINE COPY
  (YCOMBINATOR
   (LAMBDA (COPY)
     (LAMBDA (X R)
       (COND ((ATOM X) X)
             ((CONS (COPY (CAR X))
                    (COPY (CDR X)))))))))

(DEFINE ISPRECIOUS
  (LAMBDA (X)
    (COND ((EQ X NIL))
          ((EQ X T))
          ((EQ X EQ))
          ((EQ X CMP))
          ((EQ X CAR))
          ((EQ X CDR))
          ((EQ X BETA))
          ((EQ X ATOM))
          ((EQ X COND))
          ((EQ X CONS))
          ((EQ X FORK))
          ((EQ X QUOTE))
          ((EQ X MACRO))
          ((EQ X LAMBDA))
          ((EQ X DEFINE))
          ((EQ X EXPAND))
          ((EQ X CLOSURE))
          ((EQ X PARTIAL))
          ((EQ X FUNCTION))
          ((EQ X INTEGRATE))
          ((EQ X GC))
          ((EQ X READ))
          ((EQ X DUMP))
          ((EQ X EXIT))
          ((EQ X PROGN))
          ((EQ X QUIET))
          ((EQ X ERROR))
          ((EQ X TRACE))
          ((EQ X PRINT))
          ((EQ X PRINC))
          ((EQ X FLUSH))
          ((EQ X GENSYM))
          ((EQ X PPRINT))
          ((EQ X IGNORE))
          ((EQ X MTRACE))
          ((EQ X FTRACE))
          ((EQ X GTRACE))
          ((EQ X PRINTHEAP))
          ((EQ X IMPOSSIBLE)))))

(DEFINE QUOTH
  (MACRO (X)
    (CONS QUOTE (CONS X))))

(DEFINE ISQUOTE
  (LAMBDA (X)
    (COND (X (COND ((ATOM X) NIL)
                   ((EQ (CAR X) QUOTE))))
          (T))))

(DEFINE QUOTECONS
  (LAMBDA (A B)
    (COND ((COND ((ISQUOTE A)
                  (ISQUOTE B)))
           (CONS QUOTE (CONS (CONS (CAR (CDR A))
                                   (CAR (CDR B))))))
          ((CONS CONS (CONS A (COND (B (CONS B)))))))))

(DEFINE BACKQUOTER
  (YCOMBINATOR
   (LAMBDA (BACKQUOTER)
     (LAMBDA (X)
       (COND ((ATOM X)
              (COND ((ISPRECIOUS X) X)
                    ((CONS QUOTE (CONS X NIL)))))
             ((EQ (CAR X) 'COMMA_) (CAR (CDR X)))
             ((COND ((ATOM (CAR X)) NIL)
                    ((EQ (CAR (CAR X)) 'SPLICE_)))
              (CAR (CDR (CAR X))))
             ((QUOTECONS
               (BACKQUOTER (CAR X))
               (BACKQUOTER (CDR X)))))))))

(DEFINE CURLY_ QUOTH)
(DEFINE STRING_ QUOTH)
(DEFINE SQUARE_ QUOTH)
(DEFINE BACKQUOTE_ (MACRO (X) (BACKQUOTER X)))

(DEFINE >
  (MACRO (X Y)
    `(EQ (CMP ,X ,Y) T)))

(DEFINE <=
  (MACRO (X Y)
    `(EQ (EQ (CMP ,X ,Y) T))))

(DEFINE >=
  (MACRO (X Y)
    `(ATOM (CMP ,X ,Y))))

(DEFINE <
  (MACRO (X Y)
    `(EQ (ATOM (CMP ,X ,Y)))))

(DEFINE NOT
  (MACRO (P)
    `(EQ ,P)))

(DEFINE IMPLIES
  ((LAMBDA (IMPLIES)
     (MACRO A (IMPLIES A)))
   (YCOMBINATOR
    (LAMBDA (IMPLIES)
      (LAMBDA (A)
        (COND (A `(COND (,(CAR A)
                         ,(COND ((CDR (CDR A))
                                 (IMPLIES (CDR A)))
                                ((CAR (CDR A)))))
                        (T)))
              (T)))))))

(DEFINE AND
  ((LAMBDA (AND)
     (MACRO A
       (IMPLIES A (AND A))))
   (YCOMBINATOR
    (LAMBDA (AND)
      (LAMBDA (A)
        (COND ((CDR A)
               (CONS COND (CONS (CONS (CAR A) (CONS (AND (CDR A)))))))
              ((CAR A))))))))

(DEFINE OR
  ((LAMBDA (OR)
     (MACRO A
       (COND (A (COND ((CDR A)
                       (CONS COND (OR A)))
                      ((CAR A)))))))
   (YCOMBINATOR
    (LAMBDA (OR)
      (LAMBDA (A)
        (COND (A (CONS (CONS (CAR A))
                       (OR (CDR A))))))))))

(DEFINE IFF
  (MACRO (P Q)
    ((LAMBDA (S)
       `((LAMBDA (,S)
           (COND (,P ,S)
                 (,S NIL)
                 (T)))
         ,Q))
     (GENSYM))))

(DEFINE ISUPPER
  (AND (>= 'A)
       (<= 'Z)))

(DEFINE ISLOWER
  (AND (>= "A")
       (<= "Z")))

(DEFINE CONDISTRIVIAL
  (YCOMBINATOR
   (LAMBDA (CONDISTRIVIAL)
     (LAMBDA (ISTRIVIAL)
       (LAMBDA (X)
         (OR (EQ X)
             (AND (AND (ISTRIVIAL (CAR (CAR X)))
                       (OR (EQ (CDR (CAR X)))
                           (ISTRIVIAL (CAR (CDR (CAR X))))))
                  ((CONDISTRIVIAL ISTRIVIAL) (CDR X)))))))))

(DEFINE ISTRIVIAL
  (YCOMBINATOR
   (LAMBDA (ISTRIVIAL)
     (LAMBDA (X)
       (COND ((ATOM X))
             ((EQ (CAR X) QUOTE))
             ((OR (EQ (CAR X) CAR)
                  (EQ (CAR X) CDR)
                  (EQ (CAR X) ATOM))
              (ISTRIVIAL (CAR (CDR X))))
             ((OR (EQ (CAR X) EQ)
                  (EQ (CAR X) CMP))
              (AND (ISTRIVIAL (CAR (CDR X)))
                   (ISTRIVIAL (CAR (CDR (CDR X))))))
             ((EQ (CAR X) COND)
              ((CONDISTRIVIAL ISTRIVIAL) (CDR X))))))))

(DEFINE EQUAL
  ((LAMBDA (EQUAL)
     (MACRO X
       (COND ((NOT X)
              (ERROR '(NEED ARGUMENTS)))
             ((NOT (CDR X))
              `(EQ ,(CAR X)))
             ((NOT (CDR (CDR X)))
              `(EQ (CMP ,(CAR X) ,(CAR (CDR X)))))
             ((ATOM (CAR X))
              (EQUAL (CAR X) (CDR X)))
             (((LAMBDA (S)
                 `((LAMBDA (,S)
                     ,(EQUAL S (CDR X)))
                   ,(CAR X)))
               (GENSYM))))))
   (YCOMBINATOR
    (LAMBDA (EQUAL)
      (LAMBDA (S X)
        (COND ((CDR X)
               `(COND ((EQ (CMP ,S ,(CAR X)))
                       ,(EQUAL S (CDR X)))))
              (`(EQ (CMP ,S ,(CAR X))))))))))

(DEFINE LIST
  (LAMBDA A
    A))

(DEFINE IF
  (MACRO (X A B)
    (COND (B `(COND (,X ,A)
                    (,B)))
          ((ERROR (LIST (LIST 'CONSIDER (LIST 'AND X A))
                        (LIST 'INSTEAD 'OF (LIST 'IF X A))))))))

(DEFINE CURRY
  (LAMBDA (F X)
    (LAMBDA A
      (F X . A))))

(DEFINE APPEND
  (LAMBDA (X Y)
    (COND (Y ((LAMBDA (F)
                (F F X))
              (LAMBDA (F X)
                (COND (X (CONS (CAR X) (F F (CDR X))))
                      (Y)))))
          (X))))

(DEFINE -APPEND
  (YCOMBINATOR
   (LAMBDA (-APPEND)
     (LAMBDA (Y X)
       (COND (X (CONS (CAR X) (-APPEND Y (CDR X))))
             (Y))))))

(DEFINE APPEND
  (LAMBDA (X Y)
    (COND (Y (-APPEND Y X))
          (X))))

(DEFINE KEEP
  (LAMBDA (X Y)
    (COND ((EQUAL X Y) X)
          (Y))))

(DEFINE PEEL
  (LAMBDA (X Y)
    (COND ((EQUAL X (CAR Y)) Y)
          ((CONS X Y)))))

(DEFINE AKEYS
  (YCOMBINATOR
   (LAMBDA (AKEYS)
     (LAMBDA (X)
       (COND (X (CONS (CAR (CAR X))
                      (AKEYS (CDR X)))))))))

(DEFINE EVAL
  (LAMBDA (X A)
    ((CONS CLOSURE (CONS (CONS LAMBDA (CONS () (CONS X))) A)))))

(DEFINE HASATOM
  (YCOMBINATOR
   (LAMBDA (HASATOM)
     (LAMBDA (V Z)
       (COND ((EQ V Z))
             ((ATOM Z) NIL)
             ((HASATOM V (CAR Z)))
             ((HASATOM V (CDR Z))))))))

(DEFINE COND-FREEVARS
  (YCOMBINATOR
   (LAMBDA (COND-FREEVARS)
     (LAMBDA (FREEVARS X R S)
       (COND ((ATOM X) R)
             ((COND-FREEVARS
               FREEVARS
               (CDR X)
               (FREEVARS (CAR (CAR X))
                         (COND ((CDR (CAR X))
                                (FREEVARS (CAR (CDR (CAR X))) R S))
                               (R))
                         S)
               S)))))))

(DEFINE LIST-FREEVARS
  (YCOMBINATOR
   (LAMBDA (LIST-FREEVARS)
     (LAMBDA (FREEVARS X R S)
       (COND ((ATOM X)
              (FREEVARS X R S))
             (((LAMBDA (Y)
                 (LIST-FREEVARS FREEVARS (CDR X) Y S))
               (FREEVARS (CAR X) R S))))))))

(DEFINE FREEVARS
  (YCOMBINATOR
   (LAMBDA (FREEVARS)
     (LAMBDA (X R S)
       (COND ((ATOM X)
              (COND ((ISPRECIOUS X) R)
                    ((HASATOM X S) R)
                    ((CONS X R))))
             ((EQ (CAR X) QUOTE) R)
             ((EQ (CAR X) CLOSURE) R)
             ((EQ (CAR X) LAMBDA)
              (FREEVARS (CAR (CDR (CDR X))) R (CONS (CAR (CDR X)) S)))
             ((EQ (CAR X) COND)
              (COND-FREEVARS FREEVARS (CDR X) R S))
             ((LIST-FREEVARS FREEVARS X R S)))))))

(DEFINE DEFUN
  (MACRO (F A B)
    (COND ((HASATOM F (FREEVARS B))
           `(DEFINE ,F
              (YCOMBINATOR
               (LAMBDA (,F)
                 (LAMBDA ,A
                   ,B)))))
          (`(DEFINE ,F
              (LAMBDA ,A
                ,B))))))

(DEFINE DEFMACRO
  (MACRO (F A B)
    (COND ((HASATOM F (FREEVARS B))
           `(DEFINE ,F
              (MACRO A
                ((YCOMBINATOR
                  (LAMBDA (,F)
                    (LAMBDA ,A
                      ,B)))
                 . A))))
          (`(DEFINE ,F
              (MACRO ,A
                ,B))))))

(DEFUN -REQUIRED (XS)
  (ERROR 'PARAMETER (LIST XS) 'IS 'REQUIRED))

(DEFMACRO REQUIRED (X)
  `(COND (,X) ((-REQUIRED ',X))))

(DEFUN -TEST1 (X XS)
  (OR (EQ X T)
      (ERROR (LIST (LIST 'ERROR (LIST 'TEST XS))
                   (LIST 'WANT T)
                   (LIST 'GOT X)))))

(DEFUN -TEST2 (X XS Y YS)
  (OR (EQUAL X Y)
      (IF (EQUAL X XS)
          (ERROR (LIST (LIST 'ERROR 'FAILED (LIST 'TEST XS YS))
                       (LIST 'WANT X)
                       (LIST 'GOT Y)))
        (ERROR (LIST (LIST 'ERROR 'FAILED (LIST 'TEST XS YS))
                     (LIST 'GOT Y))))))

(DEFMACRO TEST A
  (COND ((EQ A)
         (ERROR '(NEED ARGUMENTS)))
        ((EQ (CDR A))
         `(IGNORE (-TEST1 (QUIET ,(CAR A)) ',(CAR A))))
        ((EQ (CDR (CDR A)))
         `(IGNORE (-TEST2 (QUIET ,(CAR A)) ',(CAR A)
                          (QUIET ,(CAR (CDR A))) ',(CAR (CDR A)))))
        ((EQ A)
         (ERROR '(TOO MANY ARGUMENTS)))))

(DEFMACRO ASSERT (A)
  `(PROGN (-TEST T ,A T ',A) NIL))

(DEFUN MEMBER (X Y)
  (COND (Y (COND ((EQUAL X (CAR Y)) Y)
                 ((MEMBER X (CDR Y)))))))

(DEFUN SUBSET (X Y)
  (IMPLIES X (AND (MEMBER (CAR X) Y)
                  (SUBSET (CDR X) Y))))

(DEFUN INTERSECTION (X Y)
  (AND X (OR (AND (CAR X) (MEMBER (CAR X) Y)
                  (INTERSECTION (CDR X) Y))
             (INTERSECTION (CDR X) Y))))

(DEFUN UNION (X Y)
  (COND (X (COND ((MEMBER (CAR X) Y)
                  (UNION (CDR X) Y))
                 ((CONS (CAR X) (UNION (CDR X) Y)))))
        (Y)))

(DEFUN AKEYS (X)
  (COND (X (CONS (CAR (CAR X))
                 (AKEYS (CDR X))))))

(DEFUN AVALS (X)
  (COND (X (CONS (CAR (CDR (CAR X)))
                 (AVALS (CDR X))))))

(DEFMACRO    CAAR (X)                `(CAR (CAR ,X)))
(DEFMACRO    CADR (X)                `(CAR (CDR ,X)))
(DEFMACRO    CDAR (X)                `(CDR (CAR ,X)))
(DEFMACRO    CDDR (X)                `(CDR (CDR ,X)))
(DEFMACRO   CAAAR (X)           `(CAR (CAR (CAR ,X))))
(DEFMACRO   CAADR (X)           `(CAR (CAR (CDR ,X))))
(DEFMACRO   CADAR (X)           `(CAR (CDR (CAR ,X))))
(DEFMACRO   CADDR (X)           `(CAR (CDR (CDR ,X))))
(DEFMACRO   CDAAR (X)           `(CDR (CAR (CAR ,X))))
(DEFMACRO   CDADR (X)           `(CDR (CAR (CDR ,X))))
(DEFMACRO   CDDAR (X)           `(CDR (CDR (CAR ,X))))
(DEFMACRO   CDDDR (X)           `(CDR (CDR (CDR ,X))))
(DEFMACRO  CAAAAR (X)      `(CAR (CAR (CAR (CAR ,X)))))
(DEFMACRO  CAAADR (X)      `(CAR (CAR (CAR (CDR ,X)))))
(DEFMACRO  CAADAR (X)      `(CAR (CAR (CDR (CAR ,X)))))
(DEFMACRO  CAADDR (X)      `(CAR (CAR (CDR (CDR ,X)))))
(DEFMACRO  CADAAR (X)      `(CAR (CDR (CAR (CAR ,X)))))
(DEFMACRO  CADADR (X)      `(CAR (CDR (CAR (CDR ,X)))))
(DEFMACRO  CADDAR (X)      `(CAR (CDR (CDR (CAR ,X)))))
(DEFMACRO  CADDDR (X)      `(CAR (CDR (CDR (CDR ,X)))))
(DEFMACRO  CDAAAR (X)      `(CDR (CAR (CAR (CAR ,X)))))
(DEFMACRO  CDAADR (X)      `(CDR (CAR (CAR (CDR ,X)))))
(DEFMACRO  CDADAR (X)      `(CDR (CAR (CDR (CAR ,X)))))
(DEFMACRO  CDADDR (X)      `(CDR (CAR (CDR (CDR ,X)))))
(DEFMACRO  CDDAAR (X)      `(CDR (CDR (CAR (CAR ,X)))))
(DEFMACRO  CDDADR (X)      `(CDR (CDR (CAR (CDR ,X)))))
(DEFMACRO  CDDDAR (X)      `(CDR (CDR (CDR (CAR ,X)))))
(DEFMACRO  CDDDDR (X)      `(CDR (CDR (CDR (CDR ,X)))))
(DEFMACRO CADDDDR (X) `(CAR (CDR (CDR (CDR (CDR ,X))))))

(DEFUN ISCONST (P)
  (COND ((EQ P NIL))
        ((EQ P T))))

(DEFMACRO NAND (P Q)
  (COND ((COND ((ISCONST P)
                (ISCONST Q)))
         (IMPLIES P (IMPLIES Q NIL)))
        (`(IMPLIES ,P (IMPLIES ,Q NIL)))))

(DEFMACRO XOR (P Q)
  (COND ((COND ((ISCONST P)
                (ISCONST Q)))
         (COND (P (NOT Q))
               (Q)))
        (P `((LAMBDA (A B)
               (COND (A (COND (B NIL) (A)))
                     (B (COND (A NIL) (B)))))
             ,P
             ,Q))))

(DEFMACRO LET (VARS EXPR)
  `((LAMBDA ,(AKEYS VARS) ,EXPR)
    ,@(AVALS VARS)))

(DEFMACRO LET* (((A B) . C) E)
  (IF A `((LAMBDA (,A) ,(LET* C E)) ,B) E))

(DEFUN LAST (X R)
  (IF X (LAST (CDR X) (CAR X)) R))

(DEFUN ASSOC (X Y D)
  (AND Y (IF (EQUAL X (CAR (CAR Y)))
             (CAR Y)
           (ASSOC X (CDR Y)))))

(DEFUN ADDSET (X Y)
  (IF (MEMBER X Y) Y (CONS X Y)))

(DEFUN REVERSE-MAPCAR (F X C R)
  (IF X (REVERSE-MAPCAR F (CDR X) C ((OR C CONS) (F (CAR X)) R)) R))

(DEFUN MAPCAR (F X)
  (AND X (CONS (F (CAR X)) (MAPCAR F (CDR X)))))

(DEFUN MAPSET (F X R)
  (IF X (MAPSET F (CDR X) (ADDSET (F (CAR X)) R)) (REVERSE R)))

(DEFINE REDUCE
  ((LAMBDA (G)
     (LAMBDA (F X)
       (IF (CDR X) (G F (CDR X) (CAR X)) (CAR X))))
   (YCOMBINATOR
    (LAMBDA (G)
      (LAMBDA (F X A)
        (IF X (G F (CDR X) (F A (CAR X))) A))))))

(DEFUN ALL (X)
  (IMPLIES X (REDUCE AND X)))

(DEFUN ANY (X)
  (REDUCE OR X))

(DEFUN PAIRWISE (X R)
  (IF X
      (PAIRWISE (CDDR X) (CONS (CONS (CAR X) (CADR X)) R))
    (REVERSE R)))

(DEFMACRO DOLIST ((A B) F)
  `(MAPCAR (LAMBDA (,A) ,F) ,B))

(DEFMACRO REVERSE-DOLIST ((A B C R) F)
  `(REVERSE-MAPCAR (LAMBDA (,A) ,F) ,B ,C ,R))

(DEFUN PAIRLIS (X Y A)
  (COND ((EQ X) A)
        ((ATOM X) (CONS (CONS X (CONS Y)) A))
        ((ATOM Y) (ERROR '(ARGUMENT STRUCTURE) X Y))
        ((LET ((A (PAIRLIS (CDR X) (CDR Y) A)))
           (COND ((CAR X) (PAIRLIS (CAR X) (CAR Y) A))
                 (A))))))

(DEFUN JOIN (S X)
  (COND ((EQ X) NIL)
        ((EQ (CDR X) NIL) (CONS (CAR X)))
        ((CONS (CAR X) (CONS S (JOIN S (CDR X)))))))

(DEFUN GETVAR (V A)
  (AND A (COND ((EQ V (CAAR A)) (CAR A))
               ((GETVAR V (CDR A))))))

(DEFUN GROUPBY (F X K G R)
  (IF X (LET ((J (F (CAR X))))
          (IF (EQUAL J K)
              (GROUPBY F (CDR X) K (CONS (CAR X) G) R)
            (GROUPBY F (CDR X) J (CONS (CAR X))
                     (OR (AND G (CONS (CONS K (REVERSE G)) R)) R))))
    (REVERSE (OR (AND G (CONS (CONS K (REVERSE G)) R)) R))))
