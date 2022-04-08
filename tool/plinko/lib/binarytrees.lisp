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
;; ;; goes 2x faster than python even though it rolls its own arithmetic.
;; ;; goes 2x faster than racket but not sbcl since, since plinko is an
;; ;; interpreter and doesn't jit native instructions currently.
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
