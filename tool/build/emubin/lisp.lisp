;; (setq lisp-indent-function 'common-lisp-indent-function)
;; (paredit-mode)

;;                              ________
;;                             /_  __/ /_  ___
;;                              / / / __ \/ _ \
;;                             / / / / / /  __/
;;                            /_/ /_/ /_/\___/
;;     __    _________ ____     ________          ____
;;    / /   /  _/ ___// __ \   / ____/ /_  ____ _/ / /__  ____  ____ ____
;;   / /    / / \__ \/ /_/ /  / /   / __ \/ __ `/ / / _ \/ __ \/ __ `/ _ \
;;  / /____/ / ___/ / ____/  / /___/ / / / /_/ / / /  __/ / / / /_/ /  __/
;; /_____/___//____/_/       \____/_/ /_/\__,_/_/_/\___/_/ /_/\__, /\___/
;;                                                           /____/
;;
;; THE LISP CHALLENGE
;;
;; PICK YOUR FAVORITE PROGRAMMING LANGUAGE
;; IMPLEMENT THE TINIEST POSSIBLE LISP MACHINE THAT
;; BOOTSTRAPS JOHN MCCARTHY'S METACIRCULAR EVALUATOR
;; WINNING IS DEFINED BY LINES OF CODE FOR SCRIPTING LANGUAGES
;; WINNING IS DEFINED BY BINARY FOOTPRINT FOR COMPILED LANGUAGES
;;
;; @SEE LISP FROM NOTHING; NILS M. HOLM; LULU PRESS, INC. 2020
;; @SEE RECURSIVE FUNCTIONS OF SYMBOLIC EXPRESSIONS AND THEIR
;;      COMPUTATION BY MACHINE, PART I; JOHN MCCARTHY, MASSACHUSETTS
;;      INSTITUTE OF TECHNOLOGY, CAMBRIDGE, MASS. APRIL 1960

;; NIL ATOM
;; ABSENCE OF VALUE
NIL

;; CONS CELL
;; BUILDING BLOCK OF DATA STRUCTURES
(CONS NIL NIL)

;; REFLECTION
;; EVERYTHING IS AN ATOM OR NOT AN ATOM
(ATOM NIL)
(ATOM (CONS NIL NIL))

;; QUOTING
;; CODE IS DATA AND DATA IS CODE
(QUOTE (CONS NIL NIL))
(CONS (QUOTE CONS) (CONS NIL (CONS NIL NIL)))

;; LOGIC
;; LAW OF IDENTITY VIA STRING INTERNING
(EQ (QUOTE A) (QUOTE A))

;; FIND FIRST ATOM IN TREE
;; RECURSIVE CONDITIONAL FUNCTION BINDING
((LAMBDA (FF X) (FF X))
 (QUOTE (LAMBDA (X)
          (COND ((ATOM X) X)
                ((QUOTE T) (FF (CAR X))))))
 (QUOTE ((A) B C)))

;; LISP IMPLEMENTED IN LISP
;; USED TO EVALUATE FIND FIRST ATOM IN TREE
;; REQUIRES CONS CAR CDR QUOTE ATOM EQ LAMBDA COND
;; FIXES BUGS FROM JOHN MCCARTHY PAPER AND MORE MINIMAL
((LAMBDA (ASSOC EVCON BIND APPEND EVAL)
   (EVAL (QUOTE ((LAMBDA (FF X) (FF X))
                 (QUOTE (LAMBDA (X)
                          (COND ((ATOM X) X)
                                ((QUOTE T) (FF (CAR X))))))
                 (QUOTE ((A) B C))))
         NIL))
 (QUOTE (LAMBDA (X E)
          (COND ((EQ E NIL) NIL)
                ((EQ X (CAR (CAR E))) (CDR (CAR E)))
                ((QUOTE T) (ASSOC X (CDR E))))))
 (QUOTE (LAMBDA (C E)
          (COND ((EVAL (CAR (CAR C)) E) (EVAL (CAR (CDR (CAR C))) E))
                ((QUOTE T) (EVCON (CDR C) E)))))
 (QUOTE (LAMBDA (V A E)
          (COND ((EQ V NIL) E)
                ((QUOTE T) (CONS (CONS (CAR V) (EVAL (CAR A) E))
                                 (BIND (CDR V) (CDR A) E))))))
 (QUOTE (LAMBDA (A B)
          (COND ((EQ A NIL) B)
                ((QUOTE T) (CONS (CAR A) (APPEND (CDR A) B))))))
 (QUOTE (LAMBDA (E A)
          (COND
            ((ATOM E) (ASSOC E A))
            ((ATOM (CAR E))
             (COND
               ((EQ (CAR E) NIL) (QUOTE *UNDEFINED))
               ((EQ (CAR E) (QUOTE QUOTE)) (CAR (CDR E)))
               ((EQ (CAR E) (QUOTE ATOM)) (ATOM (EVAL (CAR (CDR E)) A)))
               ((EQ (CAR E) (QUOTE EQ)) (EQ (EVAL (CAR (CDR E)) A)
                                            (EVAL (CAR (CDR (CDR E))) A)))
               ((EQ (CAR E) (QUOTE CAR)) (CAR (EVAL (CAR (CDR E)) A)))
               ((EQ (CAR E) (QUOTE CDR)) (CDR (EVAL (CAR (CDR E)) A)))
               ((EQ (CAR E) (QUOTE CONS)) (CONS (EVAL (CAR (CDR E)) A)
                                                (EVAL (CAR (CDR (CDR E))) A)))
               ((EQ (CAR E) (QUOTE COND)) (EVCON (CDR E) A))
               ((EQ (CAR E) (QUOTE LABEL)) (EVAL (CAR (CDR (CDR E)))
                                                 (APPEND (CAR (CDR E)) A)))
               ((EQ (CAR E) (QUOTE LAMBDA)) E)
               ((QUOTE T) (EVAL (CONS (EVAL (CAR E) A) (CDR E)) A))))
            ((EQ (CAR (CAR E)) (QUOTE LAMBDA))
             (EVAL (CAR (CDR (CDR (CAR E))))
                   (BIND (CAR (CDR (CAR E))) (CDR E) A)))))))
