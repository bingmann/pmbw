;;; Simple Memory Bandwidth Tester

bits    64
cpu     x64

extern  printf

;;; 64-bit ABI integer parameters: rdi, rsi, rdx, rcx, r8, r9

;;; Macro to push multiple registers
%macro  mpush 1-*

  %rep  %0
        push    %1
  %rotate 1
  %endrep

%endmacro

;;; Macro to pop multiple registers in reverse order
%macro  mpop 1-*

  %rep %0
  %rotate -1
        pop     %1
  %endrep

%endmacro

section .rodata
printstr_fmt0:  db "print: %s", 10, 0
printstr_fmt1:  db "print: %lld", 10, 0

;;; Define macro to printf a string
%macro printstring 1
section .rodata
%%msg:  db  %1, 0
section .text
        mpush   rdi rsi rdx rcx rax

        mov rdi, printstr_fmt0  ; format
        mov rsi, %%msg          ; string pointer for %s
        mov rax, 0              ; printf is varargs, so EAX counts # of non-integer arguments being passed
        call printf

        mpop    rdi rsi rdx rcx rax
%endmacro

;;; Define macro to printf (long long) integers from registers or more
%macro printint 1
section .text
        mpush   rdi rsi rdx rcx rax

        mov     rsi, %1             ; string pointer for %s
        mov     rdi, printstr_fmt1  ; format
        mov     rax, 0              ; printf is varargs, so EAX counts # of non-integer arguments being passed
        call    printf

        mpop    rdi rsi rdx rcx rax
%endmacro

section .text

;;; --------------------------------------------------------------------------------------------------

;;; 64-bit writer to fill a memory area
;;; Called with rdi = memarea, rsi = size
global  funcFill
funcFill:
        add     rsi, rdi        ; rsi = end iterator

.loopsize:
        mov     [rdi], rdi
        add     rdi, 8

        cmp     rdi, rsi        ; compare loop iterator
        jb      .loopsize

        ret

;;; --------------------------------------------------------------------------------------------------

;;; 64-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqWrite64PtrSimpleLoop
funcSeqWrite64PtrSimpleLoop:

        mov     rax, 0C0FFEEEEBABE0000h
        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        mov     [rcx], rax
        add     rcx, 8

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 64-bit writer in an unrolled loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqWrite64PtrUnrollLoop
funcSeqWrite64PtrUnrollLoop:

        mov     rax, 0C0FFEEEEBABE0000h
        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        mov     [rcx+0*8], rax
        mov     [rcx+1*8], rax
        mov     [rcx+2*8], rax
        mov     [rcx+3*8], rax
        mov     [rcx+4*8], rax
        mov     [rcx+5*8], rax
        mov     [rcx+6*8], rax
        mov     [rcx+7*8], rax

        mov     [rcx+8*8], rax
        mov     [rcx+9*8], rax
        mov     [rcx+10*8], rax
        mov     [rcx+11*8], rax
        mov     [rcx+12*8], rax
        mov     [rcx+13*8], rax
        mov     [rcx+14*8], rax
        mov     [rcx+15*8], rax

        add     rcx, 16*8

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 64-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqRead64PtrSimpleLoop
funcSeqRead64PtrSimpleLoop:

        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        mov	rax, [rcx]
        add     rcx, 8

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 64-bit writer in an unrolled loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqRead64PtrUnrollLoop
funcSeqRead64PtrUnrollLoop:

        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        mov     rax, [rcx+0*8]
        mov     rax, [rcx+1*8]
        mov     rax, [rcx+2*8]
        mov     rax, [rcx+3*8]
        mov     rax, [rcx+4*8]
        mov     rax, [rcx+5*8]
        mov     rax, [rcx+6*8]
        mov     rax, [rcx+7*8]

        mov     rax, [rcx+8*8]
        mov     rax, [rcx+9*8]
        mov     rax, [rcx+10*8]
        mov     rax, [rcx+11*8]
        mov     rax, [rcx+12*8]
        mov     rax, [rcx+13*8]
        mov     rax, [rcx+14*8]
        mov     rax, [rcx+15*8]

        add     rcx, 16*8

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; --------------------------------------------------------------------------------------------------

;;; 128-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqWrite128PtrSimpleLoop
funcSeqWrite128PtrSimpleLoop:

        mov     rax, 0C0FFEEEEBABE0000h
        movq    xmm0, rax
        movq    xmm1, rax
        pslldq	xmm0, 64
        por     xmm0, xmm1

        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        movdqa  [rcx], xmm0
        add     rcx, 16

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 128-bit writer in an unrolled loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqWrite128PtrUnrollLoop
funcSeqWrite128PtrUnrollLoop:

        mov     rax, 0C0FFEEEEBABE0000h
        movq    xmm0, rax
        movq    xmm1, rax
        pslldq	xmm0, 64
        por     xmm0, xmm1

        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        movdqa  [rcx+0*16], xmm0
        movdqa  [rcx+1*16], xmm0
        movdqa  [rcx+2*16], xmm0
        movdqa  [rcx+3*16], xmm0
        movdqa  [rcx+4*16], xmm0
        movdqa  [rcx+5*16], xmm0
        movdqa  [rcx+6*16], xmm0
        movdqa  [rcx+7*16], xmm0

        movdqa  [rcx+8*16], xmm0
        movdqa  [rcx+9*16], xmm0
        movdqa  [rcx+10*16], xmm0
        movdqa  [rcx+11*16], xmm0
        movdqa  [rcx+12*16], xmm0
        movdqa  [rcx+13*16], xmm0
        movdqa  [rcx+14*16], xmm0
        movdqa  [rcx+15*16], xmm0

        add     rcx, 16*16

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 128-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqRead128PtrSimpleLoop
funcSeqRead128PtrSimpleLoop:

        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        movdqa  xmm0, [rcx]
        add     rcx, 16

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 128-bit writer in an unrolled loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqRead128PtrUnrollLoop
funcSeqRead128PtrUnrollLoop:

        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        movdqa  xmm0, [rcx+0*16]
        movdqa  xmm0, [rcx+1*16]
        movdqa  xmm0, [rcx+2*16]
        movdqa  xmm0, [rcx+3*16]
        movdqa  xmm0, [rcx+4*16]
        movdqa  xmm0, [rcx+5*16]
        movdqa  xmm0, [rcx+6*16]
        movdqa  xmm0, [rcx+7*16]

        movdqa  xmm0, [rcx+8*16]
        movdqa  xmm0, [rcx+9*16]
        movdqa  xmm0, [rcx+10*16]
        movdqa  xmm0, [rcx+11*16]
        movdqa  xmm0, [rcx+12*16]
        movdqa  xmm0, [rcx+13*16]
        movdqa  xmm0, [rcx+14*16]
        movdqa  xmm0, [rcx+15*16]

        add     rcx, 16*16

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; --------------------------------------------------------------------------------------------------

;;; 64-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqWrite64IndexSimpleLoop
funcSeqWrite64IndexSimpleLoop:

        mov     rax, 0C0FFEEEEBABE0000h

.looprepeat:
        xor     rcx, rcx        ; reset index into memarea

.loopsize:
        mov     [rdi+rcx], rax
        add     rcx, 8

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 64-bit writer in an unrolled loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqWrite64IndexUnrollLoop
funcSeqWrite64IndexUnrollLoop:

        mov     rax, 0C0FFEEEEBABE0000h

.looprepeat:
        xor     rcx, rcx        ; reset index into memarea

.loopsize:
        mov     [rdi+rcx+0*8], rax
        mov     [rdi+rcx+1*8], rax
        mov     [rdi+rcx+2*8], rax
        mov     [rdi+rcx+3*8], rax
        mov     [rdi+rcx+4*8], rax
        mov     [rdi+rcx+5*8], rax
        mov     [rdi+rcx+6*8], rax
        mov     [rdi+rcx+7*8], rax

        mov     [rdi+rcx+8*8], rax
        mov     [rdi+rcx+9*8], rax
        mov     [rdi+rcx+10*8], rax
        mov     [rdi+rcx+11*8], rax
        mov     [rdi+rcx+12*8], rax
        mov     [rdi+rcx+13*8], rax
        mov     [rdi+rcx+14*8], rax
        mov     [rdi+rcx+15*8], rax

        add     rcx, 16*8

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 64-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqRead64IndexSimpleLoop
funcSeqRead64IndexSimpleLoop:

.looprepeat:
        xor     rcx, rcx        ; reset index into memarea

.loopsize:
        mov     rax, [rdi+rcx]
        add     rcx, 8

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 64-bit writer in an unrolled loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSeqRead64IndexUnrollLoop
funcSeqRead64IndexUnrollLoop:

.looprepeat:
        xor     rcx, rcx        ; reset index into memarea

.loopsize:
        mov     rax, [rdi+rcx+0*8]
        mov     rax, [rdi+rcx+1*8]
        mov     rax, [rdi+rcx+2*8]
        mov     rax, [rdi+rcx+3*8]
        mov     rax, [rdi+rcx+4*8]
        mov     rax, [rdi+rcx+5*8]
        mov     rax, [rdi+rcx+6*8]
        mov     rax, [rdi+rcx+7*8]

        mov     rax, [rdi+rcx+8*8]
        mov     rax, [rdi+rcx+9*8]
        mov     rax, [rdi+rcx+10*8]
        mov     rax, [rdi+rcx+11*8]
        mov     rax, [rdi+rcx+12*8]
        mov     rax, [rdi+rcx+13*8]
        mov     rax, [rdi+rcx+14*8]
        mov     rax, [rdi+rcx+15*8]

        add     rcx, 16*8

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; --------------------------------------------------------------------------------------------------

skiplen equ     64

;;; 64-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSkipWrite64PtrSimpleLoop
funcSkipWrite64PtrSimpleLoop:

        mov     rax, 0C0FFEEEEBABE0000h
        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        mov     [rcx], rax
        add     rcx, 8+skiplen

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 64-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSkipRead64PtrSimpleLoop
funcSkipRead64PtrSimpleLoop:

        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        mov	rax, [rcx]
        add     rcx, 8+skiplen

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 128-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSkipWrite128PtrSimpleLoop
funcSkipWrite128PtrSimpleLoop:

        mov     rax, 0C0FFEEEEBABE0000h
        movq    xmm0, rax
        movq    xmm1, rax
        pslldq	xmm0, 64
        por     xmm0, xmm1

        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        movdqa  [rcx], xmm0
        add     rcx, 16+skiplen

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 128-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSkipRead128PtrSimpleLoop
funcSkipRead128PtrSimpleLoop:

        add     rsi, rdi        ; rsi = end iterator

.looprepeat:
        mov     rcx, rdi        ; reset loop iterator

.loopsize:
        movdqa  xmm0, [rcx]
        add     rcx, 16+skiplen

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret


;;; 64-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSkipWrite64IndexSimpleLoop
funcSkipWrite64IndexSimpleLoop:

        mov     rax, 0C0FFEEEEBABE0000h

.looprepeat:
        xor     rcx, rcx        ; reset index into memarea

.loopsize:
        mov     [rdi+rcx], rax
        add     rcx, 8+skiplen

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; 64-bit writer in a simple loop
;;; Called with rdi = memarea, rsi = size, rdx = repeats
global  funcSkipRead64IndexSimpleLoop
funcSkipRead64IndexSimpleLoop:

.looprepeat:
        xor     rcx, rcx        ; reset index into memarea

.loopsize:
        mov     rax, [rdi+rcx]
        add     rcx, 8+skiplen

        cmp     rcx, rsi        ; compare loop iterator
        jb      .loopsize

        dec     rdx
        jnz     .looprepeat

        ret

;;; --------------------------------------------------------------------------------------------------

;;; Follow 64-bit permutation in a simple loop
;;; Called with rdi = memarea (permutation start), rsi = dummy, rdx = repeats
global  funcPermRead64SimpleLoop
funcPermRead64SimpleLoop:

        mov     rcx, rdi        ; reset to permutation start

.looprepeat:
        ;; dont need to reset rcx = rdi

.permloop:
        mov     rcx, [rcx]      ; read next pointer

        cmp     rcx, rdi
        jne     .permloop        ; loop until start pointer is reached

        dec     rdx
        jnz     .looprepeat

        ret

;;; Follow 64-bit permutation in an unrolled loop
;;; Called with rdi = memarea (permutation start), rsi = dummy, rdx = repeats
global  funcPermRead64UnrollLoop
funcPermRead64UnrollLoop:

        mov     rcx, rdi        ; reset to permutation start

.looprepeat:
        ;; dont need to reset rcx = rdi

.permloop:
        mov     rcx, [rcx]      ; read next pointer
        mov     rcx, [rcx]
        mov     rcx, [rcx]
        mov     rcx, [rcx]
        mov     rcx, [rcx]
        mov     rcx, [rcx]
        mov     rcx, [rcx]
        mov     rcx, [rcx]

        mov     rcx, [rcx]
        mov     rcx, [rcx]
        mov     rcx, [rcx]
        mov     rcx, [rcx]
        mov     rcx, [rcx]
        mov     rcx, [rcx]
        mov     rcx, [rcx]
        mov     rcx, [rcx]

        cmp     rcx, rdi
        jne     .permloop        ; loop until start pointer is reached

        dec     rdx
        jnz     .looprepeat

        ret
