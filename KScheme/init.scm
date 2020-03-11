; This is a init file for Mini-Scheme.
; Some modifications by Zak Fenton MMXX for use in KScheme (removed any code with additional copyright notices)

;; fake pre R^3 boolean values
(define nil #f)
(define t #t)

(define (caar x) (car (car x)))
(define (cadr x) (car (cdr x)))
(define (cdar x) (cdr (car x)))
(define (cddr x) (cdr (cdr x)))
(define (caaar x) (car (car (car x))))
(define (caadr x) (car (car (cdr x))))
(define (cadar x) (car (cdr (car x))))
(define (caddr x) (car (cdr (cdr x))))
(define (cdaar x) (cdr (car (car x))))
(define (cdadr x) (cdr (car (cdr x))))
(define (cddar x) (cdr (cdr (car x))))
(define (cdddr x) (cdr (cdr (cdr x))))

(define call/cc call-with-current-continuation)

(define (list . x) x)

(define (map proc list)
    (if (pair? list)
        (cons (proc (car list)) (map proc (cdr list)))))

(define (for-each proc list)
    (if (pair? list)
        (begin (proc (car list)) (for-each proc (cdr list)))
        #t ))

(define (list-tail x k)
    (if (zero? k)
        x
        (list-tail (cdr x) (- k 1))))

(define (list-ref x k)
    (car (list-tail x k)))

(define (last-pair x)
    (if (pair? (cdr x))
        (last-pair (cdr x))
        x))

(define (head stream) (car stream))

(define (tail stream) (force (cdr stream)))

(define (fib i)
    (if (< i 3)
        1
        (+ (fib (- i 2)) (fib (- i 1)))))
