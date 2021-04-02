; This is a init file for Mini-Scheme.
; Some modifications by Zak Fenton MMXX/MMXXI for use in KScheme (removed any code with additional copyright notices)

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

(define (fib-tests i)
    (and
        (tests)
        (if (< i 3)
        1
        (+ (fib (- i 2)) (fib (- i 1))))))

(define (strpad str ext len)
    (if (>= (string-length str) len)
        str
        (strpad (string-cat str ext) ext len)))

(define (fib-box i)
    (cons "fib-box" (strpad "" "I" i)))

(define (fib-unbox b)
    (string-length (cdr b)))

(define (boxed-fib b)
    (if (< (fib-unbox b) 3)
        (fib-box 1)
        (fib-box
            (+
                (fib-unbox (boxed-fib (fib-box (- (fib-unbox b) 2))))
                (fib-unbox (boxed-fib (fib-box (- (fib-unbox b) 1))))))))

(define fib-type 'FIB-ABSTRACTION-TYPE)

(define (fib-abstract i)
    (abstraction-new fib-type (buffer-new i)))

(define (fib-unabstract b)
    (buffer-length (abstraction-value b)))

(define (abstract-fib b)
    (if (< (fib-unabstract b) 3)
        (fib-abstract 1)
        (fib-abstract
            (+
                (fib-unabstract (abstract-fib (fib-abstract (- (fib-unabstract b) 2))))
                (fib-unabstract (abstract-fib (fib-abstract (- (fib-unabstract b) 1))))))))


(define (tests-check name expected result)
    (if (= result expected)
        "Success"
        (begin
            (display "FAILED: ") (display name) (display ": Expected ") (display expected) (display " but got ") (display result) (newline)
            #f)))

(define (tests-simple)
    (and
        (tests-check "(+ 1 1)" 2 (+ 1 1))
        (tests-check "(- 1 1)" 0 (- 1 1))
        (tests-check "(* 2 3)" 6 (* 2 3))
        (tests-check "(/ 12 3)" 4 (/ 12 3))
        (tests-check "(string? 12)" #f (string? 12))
        (tests-check "(number? 12)" #t (number? 12))))

(define (tests)
    (tests-simple))

(define (lstring-of-primitive-string str) (cons 'lstring (cons str '())))

(define (lstring? str) (and (pair? str) (eq? (car str) 'lstring)))

; A simple attempt at exception handling

(define exit-to-top #f)
(call/cc (lambda (x) (set! exit-to-top x)))
(define current-exception-handlers '())
(define (throw-recursive handlers x)
    (if (null? handlers)
        (exit-to-top (list 'uncaught-exception x))
        (begin
            (set! current-exception-handlers (cdr handlers))
            ((car handlers) (cons #f x))
        )))
(define (throw x)
    (throw-recursive current-exception-handlers x))
(define (pcall f)
    (call/cc (lambda (continuation)
        (set! current-exception-handlers (cons continuation current-exception-handlers))
        (let ((result (f)))
            (set! current-exception-handlers (cdr current-exception-handlers))
            (cons #t result))
    )))

;(define int+ +)
;(define (+ a b)
;    (if (and (string? a) (string? b))
;        (strcat a b)
;        (int+ a b)))

; Cleaned up versions of basic arithmetic functions, throwing exceptions instead of returning
; false on overflow or bad input
(define fast+ +)
(define fast- -)
(define fast* *)
(define fast/ /)

(define (+ . args)
    (let ((result (apply fast+ args)))
        (if result result (throw (cons 'math-error (cons '+ args))))))
(define (- . args)
    (let ((result (apply fast- args)))
        (if result result (throw (cons 'math-error (cons '- args))))))
(define (* . args)
    (let ((result (apply fast* args)))
        (if result result (throw (cons 'math-error (cons '* args))))))
(define (/ . args)
    (let ((result (apply fast/ args)))
        (if result result (throw (cons 'math-error (cons '/ args))))))

; This is a hook for the parser, a call to this function will be generated wherever it encounters an integer too large to parse
(define (parse-number str) (throw (cons 'number-too-large str)))

(macro example-macro (lambda (expr)
    (display "This macro will expand to a hello world, but I got: ")
    (display expr)
    (display "\n")
    '(display "Hello, world\n")))

(begin (display "Loaded init.scm!") (newline))

(define (font-new . args) (cons 'font (cons args '())))
(define font-format-8x8 (cons 'font-format '8x8))
(define font-format-16x16 (cons 'font-format '16x16))
(define (font-add font . args)
    (set-cdr! (cdr font) (cons args (cddr font)))
    '())
(define (font-stamp . args) (cons 'font-stamp args))

; Aliases for R5RS-like compatibility
(define string-ref string-get)