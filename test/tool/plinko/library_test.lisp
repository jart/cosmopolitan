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

(test   nil  (cmp 'a 'a))
(test '(nil) (cmp 'a 'b))
(test   t  (cmp 'b 'a))
(test '(nil) (cmp 'a 'aa))
(test   t  (cmp 'aa 'a))
(test '(nil) (cmp 'a '(a)))
(test   t  (cmp '(a) 'a))
(test   nil  (cmp '(a) '(a)))
(test '(nil) (cmp '(a) '(a b)))
(test   nil  (cmp '(a a) '(a a)))
(test '(nil) (cmp '(a a) '(a aa)))
(test '(nil) (cmp '(a a) '((a) a)))
(test '(nil) (cmp '(a a) '(a a b)))
(test 'hello ((macro (x) (cons (quote quote) (cons x))) hello))
(test 'hello ((closure (macro (x) (cons (quote quote) (cons x)))) hello))
(test '(quoth hello) ((macro (x) (cons (quote quote) (cons (cons (quote quoth) (cons x))))) hello))
(test (quote (macro (x) (cons (quote quote) (cons x)))) (car (cdr (macro (x) (cons (quote quote) (cons x))))))

(test 'b ((closure (lambda () a) (a . b))))
(test 'b (eval 'a '((a . b))))

(test nil (reverse))
(test '(c b a) (reverse '(a b c)))

(test '(a b c)
      (akeys '((a x)
               (b y)
               (c z))))

(test '(x y z)
      (avals '((a x)
               (b y)
               (c z))))

(test t (and))
(test nil (and nil))
(test t (and t))
(test nil (and nil nil))
(test nil (and t nil))
(test nil (and nil t))
(test t (and t t))
(test 'a (and 'a))
(test 'b (and 'a 'b))
(test 'c (and 'a 'b 'c))

(test nil (or))
(test nil (or nil))
(test t (or t))
(test nil (or nil nil))
(test t (or t nil))
(test t (or nil t))
(test t (or t t))
(test 'a (or 'a))
(test 'a (or 'a 'b))
(test 'a (or 'a 'b 'c))

(test t (not))
(test t (not nil))
(test nil (not t))
(test '(eq x) (expand '(not x)))

(test '(abc . def)
      (let ((x 'abc)
            (y 'def))
        (cons x y)))

(test '((lambda (x y)
          (cons x y))
        'abc
        'def)
      (expand '(let ((x 'abc)
                (y 'def))
            (cons x y))))

(test '(abc def (abc . def))
      (let* ((x 'abc)
             (y 'def)
             (z (cons x y)))
        (cons x (cons y (cons z)))))

(test '((lambda (x)
          ((lambda (y)
             ((lambda (z)
                (list x y z))
              (cons x y)))
           'def))
        'abc)
      (expand '(let* ((x 'abc)
                 (y 'def)
                 (z (cons x y)))
            (list x y z))))

(test nil (last nil))
(test 'a (last '(a)))
(test 'b (last '(a b)))
(test '(c) (last '(a b (c))))

(test nil (copy nil))
(test t (copy t))
(test '(a b) (copy '(a b)))
(test '(a . b) (copy '(a . b)))

(test nil (assoc 'a ()))
(test nil (assoc 'd '((a b) (c d))))
(test '(a b) (assoc 'a '((a b) (c d))))
(test '(c d) (assoc 'c '((a b) (c d))))
(test '((foo bar) d) (assoc '(foo bar) '((a b) ((foo bar) d))))

(test '(a) (addset 'a))
(test '(a) (addset 'a '(a)))
(test '(b a) (addset 'b '(a)))
(test '(a b) (addset 'b '(a b)))

(test '(a) (peel 'a))
(test '(a) (peel 'a '(a)))
(test '(b a) (peel 'b '(a)))
(test '(b a b) (peel 'b '(a b)))

(test nil (reverse (reverse-mapcar cons nil)))
(test '((a) (b) (c)) (reverse (reverse-mapcar cons '(a b c))))
(test '(a b a) (reverse (reverse-mapcar car '((a . x) (b . y) (a . z)))))
(test '(a b a c) (reverse (reverse-mapcar car '((a . x) (b . y) (a . z) (c . z)))))
(test '((a . a) (b . b) (a . a)) (reverse (reverse-mapcar (lambda (x) (cons x x)) '(a b a))))

(test nil (mapcar cons nil))
(test '((a) (b) (c)) (mapcar cons '(a b c)))
(test '(a b a) (mapcar car '((a . x) (b . y) (a . z))))
(test '(a b a c) (mapcar car '((a . x) (b . y) (a . z) (c . z))))
(test '((a . a) (b . b) (a . a)) (mapcar (lambda (x) (cons x x)) '(a b a)))

(test nil (mapset cons nil))
(test '((a) (b) (c)) (mapset cons '(a b c)))
(test '(a b) (mapset car '((a . x) (b . y) (a . z))))
(test '((a . a) (b . b)) (mapset (lambda (x) (cons x x)) '(a b a)))

(test 'a (reduce list '(a)))
(test '(((a b) c) d) (reduce list '(a b c d)))

(test '(a b c d) (append '(a b) '(c d)))

(test t (all ()))
(test nil (all '(a b nil d)))
(test 'd (all '(a b c d)))

(test nil (any ()))
(test 'a (any '(a b nil d)))
(test 'b (any '(nil b c d)))
(test nil (any '(nil nil)))

(test '((a . b) (c . d)) (pairwise '(a b c d)))
(test '((a . a) (b . b) (c . c)) (dolist (x '(a b c)) (cons x x)))
(test '((a . a) (b . b) (c . c)) (reverse (reverse-dolist (x '(a b c)) (cons x x))))
(test '((a . a) (c . c)) (reverse (reverse-dolist (x '(a a c) peel) (cons x x))))

(test '((abs ((abs k) k))
        (log ((log 1) 0))
        (add ((add 1 2) 3)
             ((add 1 3) 4)
             ((add 1 4) 5))
        (sub ((sub 1 1) 0)
             ((sub 2 1) 1)
             ((sub 3 1) 2)))
      (GroupBy (lambda (x) (caar x))
               '(((abs k) k)
                 ((log 1) 0)
                 ((add 1 2) 3)
                 ((add 1 3) 4)
                 ((add 1 4) 5)
                 ((sub 1 1) 0)
                 ((sub 2 1) 1)
                 ((sub 3 1) 2))))

(progn
  (cond ((eq 'b (car (cdr '(a b c d))))) ((error '(cadr failed))))
  (cond ((eq 'c (car (cdr (cdr '(a b c d)))))) ((error '(caddr failed))))
  (cond ((eq 'c (car (cdr (car '(((a . b) . (c . d)))))))) ((error '(cadar failed))))
  (cond ((eq 'a (car (car '((a . b) . (c . d)))))) ((error '(caar failed))))
  (cond ((eq 'b (cdr (car '((a . b) . (c . d)))))) ((error '(cdar failed))))
  (cond ((eq 'c (car (cdr '((a . b) . (c . d)))))) ((error '(cadr failed))))
  (cond ((eq 'd (cdr (cdr '((a . b) . (c . d)))))) ((error '(cddr failed))))
  (cond ((eq 'a ((lambda ((x    )) x) (cons 'a 'b)))) ((error '(lambda car failed))))
  (cond ((eq 'b ((lambda ((nil . x)) x) (cons 'a 'b)))) ((error '(lambda cdr failed))))
  (ignore))

(progn
  (cond ((eq (quote a)
         ((lambda (f)
            (f f (quote ((a) b c))))
          (lambda (f a)
            (cond ((atom a) a)
               ((f f (car a))))))))
     ((error (quote (test failed))
             (quote (find first atom)))))
  (ignore))

(progn
  (cond ((equal (cons 'a 'b)
         (((lambda (f)
             (f (cons 'a 'b)))
           (lambda (x)
             (lambda ()
               x))))))
     ((error '(enclosed memory failed to propagate upward))))
  (ignore))

