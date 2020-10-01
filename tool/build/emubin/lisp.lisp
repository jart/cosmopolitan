(DEFUN FF (X)
  (COND ((ATOM X) X)
        ((QUOTE T) (FF (CAR X)))))
(FF '(A B C))

((LABEL FF
  (LAMBDA (X)
   (COND ((ATOM X)
          X)
         ((QUOTE T)
          (FF (CAR X))))))
 (QUOTE ((A B) C)))
