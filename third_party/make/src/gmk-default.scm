;; Contents of the (gnu make) Guile module
;; Copyright (C) 2011-2020 Free Software Foundation, Inc.
;; This file is part of GNU Make.
;;
;; GNU Make is free software; you can redistribute it and/or modify it under
;; the terms of the GNU General Public License as published by the Free
;; Software Foundation; either version 3 of the License, or (at your option)
;; any later version.
;;
;; GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
;; WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
;; FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
;; details.
;;
;; You should have received a copy of the GNU General Public License along
;; with this program.  If not, see <http://www.gnu.org/licenses/>.

(define (to-string-maybe x)
  (cond
   ;; In GNU make, "false" is the empty string
   ((or (not x)
        (unspecified? x)
        (variable? x)
        (null? x)
        (and (string? x) (string-null? x)))
    #f)
   ;; We want something not false... not sure about this
   ((eq? x #t) "#t")
   ;; Basics
   ((or (symbol? x) (number? x))
    (object->string x))
   ((char? x)
    (string x))
   ;; Printable string (no special characters)
   ((and (string? x) (string-every char-set:printing x))
    x)
   ;; No idea: fail
   (else (error "Unknown object:" x))))

(define (obj-to-str x)
  (let ((acc '()))
    (define (walk x)
      (cond ((pair? x) (walk (car x)) (walk (cdr x)))
            ((to-string-maybe x) => (lambda (s) (set! acc (cons s acc))))))
    (walk x)
    (string-join (reverse! acc))))

;; Return the value of the GNU make variable V
(define (gmk-var v)
  (gmk-expand (format #f "$(~a)" (obj-to-str v))))

;; Export the public interfaces
(export gmk-expand gmk-eval gmk-var)
