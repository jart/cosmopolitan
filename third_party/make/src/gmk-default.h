static const char *const GUILE_module_defn = " \
(define (to-string-maybe x) \
  (cond \
   ((or (not x) \
        (unspecified? x) \
        (variable? x) \
        (null? x) \
        (and (string? x) (string-null? x))) \
    #f) \
   ((eq? x #t) \"#t\") \
   ((or (symbol? x) (number? x)) \
    (object->string x)) \
   ((char? x) \
    (string x)) \
   ((and (string? x) (string-every char-set:printing x)) \
    x) \
   (else (error \"Unknown object:\" x)))) \
(define (obj-to-str x) \
  (let ((acc '())) \
    (define (walk x) \
      (cond ((pair? x) (walk (car x)) (walk (cdr x))) \
            ((to-string-maybe x) => (lambda (s) (set! acc (cons s acc)))))) \
    (walk x) \
    (string-join (reverse! acc)))) \
(define (gmk-var v) \
  (gmk-expand (format #f \"$(~a)\" (obj-to-str v)))) \
(export gmk-expand gmk-eval gmk-var) \
";
