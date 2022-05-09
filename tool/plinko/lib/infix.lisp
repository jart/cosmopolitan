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

(DEFUN UNARY-INFIX (INFIX I)
  (COND ((ATOM (CAR I))
         (COND ((EQ (CAR I) "+")
                (UNARY-INFIX INFIX (CDR I)))
               ((EQ (CAR I) "-")
                (LET ((X (UNARY-INFIX INFIX (CDR I))))
                  (CONS (LIST 'SUB '0 (CAR X)) (CDR X))))
               ((EQ (CAR I) "~")
                (LET ((X (UNARY-INFIX INFIX (CDR I))))
                  (CONS (LIST 'NOT (CAR X)) (CDR X))))
               ((EQ (CAR I) "!")
                (LET ((X (UNARY-INFIX INFIX (CDR I))))
                  (CONS (LIST 'NOT (LIST 'EQ '0 (CAR X))) (CDR X))))
               (I)))
        ((CONS (INFIX (CAR I)) (CDR I)))))

(DEFUN POW-INFIX (INFIX I)
  (LET ((X (UNARY-INFIX INFIX I)))
    (COND ((EQ (CADR X) "**")
           (LET ((Y (POW-INFIX INFIX (CDDR X))))
             (CONS (LIST 'POW (CAR X) (CAR Y)) (CDR Y))))
          (X))))

(DEFUN -MUL-INFIX (INFIX X)
  (COND ((EQ (CADR X) "*")
         (LET ((Y (POW-INFIX INFIX (CDDR X))))
           (-MUL-INFIX INFIX (CONS (LIST 'MUL (CAR X) (CAR Y)) (CDR Y)))))
        ((EQ (CADR X) "/")
         (LET ((Y (POW-INFIX INFIX (CDDR X))))
           (-MUL-INFIX INFIX (CONS (LIST 'DIV (CAR X) (CAR Y)) (CDR Y)))))
        ((EQ (CADR X) "%")
         (LET ((Y (POW-INFIX INFIX (CDDR X))))
           (-MUL-INFIX INFIX (CONS (LIST 'REM (CAR X) (CAR Y)) (CDR Y)))))
        (X)))

(DEFUN MUL-INFIX (INFIX I)
  (-MUL-INFIX INFIX (POW-INFIX INFIX I)))

(DEFUN -ADD-INFIX (INFIX X)
  (COND ((EQ (CADR X) "+")
         (LET ((Y (MUL-INFIX INFIX (CDDR X))))
           (-ADD-INFIX INFIX (CONS (LIST 'ADD (CAR X) (CAR Y)) (CDR Y)))))
        ((EQ (CADR X) "-")
         (LET ((Y (MUL-INFIX INFIX (CDDR X))))
           (-ADD-INFIX INFIX (CONS (LIST 'SUB (CAR X) (CAR Y)) (CDR Y)))))
        (X)))

(DEFUN ADD-INFIX (INFIX I)
  (-ADD-INFIX INFIX (MUL-INFIX INFIX I)))

(DEFUN -SHIFT-INFIX (INFIX X)
  (COND ((EQ (CADR X) "<<")
         (LET ((Y (ADD-INFIX INFIX (CDDR X))))
           (-SHIFT-INFIX INFIX (CONS (LIST 'SHL (CAR X) (CAR Y)) (CDR Y)))))
        ((EQ (CADR X) ">>")
         (LET ((Y (ADD-INFIX INFIX (CDDR X))))
           (-SHIFT-INFIX INFIX (CONS (LIST 'SAR (CAR X) (CAR Y)) (CDR Y)))))
        ((EQ (CADR X) ">>>")
         (LET ((Y (ADD-INFIX INFIX (CDDR X))))
           (-SHIFT-INFIX INFIX (CONS (LIST 'SHR (CAR X) (CAR Y)) (CDR Y)))))
        (X)))

(DEFUN SHIFT-INFIX (INFIX I)
  (-SHIFT-INFIX INFIX (ADD-INFIX INFIX I)))

(DEFUN -RELATIONAL-INFIX (INFIX X)
  (COND ((EQ (CADR X) "<")
         (LET ((Y (SHIFT-INFIX INFIX (CDDR X))))
           (-RELATIONAL-INFIX INFIX (CONS (LIST 'LT (CAR X) (CAR Y)) (CDR Y)))))
        ((EQ (CADR X) ">")
         (LET ((Y (SHIFT-INFIX INFIX (CDDR X))))
           (-RELATIONAL-INFIX INFIX (CONS (LIST 'GT (CAR X) (CAR Y)) (CDR Y)))))
        ((EQ (CADR X) ">=")
         (LET ((Y (SHIFT-INFIX INFIX (CDDR X))))
           (-RELATIONAL-INFIX INFIX (CONS (LIST 'GE (CAR X) (CAR Y)) (CDR Y)))))
        ((EQ (CADR X) "<=")
         (LET ((Y (SHIFT-INFIX INFIX (CDDR X))))
           (-RELATIONAL-INFIX INFIX (CONS (LIST 'LE (CAR X) (CAR Y)) (CDR Y)))))
        (X)))

(DEFUN RELATIONAL-INFIX (INFIX I)
  (-RELATIONAL-INFIX INFIX (SHIFT-INFIX INFIX I)))

(DEFUN -EQUALITY-INFIX (INFIX X)
  (COND ((EQ (CADR X) "==")
         (LET ((Y (RELATIONAL-INFIX INFIX (CDDR X))))
           (-EQUALITY-INFIX INFIX (CONS (LIST 'EQ (CAR X) (CAR Y)) (CDR Y)))))
        ((EQ (CADR X) "!=")
         (LET ((Y (RELATIONAL-INFIX INFIX (CDDR X))))
           (-EQUALITY-INFIX INFIX (CONS (LIST 'NOT (LIST 'EQ (CAR X) (CAR Y))) (CDR Y)))))
        (X)))

(DEFUN EQUALITY-INFIX (INFIX I)
  (-EQUALITY-INFIX INFIX (RELATIONAL-INFIX INFIX I)))

(DEFUN -AND-INFIX (INFIX X)
  (COND ((EQ (CADR X) "&")
         (LET ((Y (EQUALITY-INFIX INFIX (CDDR X))))
           (-AND-INFIX INFIX (CONS (LIST 'AND (CAR X) (CAR Y)) (CDR Y)))))
        (X)))

(DEFUN AND-INFIX (INFIX I)
  (-AND-INFIX INFIX (EQUALITY-INFIX INFIX I)))

(DEFUN -XOR-INFIX (INFIX X)
  (COND ((EQ (CADR X) "^")
         (LET ((Y (AND-INFIX INFIX (CDDR X))))
           (-XOR-INFIX INFIX (CONS (LIST 'XOR (CAR X) (CAR Y)) (CDR Y)))))
        (X)))

(DEFUN XOR-INFIX (INFIX I)
  (-XOR-INFIX INFIX (AND-INFIX INFIX I)))

(DEFUN -OR-INFIX (INFIX X)
  (COND ((EQ (CADR X) "|")
         (LET ((Y (XOR-INFIX INFIX (CDDR X))))
           (-OR-INFIX INFIX (CONS (LIST 'OR (CAR X) (CAR Y)) (CDR Y)))))
        (X)))

(DEFUN OR-INFIX (INFIX I)
  (-OR-INFIX INFIX (XOR-INFIX INFIX I)))

(DEFUN -LOGAND-INFIX (INFIX X)
  (COND ((EQ (CADR X) "&&")
         (LET ((Y (OR-INFIX INFIX (CDDR X))))
           (-LOGAND-INFIX
            INFIX (CONS (LIST 'AND
                              (LIST 'NOT (LIST 'EQ '0 (CAR X)))
                              (LIST 'NOT (LIST 'EQ '0 (CAR Y))))
                        (CDR Y)))))
        (X)))

(DEFUN LOGAND-INFIX (INFIX I)
  (-LOGAND-INFIX INFIX (OR-INFIX INFIX I)))

(DEFUN -LOGOR-INFIX (INFIX X)
  (COND ((EQ (CADR X) "||")
         (LET ((Y (LOGAND-INFIX INFIX (CDDR X))))
           (-LOGOR-INFIX
            INFIX (CONS (LIST 'OR
                              (LIST 'NOT (LIST 'EQ '0 (CAR X)))
                              (LIST 'NOT (LIST 'EQ '0 (CAR Y))))
                        (CDR Y)))))
        (X)))

(DEFUN LOGOR-INFIX (INFIX I)
  (-LOGOR-INFIX INFIX (LOGAND-INFIX INFIX I)))

(DEFUN CONDITIONAL-INFIX (INFIX I)
  (LET ((X (LOGOR-INFIX INFIX I)))
    (COND ((EQ (CADR X) "?")
           (LET ((Y (LOGOR-INFIX INFIX (CDDR X))))
             (COND ((EQ (CADR Y) ":")
                    (LET ((Z (CONDITIONAL-INFIX INFIX (CDDR Y))))
                      (CONS (LIST 'IF (CAR X) (CAR Y) (CAR Z)) (CDR Z))))
                   (X))))
          (X))))

(DEFUN INFIX (I)
  (LET ((X (CONDITIONAL-INFIX INFIX I)))
    (IF (CDR X)
        (ERROR 'UNINFIXD (CDR X))
      (CAR X))))
