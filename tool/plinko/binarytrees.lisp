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

(DEFUN OCT (X R)
  (COND (X (OCT (CDR (CDR (CDR X)))
                (COND ((CAR (CDR (CDR X)))
                       (COND ((CAR (CDR X))
                              (COND ((CAR X) (CONS '7 R))
                                    ((CONS '6 R))))
                             ((CAR X) (CONS '5 R))
                             ((CONS '4 R))))
                      ((CAR (CDR X))
                       (COND ((CAR X) (CONS '3 R))
                             ((CONS '2 R))))
                      ((CAR X) (CONS '1 R))
                      ((CDR (CDR (CDR X)))
                       (CONS '0 R))
                      (R))))
        ((CONS '0 R))))

(DEFUN + (A B C)
  (COND (C (COND (A (COND (B (COND ((CAR A) (COND ((CAR B) (CONS T (+ (CDR A) (CDR B) T)))
                                                  ((CONS NIL (+ (CDR A) (CDR B) T)))))
                                   ((CAR B) (CONS NIL (+ (CDR A) (CDR B) T)))
                                   ((CONS T (+ (CDR A) (CDR B))))))
                          ((CAR A) (CONS NIL (+ (CDR A) NIL T)))
                          ((CONS T (CDR A)))))
                 (B (COND ((CAR B) (CONS NIL (+ NIL (CDR B) T)))
                          ((CONS T (CDR B)))))
                 ((CONS C))))
        (A (COND (B (COND ((CAR A) (COND ((CAR B) (CONS NIL (+ (CDR A) (CDR B) T)))
                                         ((CONS T (+ (CDR A) (CDR B))))))
                          ((CAR B) (CONS T (+ (CDR A) (CDR B))))
                          ((CONS NIL (+ (CDR A) (CDR B))))))
                 (A)))
        (B)))

(DEFUN ++ (A)
  (COND ((CAR A) (CONS NIL (++ (CDR A))))
        ((CONS T (CDR A)))))

(DEFUN MAKE-TREE (DEPTH)
  (COND (DEPTH
         (LET ((D (CDR DEPTH)))
           (CONS (MAKE-TREE D)
                 (MAKE-TREE D))))
        ('(NIL NIL))))

(DEFUN CHECK-TREE (N R)
  (COND ((CAR N)
         (CHECK-TREE
          (CDR N)
          (CHECK-TREE
           (CAR N)
           (++ R))))
        ((++ R))))

;; ;; binary trees benchmark game
;; (TEST
;;  '(0 1 7 7 7 7 7 7 7)
;;  (OCT
;;   (CHECK-TREE
;;    (MAKE-TREE
;;     '(NIL NIL NIL NIL NIL NIL NIL
;;           NIL NIL NIL NIL NIL NIL NIL
;;           NIL NIL NIL NIL NIL NIL NIL)))))

;; use reasonably small size so tests go fast
(TEST
 '(0 3 7 7 7 7 7 7)
 (OCT
  (CHECK-TREE
   (MAKE-TREE
    '(NIL NIL NIL NIL NIL NIL NIL
          NIL NIL NIL NIL NIL NIL
          NIL NIL NIL NIL NIL NIL)))))
