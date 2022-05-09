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

(TEST '(A B C) (SUBST '(A B C) NIL))
(TEST '(X B C) (SUBST '(A B C) '((A . X))))
(TEST '(X B Z) (SUBST '(A B C) '((A . X) (C . Z))))
(TEST '((X) B Z) (SUBST '((A) B C) '((A . X) (C . Z))))

(TEST '(ADD B C) (RIGHTEN '(ADD C B)))
(TEST '(ADD C (ADD A B)) (RIGHTEN '(ADD C (ADD A B))))
(TEST '(ADD C (* A B)) (RIGHTEN '(ADD (* A B) C)))
(TEST '(ADD C (ADD A B)) (RIGHTEN '(ADD (ADD A B) C)))
(TEST '(ADD A (ADD B (ADD C D))) (RIGHTEN '(ADD (ADD (ADD D C) B) A)))

(TEST 'DOG (SIMPTREE 'DOG '((* ((* A 0) 0)))))
(TEST '(NOT X) (SIMPTREE '(NOT X) '((* ((* A 0) 0)))))
(TEST '0 (SIMPTREE '(* Z 0) '((* ((* A 0) 0)))))
(TEST '0 (SIMPTREE '(* (* Z 0) (* Z 0))
                   '((* ((* A 0) 0)
                          ((* A 0) 0)))))

(TEST '(A) (PERMCOMM 'A))
(TEST '((SUB A B)) (PERMCOMM '(SUB A B)))
(TEST '((ADD B A) (ADD A B)) (PERMCOMM '(ADD A B)))
(TEST '((ADD C (* A B)) (ADD (* A B) C) (ADD C (* B A)) (ADD (* B A) C)) (PERMCOMM '(ADD (* A B) C)))

(TEST '0 (SIMPLIFY '(AND P (NOT P))))
(TEST '0 (SIMPLIFY '(NOT (OR P (NOT P)))))
(TEST 'A (SIMPLIFY '(* (SQRT A) (SQRT A))))
(TEST 'A (SIMPLIFY '(SQRT (POW 4 (LOGN A 2)))))
(TEST '(* (ABS A) (POW 2 (DIV 1 2))) (SIMPLIFY '(HYPOT A A)))
(TEST '1 (SIMPLIFY '(POW (SQR A) 0)))
(TEST '(POW A (* B C)) (SIMPLIFY '(POW (POW A B) C)))
(TEST '(POW A (ADD B C)) (SIMPLIFY '(* (POW A B) (POW A C))))
(TEST 'B (SIMPLIFY '(LOGN (POW A B) A)))
(TEST '(DIV (LOG (DIV A B)) (LOG C)) (SIMPLIFY '(SUB (LOGN A C) (LOGN B C))))
;; (TEST '(DIV (* P (LOG X)) (LOG A)) (SIMPLIFY '(LOGN (POW X P) A)))  ;; LITTLE WEIRD
(TEST '(* B C) (SIMPLIFY '(LOGN (POW (POW A B) C) A)))
(TEST 'A (SIMPLIFY '(DIV A 1)))
(TEST '0 (SIMPLIFY '(DIV 0 A)))
(TEST ':DBZERO (SIMPLIFY '(DIV A 0)))
(TEST ':DBZERO (SIMPLIFY '(LOGN A 1)))
(TEST '(DIV A B) (SIMPLIFY '(DIV (SUB 0 A) (SUB 0 B))))
(TEST '1 (SIMPLIFY '(DIV A A)))
(TEST '(DIV B C) (SIMPLIFY '(DIV (* A B) (* A C))))
(TEST '(DIV A C) (SIMPLIFY '(* (DIV A B) (DIV B C))))
(TEST '(DIV C B) (SIMPLIFY '(* (DIV A B) (DIV C A))))
(TEST 'B (SIMPLIFY '(* (DIV B A) A)))
(TEST '(DIV (ADD A C) B) (SIMPLIFY '(ADD (DIV A B) (DIV C B))))
(TEST '(DIV (SUB A C) B) (SIMPLIFY '(SUB (DIV A B) (DIV C B))))
;; (TEST '(DIV (* A D) (* B C)) (SIMPLIFY '(DIV (DIV A B) (DIV C D))))
(TEST '(DIV 1 A) (SIMPLIFY '(* (POW A (SUB B 1)) (POW A (SUB 0 B)))))
(TEST '(* C (POW A 2)) (SIMPLIFY '(* (ABS A) (* (ABS A) C))))
(TEST '(OR B (NOT A)) (SIMPLIFY '(SUB (ADD (SUB 0 A) (AND A B)) 1)))
(TEST '(NOT (XOR A B)) (SIMPLIFY '(SUB (ADD (SUB (SUB 0 A) B) (* 2 (AND A B))) 1)))
(TEST '(ADD Z (SUB X Y)) (SIMPLIFY '(ADD (OR (SUB (AND X (NOT Y)) (AND (NOT X) Y)) Z) (AND (SUB (AND X (NOT Y)) (AND (NOT X) Y)) Z))))
(TEST '(* (ADD A B) (SUB A B)) (SIMPLIFY '(SUB (SQR A) (SQR B))))
(TEST '(* (ADD A B) (SUB A B)) (SIMPLIFY '(SUB (SUB (* A (ADD B A)) (* A B)) (POW B 2))))
(TEST '(ADD X Y) (SIMPLIFY '(ADD (XOR X Y) (* (AND X Y) 2))))

(TEST (DERIV '(* X 1) 'X) '(ADD (* X 0) (* 1 1)))
(TEST (DERIV '(* X Y) 'X) '(ADD (* X 0) (* 1 Y)))
(TEST (DERIV '(* (* X Y) (ADD X 3)) 'X) '(ADD (* (* X Y) (ADD 1 0)) (* (ADD (* X 0) (* 1 Y)) (ADD X 3))))
(TEST '(ADD (* (POW X 2) (* (LOG X) 0)) (* 2 (* (POW X (SUB 2 1)) 1))) (DERIV '(POW X 2) 'X))

(TEST '0 (DERIVATIVE '(* X 0) 'X))
(TEST '1 (DERIVATIVE '(* X 1) 'X))
(TEST 'Y (DERIVATIVE '(* X Y) 'X))
(TEST '(* 2 X) (DERIVATIVE '(* X X) 'X))
(TEST '(* 4 (POW X 3)) (DERIVATIVE '(* (* (* X X) X) X) 'X))
(TEST '(* Y (ADD 3 (* 2 X))) (DERIVATIVE '(* (* X Y) (ADD X 3)) 'X))
(TEST '(* 3 (POW X 2)) (DERIVATIVE '(POW X 3) 'X))
(TEST '(* 4 (POW X 3)) (DERIVATIVE '(POW (* X X) 2) 'X))
(TEST '(* Y (* 3 (POW X 2))) (DERIVATIVE '(* (* (* X Y) X) X) 'X))
(TEST '(DIV 1 X) (DERIVATIVE '(LOG X) 'X))
(TEST '(DIV Y X) (DERIVATIVE '(LOG (POW X Y)) 'X))
(TEST '(COS X) (DERIVATIVE '(SIN X) 'X))
(TEST '(* Y (COS (* X Y))) (DERIVATIVE '(SIN (* X Y)) 'X))
(TEST '(SUB 0 (SIN X)) (DERIVATIVE '(COS X) 'X))
(TEST '(SUB 0 (* Y (SIN (* X Y)))) (DERIVATIVE '(COS (* X Y)) 'X))
(TEST '(DIV 1 (* 2 (POW X (DIV 1 2)))) (DERIVATIVE '(SQRT X) 'X))
(TEST '(* Y (DIV X (ABS X))) (DERIVATIVE '(* Y (ABS X)) 'X))
(TEST '1 (DERIVATIVE '(SUB X Y) 'X))
(TEST '(SUB 0 1) (DERIVATIVE '(SUB X Y) 'Y))
(TEST '(* Y (POW X (SUB Y 1))) (DERIVATIVE '(POW X Y) 'X))
