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

;; unification
(TEST '(A) (RUN* (Q) (== Q 'A)))
(TEST '(A) (RUN* (Q) (== 'A Q)))
(TEST NIL (RUN* (Q) (== 'A 'B)))
(TEST '(_0) (RUN* (Q) (== 'A 'A)))
(TEST '((A B)) (RUN* (Q) (FRESH (X) (== Q (LIST 'A X)) (== X 'B))))
(TEST NIL (RUN* (Q) (== Q 'A) (== Q 'B)))
(TEST '((A . _0)) (RUN* (Q) (FRESH (X Y) (== Q (CONS X Y)) (== X 'A))))
(TEST '((_0 _0)) (RUN* (Q) (FRESH (X Y) (== Q (LIST X Y)) (== X Y))))
(TEST '((_0 _1)) (RUN* (Q) (FRESH (X Y) (== Q (LIST X Y)))))
(TEST '((A A)) (RUN* (X Y) (== X 'A) (== Y X)))

;; disjunction and conjunction
(TEST '(TEA COFFEE) (RUN* (Q) (CONDE ((== Q 'TEA)) ((== Q 'COFFEE)))))
(TEST '(TEA) (RUN 1 (Q) (CONDE ((== Q 'TEA)) ((== Q 'COFFEE)))))
(TEST NIL (RUN* (Q) FAIL))
(TEST '(_0) (RUN* (Q) SUCCEED))
(TEST '((TEA CUP))
      (RUN* (X Y)
        (CONDE ((== X 'TEA) (== Y 'CUP))
               ((== X 'COFFEE) (== Y 'MUG)))
        (== X 'TEA)))

;; relations run forwards, backwards, and sideways
(TEST '((A B C D)) (RUN* (Q) (APPENDO '(A B) '(C D) Q)))
(TEST '((A B)) (RUN* (Q) (APPENDO Q '(C) '(A B C))))
(TEST '((NIL (A B C))
        ((A) (B C))
        ((A B) (C))
        ((A B C) NIL))
      (RUN* (X Y) (APPENDO X Y '(A B C))))
(TEST '((NIL _0 _0)
        ((_0) _1 (_0 . _1))
        ((_0 _1) _2 (_0 _1 . _2)))
      (RUN 3 (X Y Z) (APPENDO X Y Z)))
(TEST '(A B C) (RUN* (Q) (MEMBERO Q '(A B C))))
(TEST '((A B) (B C)) (RUN* (X Y) (LEFTO X Y '(A B C))))
(TEST '(A C) (RUN* (Q) (NEXTO Q 'B '(A B C))))

;; the zebra puzzle, where houses are (color nationality pet smoke drink)
(DEFUN ZEBRAO (H)
  (CONJ*
   (== H (LIST (LIST (_) 'NORWEGIAN (_) (_) (_))
               (_)
               (LIST (_) (_) (_) (_) 'MILK)
               (_)
               (_)))
   (MEMBERO (LIST 'RED 'ENGLISHMAN (_) (_) (_)) H)
   (LEFTO (LIST 'IVORY (_) (_) (_) (_)) (LIST 'GREEN (_) (_) (_) (_)) H)
   (NEXTO (LIST (_) 'NORWEGIAN (_) (_) (_)) (LIST 'BLUE (_) (_) (_) (_)) H)
   (MEMBERO (LIST (_) (_) 'SNAILS 'WINSTON (_)) H)
   (MEMBERO (LIST 'YELLOW (_) (_) 'KOOLS (_)) H)
   (MEMBERO (LIST (_) 'SPANIARD 'DOG (_) (_)) H)
   (MEMBERO (LIST 'GREEN (_) (_) (_) 'COFFEE) H)
   (MEMBERO (LIST (_) 'UKRAINIAN (_) (_) 'TEA) H)
   (MEMBERO (LIST (_) (_) (_) 'LUCKYSTRIKES 'JUICE) H)
   (MEMBERO (LIST (_) 'JAPANESE (_) 'PARLIAMENTS (_)) H)
   (NEXTO (LIST (_) (_) 'HORSE (_) (_)) (LIST (_) (_) (_) 'KOOLS (_)) H)
   (NEXTO (LIST (_) (_) (_) 'CHESTERFIELDS (_)) (LIST (_) (_) 'FOX (_) (_)) H)
   (MEMBERO (LIST (_) (_) (_) (_) 'WATER) H)
   (MEMBERO (LIST (_) (_) 'ZEBRA (_) (_)) H)))

(TEST '(((YELLOW NORWEGIAN FOX KOOLS WATER)
         (BLUE UKRAINIAN HORSE CHESTERFIELDS TEA)
         (RED ENGLISHMAN SNAILS WINSTON MILK)
         (IVORY SPANIARD DOG LUCKYSTRIKES JUICE)
         (GREEN JAPANESE ZEBRA PARLIAMENTS COFFEE)))
      (RUN* (H) (ZEBRAO H)))
