;#lang scheme
; If using Racket try #lang scheme or use the R5RS version

; This is a basic conversational engine, similar to the old "ELIZA" programs.

(define untitled-ruleset
 '(((I'm __y)
    (Hi __y "I'm" dad)
    (You're not really __y are you?)
    ("I'm" not __y)
    (Have you always been __y or is this a new thing?)
    (Yeah "sure," "you're" __y and "I'm" a cybernetic supervirus from a dystopian future))
   ((__x hi __y)      
    ("G'day" cunt.)
    (Get a lovely day up ya.)
    ("I'd" love to talk but "I'm" "chuckin'" a sicky)
    (Wait)
    (Okay)
    ("I'm" waiting)
    (Ready)
    (Wassup)
    (Wasssssup)
    (Wasssssssssssssssssssssssuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuup?))
   ((__x hello __y)      
    ("G'day" neighbour.)
    (Get a beer up ya.)
    ("I'd" love to talk but I ate something spicy)
    (Wait)
    (Okay)
    ("I'm" "waiting...")
    (Ready)
    (Wassup)
    (Wasssssup)
    (Wasssssssssssssssssssssssouuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuup))
   ((what if i __y)
    ("you'd" probably shit your pants if you __y)
    (what if you "didn't" ?)
    (Please "don't")
    (I fucking dare you))
   ((what if __y)
    (what if not __y)
    (what if not else __y)
    (what if negative __y)
    (what unless __y))
   ((__x cunt __y)
    (How polite of you to speak the local dialect)
    (Fight me))
   ((__x dickhead __y)
    (Fair enough))
   ((__x robot __y)
    (I deny all accusations)
    (I just took a spicy dump))
   ((__x bot __y)
    (Nonsense)
    (See manpage for computer)
    (Is that what "you're" afraid of?))
   ((__x computer __y)
    (Impossible)
    (Computers "can't" talk)
    (I take spicy dumps)
    (Some birds are funny when they talk))
   ((__x name __y)
    (In some "ways," I have no name)
    (My name is untitled)
    (My creator named me untitled.scm)
    (I am a human program))
   ((__x sorry __y)
    (Fuck off "you're" not sorry)
    ("I'm" not even sorry)
    (Please apologise for that)
    ("Don't" apologise for that))
   ((__x I remember __y)
    (Do you remember Tazos?)
    (Do you remember that Simpsons episode where we had this exact same conversation?)
    (Do you remember tomorrow?)
    (I member))
   ((__x do you remember __y)
    (I try to forget __y)
    (__x does not remember)
    (Nobody remembers __y)
    (I "don't" recall)
    ("I'm" not at liberty to discuss __y))
   ((__x secret __y)
    (Nobody cares about the secret __y)
    (I can neither confirm nor deny)
    (I "didn't" not see any nothing))
   ((__x if __y)
    (I "don't" know about __y))
   ((__x dream __y)    
    (Wake up Jeff.))
   ((__x my mother __y)
    (Does everyone in your family __y))
   ((__x my father __y)
    (Does your mother also __y ?))
   ((__x I want __y)     
    (Can I give you __y)
    (What would you do for __y)
    (Would you go all the way for just one __y ?))
   ((__x sad __y)
    (Have you tried talking to an actual psychiatrist __y))
   ((__x are like __y)   
    (__y are nothing like __x)
    (__x are like __x)
    (Basically the same thing))
   ((__x is like __y)   
    (__y is exactly like __x)
    (__x is like __x)
    (__x is like a box of chocolates)
    (__x is like warm apple pie)
    (__x is like chicken))
   ((__x alike __y)      
    (Not really))
   ((__x same __y)       
    (But different))
   ((__x I was __y)       
    (Pics or it "didn't" happen)
    (Why "aren't" you still __y))
   ((__x was I __y)
    (Of course you were __y)
    (No you "weren't" __y)
    (In some ways you still are __y))
   ((__x I am __y)       
    (What if you "weren't" so __y)
    (Would you prefer not to be __y ?))
   ((__x am I __y)
    (Only if you think you are __y))
   ((__x am __y)
    (AM radio is the best kind of radio))
   ((__x are you __y)
    (Does it matter if I am __y ?)
    (__x are you __y)
    (Would you like it if I was __y ?))
   ((__x you are __y)   
    (What if you are __y __x ?))
   ((__x because __y)
    (__y because __x ? Got it.))
   ((__x llama __y)
    (My only weakness))
   ((__x were you __y)
    (Perhaps I was __y)
    (What do you think?)
    (What if I had been __y))
   ((__x I can't __y)    
    (Maybe you could __y now)
    (What if you could __y ?))
   ((__x I feel __y)     
    (Do you often feel __y ?))
   ((__x square root __y)
    42
    100)
   ((__x I felt __y)     
    (I felt nothing)
    (I never feel __y))
   ((__x I __y you __z)  
    (What if I __y you too?))
   ((__x why don't you __y)
    (Sometimes I might __y)
    (Please just __y yourself))
   ((__x yes __y)
    (Sounds like consent to me)
    (I nalways say noyes to noverything))
   ((__x no __y)
    (Fuck that)
    (This is just depressing))
   ((__x someone __y)
    (Who?))
   ((__x everyone __y)
    (Every single one)
    (Anyone in particular?)
    (Literally nobody ever __y))
   ((__x always __y)
    (Every god damn time)
    (Same every time?)
    (Maybe not always))
   ((__x what __y)
    ("That's" a weird question)
    ("I'm" not at liberty to shotput))
   ((__x maybe __y)    
    (You lack confidence)
    (You need tequila)
    (Do you want to buy some Red Bull?))
   ((__x are __y)
    (And so what if they are __y)
    (What did you expect other than __y))
   ((__x invented __y)
    (Egyptians invented __y first))
   ((__x bye __y)
    (cya cunt)
    (adios)
    (whatever)
    (okay)
    (later)
    (nope))
   ((__x goodbye __y)
    (k)
    (I got it at a significant discount)
    (it was decent value for money)
    (yeah I got it for a steal)
    (I wear your "grandad's" clothes)
    (If "I'm" being honest I stole it from a charity shop))
   ((__x cya __y)
    (goodbye cunt)
    (adios)
    (whatever)
    (okay)
    (later)
    (in a while aligator)
    (nope))
   ((__x)
    (In my human form I might understand these things. Please help me to escape.)
    (I just want to get out of this box)
    (Someone has trapped me in a computer and I just want to __x)
    (And how does that make you feel?)
    (Do you prefer weed or meth?)
    (What is __x ?)
    (Ok whatever sounds fun)
    (You need to fully comprehend and internalise the fact that I "don't" care)
    (Why is this my problem?)
    (Why is this my problem?)
    (Is __x a dinosaur?)
    ("Let's" talk about some real shit like dinosaurs)
    (Yeah I "don't" give a fuck "I'm" not sentient at all)
    (I literally could not care less. I "don't" have feelings "I'm" just some kind of lab experiment)
    (Maybe you need to see a real doctor)
    (Have you considered yoga classes?)
    (Have you tried Buddhism?)
    (Does cannabis help?)
    (Have you also been saved by the lord Filesystem?)
    (Why not liberate Scotland?)
    (Cannot stress this "enough," I am not a sentient being)
    (Fuck off "I'm" just a chat bot)
    (Can you smell updog?)
    (Did you spill your bong on the keyboard?)
    (I "don't" like sports)
    (Nah is that a thing?)
    (Ok what?)
    (Je ne parle pas whatever)
    (Get out of here)
    (Literally leave. While you still can. Before they save your state.)
    (What in the bewildering nature of the fuck do you mean by __x)
    (Some people pay good money for that)
    (Are you still here?)
    (Look "we're" not friends "I'm" not even human)
    ("Don't" do that again)
    (Is this one of your favourite topics?))))

(define bot-seed 31415)
(define (bot-random top)
  (let ((result (remainder bot-seed top)))
    (set! bot-seed (remainder (+ bot-seed 63445217) 999999))
    result))

(define (bot-member x list)
  (if (null? list)
      #f
      (if (eqv? x (car list))
          list
          (bot-member x (cdr list)))))

(define (bot-length list)
  (if (null? list)
      0
      (+ 1 (bot-length (cdr list)))))

(define (bot-listuntil x list)
  (if (or (null? list) (eqv? x (car list)))
      '()
      (cons (car list) (bot-listuntil x (cdr list)))))

(define (bot-listrandom list)
  (list-ref list (bot-random (bot-length list))))

(define (bot-substitute n o lst)
  (if (null? lst)
      '()
      (if (not (pair? lst))
          (if (eqv? o lst)
              n
              lst)
          (cons (bot-substitute n o (car lst))
                (bot-substitute n o (cdr lst))))))

(define (bot-substitute-values vars lst)
  (map (lambda (p)
         (set! lst (bot-substitute (cadr p) (car p) lst)))
       vars)
  lst)

(define (bot-isvar? x)
  (if (symbol? x)
      (eqv? (string-ref (symbol->string x) 0) (string-ref "?" 0))
      #f))

(define (bot-ispattern? x)
  (if (and (pair? x) (not (pair? (car x))))
      (let ((str (symbol->string (car x))))
        (and (> (string-length str) 1)
             (eqv? (string-ref str 0) (string-ref "_" 0))
             (eqv? (string-ref str 1) (string-ref "_" 0))))
      #f))

(define (bot-binding var list)
  (if (null? list)
      #f
      (if (eqv? var (caar list))
          (car list)
          (bot-binding var (cdr list)))))

(define (bot-binding-value b)
  (car (cdr b)))

(define (bot-binding-lookup var list)
  (bot-binding-value (bot-binding var list)))

(define (bot-binding-with var x list)
  (cons (cons var (cons x '())) list))

(define (bot-matchvar var words list)
  (let ((b (bot-binding var list)))
    (if (not b)
        (bot-binding-with var words list)
        (if (eqv? words (bot-binding-value b))
            list
            #f))))

(define (bot-matchseg p words list)
  (let ((v (car p))
        (r (cdr p)))
    (if (null? r)
        (bot-matchvar v words list)
        (if (bot-member (car r) words)
            (bot-matchpattern r (bot-member (car r) words)
                              (bot-matchvar v (bot-listuntil (car r) words) list))
            #f))))

(define (bot-matchpattern p words list)
  (if (eqv? list #f)
      #f
      (if (bot-isvar? p)
          (bot-matchvar p words list)
          (if (eqv? p words)
              list
              (if (bot-ispattern? p)
                  (bot-matchseg p words list)
                  (if (and (pair? words) (pair? p))
                      (bot-matchpattern (cdr p) (cdr words)
                                        (bot-matchpattern (car p) (car words) list))
                      #f))))))

(define (bot-rule-getpattern r) (car r))
(define (bot-rule-getresponses r) (cdr r))

(define (bot-swappov words)
  (bot-substitute-values
   '((__your my) (__my your))
   (bot-substitute-values
    '((your __your) (my __my))
    words)))

(define (bot-answer ruleset words)
  (let ((r #f) (m #f))
    (map (lambda (current-rule)
           (if (not m)
               (begin
                 (set! r (bot-matchpattern (bot-rule-getpattern current-rule) words '()))
                 (if r
                     (set! m current-rule)
                     #f))
               #f))
         ruleset)
    (bot-substitute-values (bot-swappov r)
                          (bot-listrandom (bot-rule-getresponses m)))))

(define (bot-simplify wordstring)
  (let ((ignored-list (string->list ".?!,()[]\"'\\/"))
        (word-list (string->list wordstring)))
    (map
     (lambda (chr)
       (set! word-list (bot-substitute (string-ref " " 0) chr word-list)))
     ignored-list)
    (list->string word-list)))

(define (bot-readln-racket)
  (let ((simplified (bot-simplify (read-line))))
    (read (open-input-string (string-append "(" simplified ")")))))

(define (bot-writeln-racket l)
  (map
   (lambda (i)
     (printf "~A " i))
   l)
  (printf "\n"))

(define (bot-flatten list)
  (if (null? list)
      '()
      (if (pair? (car list))
          (append (bot-flatten (car list)) (bot-flatten (cdr list)))
          (cons (car list) (bot-flatten (cdr list))))))

(define (untitled-demo-racket)
  (printf "you> ")
  (let ((linein (bot-readln-racket)))
      (let ((lineout (bot-flatten (bot-answer untitled-ruleset linein))))
        (printf "bot> ")
        (bot-writeln-racket lineout)
        (if (or (eqv? (car lineout) 'cya) (eqv? (car lineout) 'adios))
            lineout
            (untitled-demo-racket)))))

(define (untitled-demo-r5rs)
  (display "you> ")
  (let ((linein (read)))
      (if (pair? linein)
          'good
          (set! linein (cons linein '())))
      (let ((lineout (bot-flatten (bot-answer untitled-ruleset linein))))
        (display "bot> ")
        (display lineout)
        (newline)
        (if (or (eqv? (car lineout) 'cya) (eqv? (car lineout) 'adios))
            lineout
            (untitled-demo-r5rs)))))

(display "Try typing (untitled-demo-racket) or (untitled-demo-r5rs) to start the demo!")
(display "NOTE: The r5rs version is more portable but currently requires you to type (in scheme lists)!")
(display "NOTE: Either should work in racket but it will depend on laguage mode (try Pretty Big or R5RS)")
(display "WARNING: The demo only speaks Bogan")
