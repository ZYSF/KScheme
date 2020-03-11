/* KScheme, a BRUTAL EDIT of MiniScheme, by Zak Fenton MMXX
 * CHANGES from Mini-Scheme 0.85 to KScheme 0.1:
 *  - Fixed up some old-style C code that modern/C++ compilers complain about (mostly changes to function definitions, now compiles in Visual Studio 2019)
 *  - Removed copyrighted parts of init.scm (assuming the sections with copyright notices aren't covered by the public domain dedication in this file)
 *  - Made partly embeddable (all globals now stored in a struct passed between each function, each function now has kscm_ prefix with an extra underscore
 *    for most of them to indicate that it's part of a private API for now)
 *  - Changed name (Mini-Scheme -> KScheme, as in "kernel scheme")
 *  - Maybe more features coming...
 *
 *      ---------- Mini-Scheme Interpreter Version 0.85 ----------
 *
 *                coded by Atsushi Moriwaki (11/5/1989)
 *
 *            E-MAIL :  moriwaki@kurims.kurims.kyoto-u.ac.jp
 *
 *               THIS SOFTWARE IS IN THE PUBLIC DOMAIN
 *               ------------------------------------
 * This software is completely free to copy, modify and/or re-distribute.
 * But I would appreciate it if you left my name on the code as the author.
 *
 */
 /*--
  *
  *  This version has been modified by Chris Pressey.
  *	current version is 0.85p1 (as yet unreleased)
  *
  *  This version has been modified by R.C. Secrist.
  *
  *  Mini-Scheme is now maintained by Akira KIDA.
  *
  *  This is a revised and modified version by Akira KIDA.
  *	current version is 0.85k4 (15 May 1994)
  *
  *  Please send suggestions, bug reports and/or requests to:
  *		<SDI00379@niftyserve.or.jp>
  *--
  */
#define KSCM_PLATFORM_BSD
#define KSCM_PLUSPLUS
// This just allows unsafe legacy C functions in Visual Studio. It should be disabled and fixed properly but not the highest priority.
#define _CRT_SECURE_NO_WARNINGS

#ifdef KSCM_PLUSPLUS
extern "C" {
#endif
  /*
   * Here is System declaration.
   * Please define exactly one symbol in the following section.
   */
   /* #define LSC		*/	/* LightSpeed C for Macintosh */
   /* #define LSC4		*/	/* THINK C version 4.0 for Macintosh */
   /* #define MPW2		*/	/* Macintosh Programmer's Workshop v2.0x */
   /* #define KSCM_PLATFORM_BSD		*/	/* 4.x KSCM_PLATFORM_BSD */
   /* #define MSC		*/	/* Microsoft C Compiler v.4.00 - 7.00 */
   /* #define KSCM_PLATFORM_TURBOC	*/	/* Turbo C compiler v.2.0, or TC++ 1.0  */
   /* #define SYSV		*/	/* System-V, or POSIX */
   /* #define KSCM_PLATFORM_VAXC		*/	/* VAX/VMS KSCM_PLATFORM_VAXC 2.x or later */ /* (automatic) */

#ifdef __BORLANDC__	/* Borland C++ - MS-DOS */
#define KSCM_PLATFORM_TURBOC
#endif

#ifdef __TURBOC__	/* Turbo C V1.5 - MS-DOS */
#define KSCM_PLATFORM_TURBOC
#endif

#ifdef mips		/* DECstation running OSF/1 */
#define KSCM_PLATFORM_BSD
#endif

#ifdef __osf__		/* Alpha AXP running OSF/1 */
#define KSCM_PLATFORM_BSD
#endif

#ifdef __DECC		/* Alpha AXP running VMS */
#define KSCM_PLATFORM_VAXC
#endif

#ifdef _AIX		/* RS/6000 running AIX */
#define KSCM_PLATFORM_BSD
#endif

/*
 * Define or undefine following symbols as you need.
 */
 /* #define VERBOSE */	/* define this if you want verbose GC */
#define	KSCM_CONFIG_AVOID_HACK_LOOP	/* define this if your compiler is poor
			 * enougth to complain "do { } while (0)"
			 * construction.
			 */
#define KSCM_CONFIG_USE_SETJMP	/* undef this if you do not want to use setjmp() */
#define KSCM_CONFIG_USE_QQUOTE	/* undef this if you do not need quasiquote */
#define KSCM_CONFIG_USE_MACRO	/* undef this if you do not need macro */


#ifdef KSCM_CONFIG_USE_QQUOTE
			 /*--
			  *  If your machine can't support "forward single quotation character"
			  *  i.e., '`',  you may have trouble to use backquote.
			  *  So use '^' in place of '`'.
			  */
# define BACKQUOTE '`'
#endif

			  /*
			   *  Basic memory allocation units
			   */

#ifdef KSCM_PLATFORM_TURBOC             	/* rcs */
#define KSCM_CONFIG_CELL_SEGSIZE  2048
#define KSCM_CONFIG_CELL_NSEGMENT  100
#define KSCM_CONFIG_STR_SEGSIZE   2048
#define KSCM_CONFIG_STR_NSEGMENT   100
#else
#define KSCM_CONFIG_CELL_SEGSIZE    5000	/* # of cells in one segment */
#define KSCM_CONFIG_CELL_NSEGMENT   100	/* # of segments for cells */
#define KSCM_CONFIG_STR_SEGSIZE     2500	/* bytes of one string segment */
#define KSCM_CONFIG_STR_NSEGMENT    100	/* # of segments for strings */
#endif



#define KSCM_CONFIG_BANNER "Hello, This is KScheme (kscm) 0.1, based on Mini-Scheme Interpreter Version 0.85p1.\n"


#include <stdio.h>
#include <ctype.h>
#ifdef KSCM_CONFIG_USE_SETJMP
#include <setjmp.h>
#endif


			   /* System dependency */
#ifdef LSC
#include <strings.h>
#include <unix.h>
#define malloc(x)	NewPtr((long)(x))
#define KSCM_CONFIG_PROMPT "> "
#define KSCM_CONFIG_INITFILE "init.scm"
#define KSCM_CONFIG_FIRST_CELLSEGS 5
#endif

#ifdef LSC4
#include <string.h>
#include <stdlib.h>
#define malloc(x)	NewPtr((long)(x))
#define KSCM_CONFIG_PROMPT "> "
#define KSCM_CONFIG_INITFILE "init.scm"
#define KSCM_CONFIG_FIRST_CELLSEGS 5
#endif

#ifdef MPW2
#include <strings.h>
#include <memory.h>
#define malloc(x)	NewPtr((long)(x))
#define KSCM_CONFIG_PROMPT "> [enter at next line]\n"
#define KSCM_CONFIG_INITFILE "init.scm"
#define KSCM_CONFIG_FIRST_CELLSEGS 5
#endif

#ifdef KSCM_PLATFORM_BSD
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#define KSCM_CONFIG_PROMPT "> "
#define KSCM_CONFIG_INITFILE "init.scm"
#define KSCM_CONFIG_FIRST_CELLSEGS 10
#endif

// Old definition, no detection
#ifdef MSC
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <process.h>
#define KSCM_CONFIG_PROMPT "> "
#define KSCM_CONFIG_INITFILE "init.scm"
#define KSCM_CONFIG_FIRST_CELLSEGS 3
#endif

#ifdef KSCM_PLATFORM_TURBOC
#include <string.h>
#include <stdlib.h>
#define KSCM_CONFIG_PROMPT "> "
#define KSCM_CONFIG_INITFILE "init.scm"
#define KSCM_CONFIG_FIRST_CELLSEGS 3
#endif

// Old definition, no detection
#ifdef SYSV
#include <string.h>
#include <malloc.h>
#if __STDC__
# include <stdlib.h>
#endif
#define KSCM_CONFIG_PROMPT "> "
#define KSCM_CONFIG_INITFILE "init.scm"
#define KSCM_CONFIG_FIRST_CELLSEGS 10
#endif

#ifdef	KSCM_PLATFORM_VAXC
#include <string.h>
#include <stdlib.h>
#define KSCM_CONFIG_PROMPT "> "
#define KSCM_CONFIG_INITFILE "init.scm"
#define KSCM_CONFIG_FIRST_CELLSEGS 10
#endif

#ifdef __GNUC__
/*
 * If we use gcc, KSCM_CONFIG_AVOID_HACK_LOOP is unnecessary
 */
#undef KSCM_CONFIG_AVOID_HACK_LOOP
#endif

#ifndef	KSCM_CONFIG_FIRST_CELLSEGS
#error Please define your system type.
 /*
  * We refrain this to raise an error anyway even if on pre-ANSI system.
  */
error Please define your system type.
#endif

typedef struct kscm kscm_t;

/* cell structure */
struct kscm_cell {
	unsigned short _flag;
	union {
		struct {
			char* _svalue;
			short   _keynum;
		} _string;
		struct {
			long    _ivalue;
		} _number;
		struct {
			struct kscm_cell* _car;
			struct kscm_cell* _cdr;
		} _cons;
	} _object;
};

typedef struct kscm_cell* kscm_object_t;

#define T_STRING         1	/* 0000000000000001 */
#define T_NUMBER         2	/* 0000000000000010 */
#define T_SYMBOL         4	/* 0000000000000100 */
#define T_SYNTAX         8	/* 0000000000001000 */
#define T_PROC          16	/* 0000000000010000 */
#define T_PAIR          32	/* 0000000000100000 */
#define T_CLOSURE       64	/* 0000000001000000 */
#define T_CONTINUATION 128	/* 0000000010000000 */
#ifdef KSCM_CONFIG_USE_MACRO
# define T_MACRO        256	/* 0000000100000000 */
#endif
#define T_PROMISE      512	/* 0000001000000000 */
#define T_ATOM       16384	/* 0100000000000000 */	/* only for gc */
#define CLRATOM      49151	/* 1011111111111111 */	/* only for gc */
#define MARK         32768	/* 1000000000000000 */
#define UNMARK       32767	/* 0111111111111111 */

/* macros for cell operations */
#define kscm__type(kscm,p)         ((p)->_flag)

#define kscm__isstring(kscm,p)     (kscm__type(kscm, p)&T_STRING)
#define kscm__strvalue(kscm,p)     ((p)->_object._string._svalue)
#define kscm__keynum(kscm,p)       ((p)->_object._string._keynum)

#define kscm__isnumber(kscm,p)     (kscm__type(kscm, p)&T_NUMBER)
#define kscm__ivalue(kscm,p)       ((p)->_object._number._ivalue)

#define kscm__ispair(kscm,p)       (kscm__type(kscm,p)&T_PAIR)
#define kscm__car(kscm,p)          ((p)->_object._cons._car)
#define kscm__cdr(kscm,p)          ((p)->_object._cons._cdr)

#define kscm__issymbol(kscm,p)     (kscm__type(kscm, p)&T_SYMBOL)
#define kscm__symname(kscm,p)      kscm__strvalue(kscm, kscm__car(kscm, p))
#define kscm__hasprop(kscm,p)      (kscm__type(kscm, p)&T_SYMBOL)
#define kscm__symprop(kscm,p)      kscm__cdr(kscm, p)

#define kscm__issyntax(kscm,p)     (kscm__type(kscm, p)&T_SYNTAX)
#define kscm__isproc(kscm,p)       (kscm__type(kscm, p)&T_PROC)
#define kscm__syntaxname(kscm,p)   kscm__strvalue(kscm, kscm__car(kscm, p))
#define kscm__syntaxnum(kscm,p)    kscm__keynum(kscm, kscm__car(kscm, p))
#define kscm__procnum(kscm,p)      kscm__ivalue(kscm, p)

#define kscm__isclosure(kscm,p)    (kscm__type(kscm, p)&T_CLOSURE)
#ifdef KSCM_CONFIG_USE_MACRO
# define kscm__ismacro(kscm,p)      (kscm__type(kscm, p)&T_MACRO)
#endif
#define kscm__closure_code(kscm,p) kscm__car(kscm, p)
#define kscm__closure_env(kscm,p)  kscm__cdr(kscm, p)

#define kscm__iscontinuation(kscm,p) (kscm__type(kscm, p)&T_CONTINUATION)
#define kscm__cont_dump(kscm,p)    kscm__cdr(kscm, p)

#define kscm__ispromise(kscm,p)    (kscm__type(kscm,p)&T_PROMISE)
#define kscm__setpromise(kscm,p)   kscm__type(kscm,p) |= T_PROMISE

#define kscm__isatom(kscm,p)       (kscm__type(kscm,p)&T_ATOM)
#define kscm__setatom(kscm,p)      kscm__type(kscm,p) |= T_ATOM
#define kscm__clratom(kscm,p)      kscm__type(kscm,p) &= CLRATOM

#define kscm__ismark(kscm,p)       (kscm__type(kscm, p)&MARK)
#define kscm__setmark(kscm,p)      kscm__type(kscm, p) |= MARK
#define kscm__clrmark(kscm,p)      kscm__type(kscm, p) &= UNMARK

#define kscm__caar(kscm, p)         kscm__car(kscm, kscm__car(kscm, p))
#define kscm__cadr(kscm, p)         kscm__car(kscm, kscm__cdr(kscm, p))
#define kscm__cdar(kscm, p)         kscm__cdr(kscm, kscm__car(kscm, p))
#define kscm__cddr(kscm, p)         kscm__cdr(kscm, kscm__cdr(kscm, p))
#define kscm__cadar(kscm, p)        kscm__car(kscm, kscm__cdr(kscm, kscm__car(kscm, p)))
#define kscm__caddr(kscm, p)        kscm__car(kscm, kscm__cdr(kscm, kscm__cdr(kscm, p)))
#define kscm__cadaar(kscm, p)       kscm__car(kscm, kscm__cdr(kscm, kscm__car(kscm, kscm__car(kscm, p))))
#define kscm__cadddr(kscm, p)       kscm__car(kscm, kscm__cdr(kscm, kscm__cdr(kscm, kscm__cdr(kscm, p))))
#define kscm__cddddr(kscm, p)       kscm__cdr(kscm, kscm__cdr(kscm, kscm__cdr(kscm, kscm__cdr(kscm, p))))

#define LINESIZE 1024

struct kscm {
	/* arrays for segments */
	kscm_object_t cell_seg[KSCM_CONFIG_CELL_NSEGMENT];
	int     last_cell_seg = -1;
	char* str_seg[KSCM_CONFIG_STR_NSEGMENT];
	int     str_seglast = -1;

	/* We use 4 registers. */
	kscm_object_t args;			/* register for arguments of function */
	kscm_object_t envir;			/* stack register for current environment */
	kscm_object_t code;			/* register for current code */
	kscm_object_t dump;			/* stack register for next evaluation */

	struct kscm_cell _NIL;
	kscm_object_t NIL = &_NIL;		/* special cell representing empty cell */
	struct kscm_cell _T;
	kscm_object_t T = &_T;		/* special cell representing #t */
	struct kscm_cell _F;
	kscm_object_t F = &_F;		/* special cell representing #f */
	kscm_object_t oblist = &_NIL;		/* pointer to symbol table */
	kscm_object_t global_env;		/* pointer to global environment */

	/* global pointers to special symbols */
	kscm_object_t LAMBDA;			/* pointer to syntax lambda */
	kscm_object_t QUOTE;			/* pointer to syntax quote */

#ifdef KSCM_CONFIG_USE_QQUOTE
	kscm_object_t QQUOTE;			/* pointer to symbol quasiquote */
	kscm_object_t UNQUOTE;		/* pointer to symbol unquote */
	kscm_object_t UNQUOTESP;		/* pointer to symbol unquote-splicing */

#endif

	kscm_object_t free_cell = &_NIL;	/* pointer to top of free cells */
	long    fcells = 0;		/* # of free cells */

	FILE* infp;			/* input file */
	FILE* outfp;			/* output file */

#ifdef KSCM_CONFIG_USE_SETJMP
	jmp_buf error_jmp;

#endif
	char    gc_verbose;		/* if gc_verbose is not zero, print gc status */
	int     quiet = 0;		/* if not zero, print KSCM_CONFIG_BANNER, KSCM_CONFIG_PROMPT, results */
	int     all_errors_fatal = 0;   /* if not zero, every error is a FatalError */

	FILE* tmpfp;
	int tok;
	int print_flag;
	kscm_object_t value;
	short _operator;

	char    linebuff[LINESIZE];
	char    strbuff[256];
	char* currentline = linebuff;
	char* endline = linebuff;
};

/* allocate new cell segment */
int kscm__alloc_cellseg(kscm_t *kscm, int n)
{
	register kscm_object_t p;
	register long i;
	register int k;

	for (k = 0; k < n; k++) {
		if (kscm->last_cell_seg >= KSCM_CONFIG_CELL_NSEGMENT - 1)
			return k;
		p = (kscm_object_t)malloc(KSCM_CONFIG_CELL_SEGSIZE * sizeof(struct kscm_cell));
		if (p == (kscm_object_t)0)
			return k;
		kscm->cell_seg[++kscm->last_cell_seg] = p;
		kscm->fcells += KSCM_CONFIG_CELL_SEGSIZE;
		for (i = 0; i < KSCM_CONFIG_CELL_SEGSIZE - 1; i++, p++) {
			kscm__type(kscm, p) = 0;
			kscm__car(kscm, p) = kscm->NIL;
			kscm__cdr(kscm, p) = p + 1;
		}
		kscm__type(kscm, p) = 0;
		kscm__car(kscm, p) = kscm->NIL;
		kscm__cdr(kscm, p) = kscm->free_cell;
		kscm->free_cell = kscm->cell_seg[kscm->last_cell_seg];
	}
	return n;
}

/* allocate new string segment */
int kscm__alloc_strseg(kscm_t* kscm, int n)
{
	register char* p;
	register long i;
	register int k;

	for (k = 0; k < n; k++) {
		if (kscm->str_seglast >= KSCM_CONFIG_STR_NSEGMENT)
			return k;
		p = (char*)malloc(KSCM_CONFIG_STR_SEGSIZE * sizeof(char));
		if (p == (char*)0)
			return k;
		kscm->str_seg[++kscm->str_seglast] = p;
		for (i = 0; i < KSCM_CONFIG_STR_SEGSIZE; i++)
			*p++ = (char)(-1);
	}
	return n;
}

void kscm__fatal_error(kscm_t* kscm, const char* msg, const char* a, const char* b, const char* c);
void kscm__error(kscm_t* kscm, const char* msg, const char* a, const char* b, const char* c);
void kscm__init_globals(kscm_t* kscm);

/* initialization of Mini-Scheme */
void kscm__init_scheme(kscm_t* kscm)
{
	register kscm_object_t i;

	kscm->last_cell_seg = -1;
	kscm->str_seglast = -1;
	kscm->NIL = &kscm->_NIL;
	kscm->T = &kscm->_T;
	kscm->F = &kscm->_F;
	kscm->oblist = &kscm->_NIL;
	kscm->free_cell = &kscm->_NIL;

	if (kscm__alloc_cellseg(kscm, KSCM_CONFIG_FIRST_CELLSEGS) != KSCM_CONFIG_FIRST_CELLSEGS)
		kscm__fatal_error(kscm, "Unable to allocate initial cell segments", NULL, NULL, NULL);
	if (!kscm__alloc_strseg(kscm, 1))
		kscm__fatal_error(kscm, "Unable to allocate initial string segments", NULL, NULL, NULL);
#ifdef VERBOSE
	kscm->gc_verbose = 1;
#else
	kscm->gc_verbose = 0;
#endif
	kscm__init_globals(kscm);
}

void kscm__gc(kscm_t* kscm, register kscm_object_t a, register kscm_object_t b);

/* get new cell.  parameter a, b is marked by gc. */
kscm_object_t kscm__get_cell(kscm_t* kscm, register kscm_object_t a, register kscm_object_t b)
{
	register kscm_object_t x;

	if (kscm->free_cell == kscm->NIL) {
		kscm__gc(kscm, a, b);
		if (kscm->free_cell == kscm->NIL)
#ifdef KSCM_CONFIG_USE_SETJMP
			if (!kscm__alloc_cellseg(kscm, 1)) {
				kscm->args = kscm->envir = kscm->code = kscm->dump = kscm->NIL;
				kscm__gc(kscm, kscm->NIL, kscm->NIL);
				if (kscm->free_cell != kscm->NIL)
					kscm__error(kscm, "run out of cells --- rerurn to top level", NULL, NULL, NULL);
				else
					kscm__fatal_error(kscm, "run out of cells --- unable to recover cells", NULL, NULL, NULL);
			}
#else
			if (!alloc_cellseg(kscm, 1))
				FatalError(kscm, "run out of cells  --- unable to recover cells", NULL, NULL, NULL);
#endif
	}
	x = kscm->free_cell;
	kscm->free_cell = kscm__cdr(kscm, x);
	--kscm->fcells;
	return (x);
}

/* get new cons cell */
kscm_object_t kscm__cons(kscm_t* kscm, register kscm_object_t a, register kscm_object_t b)
{
	register kscm_object_t x = kscm__get_cell(kscm, a, b);

	kscm__type(kscm, x) = T_PAIR;
	kscm__car(kscm, x) = a;
	kscm__cdr(kscm, x) = b;
	return (x);
}

/* get number atom */
kscm_object_t kscm__mk_number(kscm_t* kscm, register long num)
{
	register kscm_object_t x = kscm__get_cell(kscm, kscm->NIL, kscm->NIL);

	kscm__type(kscm, x) = (T_NUMBER | T_ATOM);
	kscm__ivalue(kscm, x) = num;
	return (x);
}

/* allocate name to string area */
char* kscm__store_string(kscm_t* kscm, const char *name)
{
	register char* q = NULL;
	register short i;
	long    len, remain;

	/* first check name has already listed */
	for (i = 0; i <= kscm->str_seglast; i++)
		for (q = kscm->str_seg[i]; *q != (char)(-1); ) {
			if (!strcmp(q, name))
				goto FOUND;
			while (*q++)
				;	/* get next string */
		}
	len = strlen(name) + 2;
	remain = (long)KSCM_CONFIG_STR_SEGSIZE - ((long)q - (long)kscm->str_seg[kscm->str_seglast]);
	if (remain < len) {
		if (!kscm__alloc_strseg(kscm, 1))
			kscm__fatal_error(kscm, "run out of string area", NULL, NULL, NULL);
		q = kscm->str_seg[kscm->str_seglast];
	}
	strcpy(q, name);
FOUND:
	return (q);
}

/* get new string */
kscm_object_t kscm__mk_string(kscm_t* kscm, const char *str)
{
	register kscm_object_t x = kscm__get_cell(kscm, kscm->NIL, kscm->NIL);

	kscm__strvalue(kscm, x) = kscm__store_string(kscm, str);
	kscm__type(kscm, x) = (T_STRING | T_ATOM);
	kscm__keynum(kscm, x) = (short)(-1);
	return (x);
}

/* get new symbol */
kscm_object_t kscm__mk_symbol(kscm_t* kscm, const char *name)
{
	register kscm_object_t x;

	/* fisrt check oblist */
	for (x = kscm->oblist; x != kscm->NIL; x = kscm__cdr(kscm, x))
		if (!strcmp(name, kscm__symname(kscm, kscm__car(kscm, x))))
			break;

	if (x != kscm->NIL)
		return (kscm__car(kscm, x));
	else {
		x = kscm__cons(kscm, kscm__mk_string(kscm, name), kscm->NIL);
		kscm__type(kscm, x) = T_SYMBOL;
		kscm->oblist = kscm__cons(kscm, x, kscm->oblist);
		return (x);
	}
}

/* make symbol or number atom from string */
kscm_object_t kscm__mk_atom(kscm_t* kscm, const char *q)
{
	char    c;
	const char *p;

	p = q;
	if (!isdigit(c = *p++)) {
		if ((c != '+' && c != '-') || !isdigit(*p))
			return (kscm__mk_symbol(kscm, q));
	}
	for (; (c = *p) != 0; ++p)
		if (!isdigit(c))
			return (kscm__mk_symbol(kscm, q));
	return (kscm__mk_number(kscm, atol(q)));
}

/* make constant */
kscm_object_t kscm__mk_const(kscm_t* kscm, const char *name)
{
	long    x;
	char    tmp[256];

	if (!strcmp(name, "t"))
		return (kscm->T);
	else if (!strcmp(name, "f"))
		return (kscm->F);
	else if (*name == 'o') {/* #o (octal) */
		sprintf(tmp, "0%s", &name[1]);
		sscanf(tmp, "%lo", (unsigned long int*) & x);
		return (kscm__mk_number(kscm, x));
	}
	else if (*name == 'd') {	/* #d (decimal) */
		sscanf(&name[1], "%ld", &x);
		return (kscm__mk_number(kscm, x));
	}
	else if (*name == 'x') {	/* #x (hex) */
		sprintf(tmp, "0x%s", &name[1]);
		sscanf(tmp, "%lx", (unsigned long int*) & x);
		return (kscm__mk_number(kscm, x));
	}
	else
		return (kscm->NIL);
}


/* ========== garbage collector ========== */

/*--
 *  We use algorithm E (Kunuth, The Art of Computer Programming Vol.1,
 *  sec.3.5) for marking.
 */
void kscm__mark(kscm_t* kscm, kscm_object_t a)
{
	register kscm_object_t t, q, p;

E1:	t = (kscm_object_t)0;
	p = a;
E2:	kscm__setmark(kscm, p);
E3:	if (kscm__isatom(kscm, p))
goto E6;
E4:	q = kscm__car(kscm, p);
if (q && !kscm__ismark(kscm, q)) {
	kscm__setatom(kscm, p);
	kscm__car(kscm, p) = t;
	t = p;
	p = q;
	goto E2;
}
E5:	q = kscm__cdr(kscm, p);
if (q && !kscm__ismark(kscm, q)) {
	kscm__cdr(kscm, p) = t;
	t = p;
	p = q;
	goto E2;
}
E6:	if (!t)
return;
q = t;
if (kscm__isatom(kscm, q)) {
	kscm__clratom(kscm, q);
	t = kscm__car(kscm, q);
	kscm__car(kscm, q) = p;
	p = q;
	goto E5;
}
else {
	t = kscm__cdr(kscm, q);
	kscm__cdr(kscm, q) = p;
	p = q;
	goto E6;
}
}


/* garbage collection. parameter a, b is marked. */
void kscm__gc(kscm_t* kscm, register kscm_object_t a, register kscm_object_t b)
{
	register kscm_object_t p;
	register short i;
	register long j;

	if (kscm->gc_verbose)
		printf("gc...");

	/* mark system globals */
	kscm__mark(kscm, kscm->oblist);
	kscm__mark(kscm, kscm->global_env);

	/* mark current registers */
	kscm__mark(kscm, kscm->args);
	kscm__mark(kscm, kscm->envir);
	kscm__mark(kscm, kscm->code);
	kscm__mark(kscm, kscm->dump);

	/* mark variables a, b */
	kscm__mark(kscm, a);
	kscm__mark(kscm, b);

	/* garbage collect */
	kscm__clrmark(kscm, kscm->NIL);
	kscm->fcells = 0;
	kscm->free_cell = kscm->NIL;
	for (i = 0; i <= kscm->last_cell_seg; i++) {
		for (j = 0, p = kscm->cell_seg[i]; j < KSCM_CONFIG_CELL_SEGSIZE; j++, p++) {
			if (kscm__ismark(kscm, p))
				kscm__clrmark(kscm, p);
			else {
				kscm__type(kscm, p) = 0;
				kscm__cdr(kscm, p) = kscm->free_cell;
				kscm__car(kscm, p) = kscm->NIL;
				kscm->free_cell = p;
				++kscm->fcells;
			}
		}
	}

	if (kscm->gc_verbose)
		printf(" done %ld cells are recovered.\n", kscm->fcells);
}


/* ========== Rootines for Reading ========== */

#define TOK_LPAREN  0
#define TOK_RPAREN  1
#define TOK_DOT     2
#define TOK_ATOM    3
#define TOK_QUOTE   4
#define TOK_COMMENT 5
#define TOK_DQUOTE  6
#ifdef KSCM_CONFIG_USE_QQUOTE
# define TOK_BQUOTE  7
# define TOK_COMMA   8
# define TOK_ATMARK  9
#endif
#define TOK_SHARP   10

/* get new character from input file */
int     kscm__inchar(kscm_t* kscm)
{
	if (kscm->currentline >= kscm->endline) {	/* input buffer is empty */
		if (feof(kscm->infp)) {
			fclose(kscm->infp);
			kscm->infp = stdin;
			if (!kscm->quiet)
				printf(KSCM_CONFIG_PROMPT);
		}
		strcpy(kscm->linebuff, "\n");
		if (fgets(kscm->currentline = kscm->linebuff, LINESIZE, kscm->infp) == NULL)
			if (kscm->infp == stdin) {
				if (!kscm->quiet)
					fprintf(stderr, "Good-bye\n");
				exit(0);
			}
		kscm->endline = kscm->linebuff + strlen(kscm->linebuff);
	}
	return (*kscm->currentline++);
}

/* clear input buffer */
void kscm__clearinput(kscm_t* kscm)
{
	kscm->currentline = kscm->endline = kscm->linebuff;
}

/* back to standard input */
void kscm__flushinput(kscm_t* kscm)
{
	if (kscm->infp != stdin) {
		fclose(kscm->infp);
		kscm->infp = stdin;
	}
	kscm__clearinput(kscm);
}

/* back character to input buffer */
void kscm__backchar(kscm_t* kscm)
{
	kscm->currentline--;
}

int kscm__isdelim(kscm_t* kscm, const char* s, char c);

/* read chacters to delimiter */
char* kscm__readstr(kscm_t* kscm, const char *delim)
{
	char* p = kscm->strbuff;

	while (kscm__isdelim(kscm, delim, (*p++ = kscm__inchar(kscm))))
		;
	kscm__backchar(kscm);
	*--p = '\0';
	return (kscm->strbuff);
}

/* read string expression "xxx...xxx" */
char* kscm__readstrexp(kscm_t* kscm)
{
	char    c, * p = kscm->strbuff;

	for (;;) {
		if ((c = kscm__inchar(kscm)) != '"')
			*p++ = c;
		else if (p > kscm->strbuff&&* (p - 1) == '\\')
			*(p - 1) = '"';
		else {
			*p = '\0';
			return (kscm->strbuff);
		}
	}
}

/* check c is delimiter */
int kscm__isdelim(kscm_t* kscm, const char *s, char c)
{
	while (*s)
		if (*s++ == c)
			return (0);
	return (1);
}

/* skip white characters */
void kscm__skipspace(kscm_t* kscm)
{
	while (isspace(kscm__inchar(kscm)))
		;
	kscm__backchar(kscm);
}

/* get token */
int kscm__token(kscm_t* kscm)
{
	kscm__skipspace(kscm);
	switch (kscm__inchar(kscm)) {
	case '(':
		return (TOK_LPAREN);
	case ')':
		return (TOK_RPAREN);
	case '.':
		return (TOK_DOT);
	case '\'':
		return (TOK_QUOTE);
	case ';':
		return (TOK_COMMENT);
	case '"':
		return (TOK_DQUOTE);
#ifdef KSCM_CONFIG_USE_QQUOTE
	case BACKQUOTE:
		return (TOK_BQUOTE);
	case ',':
		if (kscm__inchar(kscm) == '@')
			return (TOK_ATMARK);
		else {
			kscm__backchar(kscm);
			return (TOK_COMMA);
		}
#endif
	case '#':
		return (TOK_SHARP);
	default:
		kscm__backchar(kscm);
		return (TOK_ATOM);
	}
}

/* ========== Rootines for Printing ========== */
#define	kscm__ok_abbrev(kscm,x)	(kscm__ispair(kscm, x) && kscm__cdr(kscm, x) == kscm->NIL)

void kscm__strunquote(kscm_t* kscm, char *p, const char *s)
{
	*p++ = '"';
	for (; *s; ++s) {
		if (*s == '"') {
			*p++ = '\\';
			*p++ = '"';
		}
		else if (*s == '\n') {
			*p++ = '\\';
			*p++ = 'n';
		}
		else
			*p++ = *s;
	}
	*p++ = '"';
	*p = '\0';
}


/* print atoms */
int kscm__printatom(kscm_t* kscm, kscm_object_t l, int f)
{
	char *p = NULL;

	if (l == kscm->NIL)
		p = (char*)(void*)"()";
	else if (l == kscm->T)
		p = (char*)(void*)"#t";
	else if (l == kscm->F)
		p = (char*)(void*)"#f";
	else if (kscm__isnumber(kscm, l)) {
		p = kscm->strbuff;
		sprintf(p, "%ld", kscm__ivalue(kscm, l));
	}
	else if (kscm__isstring(kscm, l)) {
		if (!f)
			p = kscm__strvalue(kscm, l);
		else {
			p = kscm->strbuff;
			kscm__strunquote(kscm, p, kscm__strvalue(kscm, l));
		}
	}
	else if (kscm__issymbol(kscm, l))
		p = kscm__symname(kscm, l);
	else if (kscm__isproc(kscm, l)) {
		p = kscm->strbuff;
		sprintf(p, "#<PROCEDURE %ld>", kscm__procnum(kscm, l));
#ifdef KSCM_CONFIG_USE_MACRO
	}
	else if (kscm__ismacro(kscm, l)) {
		p = (char*)(void*)"#<MACRO>";
#endif
	}
	else if (kscm__isclosure(kscm, l))
		p = (char*)(void*)"#<CLOSURE>";
	else if (kscm__iscontinuation(kscm, l))
		p = (char*)(void*)"#<CONTINUATION>";
	if (f < 0)
		return strlen(p);
	fputs(p, kscm->outfp);
	return 0;
}


/* ========== Rootines for Evaluation Cycle ========== */

/* make closure. c is code. e is environment */
kscm_object_t kscm__mk_closure(kscm_t* kscm, register kscm_object_t c, register kscm_object_t e)
{
	register kscm_object_t x = kscm__get_cell(kscm, c, e);

	kscm__type(kscm, x) = T_CLOSURE;
	kscm__car(kscm, x) = c;
	kscm__cdr(kscm, x) = e;
	return (x);
}

/* make continuation. */
kscm_object_t kscm__mk_continuation(kscm_t* kscm, register kscm_object_t d)
{
	register kscm_object_t x = kscm__get_cell(kscm, kscm->NIL, d);

	kscm__type(kscm, x) = T_CONTINUATION;
	kscm__cont_dump(kscm, x) = d;
	return (x);
}

/* reverse list -- make new cells */
kscm_object_t kscm__reverse(kscm_t* kscm, register kscm_object_t a)		/* a must be checked by gc */
{
	register kscm_object_t p = kscm->NIL;

	for (; kscm__ispair(kscm, a); a = kscm__cdr(kscm, a))
		p = kscm__cons(kscm, kscm__car(kscm, a), p);
	return (p);
}

/* reverse list --- no make new cells */
kscm_object_t kscm__non_alloc_rev(kscm_t* kscm, kscm_object_t term, kscm_object_t list)
{
	register kscm_object_t p = list, result = term, q;

	while (p != kscm->NIL) {
		q = kscm__cdr(kscm, p);
		kscm__cdr(kscm, p) = result;
		result = p;
		p = q;
	}
	return (result);
}

/* append list -- make new cells */
kscm_object_t kscm__append(kscm_t* kscm, register kscm_object_t a, register kscm_object_t b)
{
	register kscm_object_t p = b, q;

	if (a != kscm->NIL) {
		a = kscm__reverse(kscm, a);
		while (a != kscm->NIL) {
			q = kscm__cdr(kscm, a);
			kscm__cdr(kscm, a) = p;
			p = a;
			a = q;
		}
	}
	return (p);
}

/* equivalence of atoms */
int kscm__eqv(kscm_t* kscm, register kscm_object_t a, register kscm_object_t b)
{
	if (kscm__isstring(kscm, a)) {
		if (kscm__isstring(kscm, b))
			return (kscm__strvalue(kscm, a) == kscm__strvalue(kscm, b));
		else
			return (0);
	}
	else if (kscm__isnumber(kscm, a)) {
		if (kscm__isnumber(kscm, b))
			return (kscm__ivalue(kscm, a) == kscm__ivalue(kscm, b));
		else
			return (0);
	}
	else
		return (a == b);
}

/* true or false value macro */
#define kscm__istrue(kscm,p)       ((p) != kscm->NIL && (p) != kscm->F)
#define kscm__isfalse(kscm,p)      ((p) == kscm->NIL || (p) == kscm->F)

/* Error macro */
#ifdef	KSCM_CONFIG_AVOID_HACK_LOOP
# define	KSCM__BEGIN	{
# define	KSCM__END	}
#else
/*
 * I believe this is better, but some compiler complains....
 */
# define	KSCM__BEGIN	do {
# define	KSCM__END	} while (0)
#endif

#define kscm__error_0(kscm,s) KSCM__BEGIN                       \
    kscm->args = kscm__cons(kscm, kscm__mk_string(kscm, (s)), kscm->NIL);          \
    kscm->_operator = (short)OP_ERR0;                 \
    return kscm->T; KSCM__END

#define kscm__error_1(kscm,s, a) KSCM__BEGIN                    \
    kscm->args = kscm__cons(kscm, (a), kscm->NIL);                     \
    kscm->args = kscm__cons(kscm, kscm__mk_string(kscm, (s)), kscm->args);         \
    kscm->_operator = (short)OP_ERR0;                 \
    return kscm->T; KSCM__END

 /* control macros for Eval_Cycle */
#define kscm__s_goto(kscm,a) KSCM__BEGIN                        \
    kscm->_operator = (short)(a);                     \
    return kscm->T; KSCM__END

#define kscm__s_save(kscm,a, b, c)  (                     \
    kscm->dump = kscm__cons(kscm, kscm->envir, kscm__cons(kscm, (c), kscm->dump)),       \
    kscm->dump = kscm__cons(kscm, (b), kscm->dump),                    \
    kscm->dump = kscm__cons(kscm, kscm__mk_number(kscm, (long)(a)), kscm->dump))   \


#define kscm__s_return(kscm,a) KSCM__BEGIN                      \
    kscm->value = (a);                               \
    kscm->_operator = kscm__ivalue(kscm, kscm__car(kscm, kscm->dump));              \
    kscm->args = kscm__cadr(kscm, kscm->dump);                         \
    kscm->envir = kscm__caddr(kscm, kscm->dump);                       \
    kscm->code = kscm__cadddr(kscm, kscm->dump);                       \
    kscm->dump = kscm__cddddr(kscm, kscm->dump);                       \
    return kscm->T; KSCM__END

#define kscm__s_retbool(kscm,tf)	kscm__s_return(kscm, (tf) ? kscm->T : kscm->F)



/* ========== Evaluation Cycle ========== */

/* operator code */
#define	OP_LOAD			0
#define	OP_T0LVL		1
#define	OP_T1LVL		2
#define	OP_READ			3
#define	OP_VALUEPRINT		4
#define	OP_EVAL			5
#define	OP_E0ARGS		6
#define	OP_E1ARGS		7
#define	OP_APPLY		8
#define	OP_DOMACRO		9

#define	OP_LAMBDA		10
#define	OP_QUOTE		11
#define	OP_DEF0			12
#define	OP_DEF1			13
#define	OP_BEGIN		14
#define	OP_IF0			15
#define	OP_IF1			16
#define	OP_SET0			17
#define	OP_SET1			18
#define	OP_LET0			19
#define	OP_LET1			20
#define	OP_LET2			21
#define	OP_LET0AST		22
#define	OP_LET1AST		23
#define	OP_LET2AST		24
#define	OP_LET0REC		25
#define	OP_LET1REC		26
#define	OP_LET2REC		27
#define	OP_COND0		28
#define	OP_COND1		29
#define	OP_DELAY		30
#define	OP_AND0			31
#define	OP_AND1			32
#define	OP_OR0			33
#define	OP_OR1			34
#define	OP_C0STREAM		35
#define	OP_C1STREAM		36
#define	OP_0MACRO		37
#define	OP_1MACRO		38
#define	OP_CASE0		39
#define	OP_CASE1		40
#define	OP_CASE2		41

#define	OP_PEVAL		42
#define	OP_PAPPLY		43
#define	OP_CONTINUATION		44
#define	OP_ADD			45
#define	OP_SUB			46
#define	OP_MUL			47
#define	OP_DIV			48
#define	OP_REM			49
#define	OP_CAR			50
#define	OP_CDR			51
#define	OP_CONS			52
#define	OP_SETCAR		53
#define	OP_SETCDR		54
#define	OP_NOT			55
#define	OP_BOOL			56
#define	OP_NULL			57
#define	OP_ZEROP		58
#define	OP_POSP			59
#define	OP_NEGP			60
#define	OP_NEQ			61
#define	OP_LESS			62
#define	OP_GRE			63
#define	OP_LEQ			64
#define	OP_GEQ			65
#define	OP_SYMBOL		66
#define	OP_NUMBER		67
#define	OP_STRING		68
#define	OP_PROC			69
#define	OP_PAIR			70
#define	OP_EQ			71
#define	OP_EQV			72
#define	OP_FORCE		73
#define	OP_WRITE		74
#define	OP_DISPLAY		75
#define	OP_NEWLINE		76
#define	OP_ERR0			77
#define	OP_ERR1			78
#define	OP_REVERSE		79
#define	OP_APPEND		80
#define	OP_PUT			81
#define	OP_GET			82
#define	OP_QUIT			83
#define	OP_GC			84
#define	OP_GCVERB		85
#define	OP_NEWSEGMENT		86

#define	OP_RDSEXPR		87
#define	OP_RDLIST		88
#define	OP_RDDOT		89
#define	OP_RDQUOTE		90
#define	OP_RDQQUOTE		91
#define	OP_RDUNQUOTE		92
#define	OP_RDUQTSP		93

#define	OP_P0LIST		94
#define	OP_P1LIST		95

#define	OP_LIST_LENGTH		96
#define	OP_ASSQ			97
#define	OP_PRINT_WIDTH		98
#define	OP_P0_WIDTH		99
#define	OP_P1_WIDTH		100
#define	OP_GET_CLOSURE		101
#define	OP_CLOSUREP		102
#define	OP_MACROP		103

kscm_object_t kscm__opexe_0(kscm_t* kscm, register short op)
{
	register kscm_object_t x;
	register kscm_object_t y = NULL;

	switch (op) {
	case OP_LOAD:		/* load */
		if (!kscm__isstring(kscm, kscm__car(kscm, kscm->args))) {
			kscm__error_0(kscm, "load -- argument is not string");
		}
		if ((kscm->infp = fopen(kscm__strvalue(kscm, kscm__car(kscm, kscm->args)), "r")) == NULL) {
			kscm->infp = stdin;
			kscm__error_1(kscm, "Unable to open", kscm__car(kscm, kscm->args));
		}
		if (!kscm->quiet)
			fprintf(kscm->outfp, "loading %s", kscm__strvalue(kscm, kscm__car(kscm, kscm->args)));
		kscm__s_goto(kscm, OP_T0LVL);

	case OP_T0LVL:	/* top level */
		if (!kscm->quiet)
			fprintf(kscm->outfp, "\n");
		kscm->dump = kscm->NIL;
		kscm->envir = kscm->global_env;
		kscm__s_save(kscm, OP_VALUEPRINT, kscm->NIL, kscm->NIL);
		kscm__s_save(kscm, OP_T1LVL, kscm->NIL, kscm->NIL);
		if (kscm->infp == stdin && !kscm->quiet)
			printf(KSCM_CONFIG_PROMPT);
		kscm__s_goto(kscm, OP_READ);

	case OP_T1LVL:	/* top level */
		kscm->code = kscm->value;
		kscm__s_goto(kscm, OP_EVAL);

	case OP_READ:		/* read */
		kscm->tok = kscm__token(kscm);
		kscm__s_goto(kscm, OP_RDSEXPR);

	case OP_VALUEPRINT:	/* print evalution result */
		kscm->print_flag = 1;
		kscm->args = kscm->value;
		if (kscm->quiet) {
			kscm__s_goto(kscm, OP_T0LVL);
		}
		else {
			kscm__s_save(kscm, OP_T0LVL, kscm->NIL, kscm->NIL);
			kscm__s_goto(kscm, OP_P0LIST);
		}

	case OP_EVAL:		/* main part of evalution */
		if (kscm__issymbol(kscm, kscm->code)) {	/* symbol */
			for (x = kscm->envir; x != kscm->NIL; x = kscm__cdr(kscm, x)) {
				for (y = kscm__car(kscm, x); y != kscm->NIL; y = kscm__cdr(kscm, y))
					if (kscm__caar(kscm, y) == kscm->code)
						break;
				if (y != kscm->NIL)
					break;
			}
			if (x != kscm->NIL) {
				kscm__s_return(kscm, kscm__cdar(kscm, y));
			}
			else {
				kscm__error_1(kscm, "Unbounded variable", kscm->code);
			}
		}
		else if (kscm__ispair(kscm, kscm->code)) {
			if (kscm__issyntax(kscm, x = kscm__car(kscm, kscm->code))) {	/* SYNTAX */
				kscm->code = kscm__cdr(kscm, kscm->code);
				kscm__s_goto(kscm, kscm__syntaxnum(kscm, x));
			}
			else {/* first, eval top element and eval arguments */
#ifdef KSCM_CONFIG_USE_MACRO
				kscm__s_save(kscm, OP_E0ARGS, kscm->NIL, kscm->code);
#else
				s_save(kscm, OP_E1ARGS, kscm->NIL, cdr(code));
#endif
				kscm->code = kscm__car(kscm, kscm->code);
				kscm__s_goto(kscm, OP_EVAL);
			}
		}
		else {
			kscm__s_return(kscm, kscm->code);
		}

#ifdef KSCM_CONFIG_USE_MACRO
	case OP_E0ARGS:	/* eval arguments */
		if (kscm__ismacro(kscm, kscm->value)) {	/* macro expansion */
			kscm__s_save(kscm, OP_DOMACRO, kscm->NIL, kscm->NIL);
			kscm->args = kscm__cons(kscm, kscm->code, kscm->NIL);
			kscm->code = kscm->value;
			kscm__s_goto(kscm, OP_APPLY);
		}
		else {
			kscm->code = kscm__cdr(kscm, kscm->code);
			kscm__s_goto(kscm, OP_E1ARGS);
		}
#endif

	case OP_E1ARGS:	/* eval arguments */
		kscm->args = kscm__cons(kscm, kscm->value, kscm->args);
		if (kscm__ispair(kscm, kscm->code)) {	/* continue */
			kscm__s_save(kscm, OP_E1ARGS, kscm->args, kscm__cdr(kscm, kscm->code));
			kscm->code = kscm__car(kscm, kscm->code);
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, OP_EVAL);
		}
		else {	/* end */
			kscm->args = kscm__reverse(kscm, kscm->args);
			kscm->code = kscm__car(kscm, kscm->args);
			kscm->args = kscm__cdr(kscm, kscm->args);
			kscm__s_goto(kscm, OP_APPLY);
		}

	case OP_APPLY:		/* apply 'code' to 'args' */
		if (kscm__isproc(kscm, kscm->code)) {
			kscm__s_goto(kscm, kscm__procnum(kscm, kscm->code));	/* PROCEDURE */
		}
		else if (kscm__isclosure(kscm, kscm->code)) {	/* CLOSURE */
		 /* make environment */
			kscm->envir = kscm__cons(kscm, kscm->NIL, kscm__closure_env(kscm, kscm->code));
			for (x = kscm__car(kscm, kscm__closure_code(kscm, kscm->code)), y = kscm->args;
				kscm__ispair(kscm, x); x = kscm__cdr(kscm, x), y = kscm__cdr(kscm, y)) {
				if (y == kscm->NIL) {
					kscm__error_0(kscm, "Few arguments");
				}
				else {
					kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, kscm__car(kscm, x), kscm__car(kscm, y)), kscm__car(kscm, kscm->envir));
				}
			}
			if (x == kscm->NIL) {
				/*--
				 * if (y != kscm->NIL) {
				 * 	Error_0("Many arguments");
				 * }
				 */
			}
			else if (kscm__issymbol(kscm, x))
				kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, x, y), kscm__car(kscm, kscm->envir));
			else {
				kscm__error_0(kscm, "Syntax error in closure");
			}
			kscm->code = kscm__cdr(kscm, kscm__closure_code(kscm, kscm->code));
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, OP_BEGIN);
		}
		else if (kscm__iscontinuation(kscm, kscm->code)) {	/* CONTINUATION */
			kscm->dump = kscm__cont_dump(kscm, kscm->code);
			kscm__s_return(kscm, kscm->args != kscm->NIL ? kscm__car(kscm, kscm->args) : kscm->NIL);
		}
		else {
			kscm__error_0(kscm, "Illegal function");
		}

#ifdef KSCM_CONFIG_USE_MACRO
	case OP_DOMACRO:	/* do macro */
		kscm->code = kscm->value;
		kscm__s_goto(kscm, OP_EVAL);
#endif

	case OP_LAMBDA:	/* lambda */
		kscm__s_return(kscm, kscm__mk_closure(kscm, kscm->code, kscm->envir));

	case OP_QUOTE:		/* quote */
		kscm__s_return(kscm, kscm__car(kscm, kscm->code));

	case OP_DEF0:	/* define */
		if (kscm__ispair(kscm, kscm__car(kscm, kscm->code))) {
			x = kscm__caar(kscm, kscm->code);
			kscm->code = kscm__cons(kscm, kscm->LAMBDA, kscm__cons(kscm, kscm__cdar(kscm, kscm->code), kscm__cdr(kscm, kscm->code)));
		}
		else {
			x = kscm__car(kscm, kscm->code);
			kscm->code = kscm__cadr(kscm, kscm->code);
		}
		if (!kscm__issymbol(kscm, x)) {
			kscm__error_0(kscm, "Variable is not symbol");
		}
		kscm__s_save(kscm, OP_DEF1, kscm->NIL, x);
		kscm__s_goto(kscm, OP_EVAL);

	case OP_DEF1:	/* define */
		for (x = kscm__car(kscm, kscm->envir); x != kscm->NIL; x = kscm__cdr(kscm, x))
			if (kscm__caar(kscm, x) == kscm->code)
				break;
		if (x != kscm->NIL)
			kscm__cdar(kscm, x) = kscm->value;
		else
			kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, kscm->code, kscm->value), kscm__car(kscm, kscm->envir));
		kscm__s_return(kscm, kscm->code);

	case OP_SET0:		/* set! */
		kscm__s_save(kscm, OP_SET1, kscm->NIL, kscm__car(kscm, kscm->code));
		kscm->code = kscm__cadr(kscm, kscm->code);
		kscm__s_goto(kscm, OP_EVAL);

	case OP_SET1:		/* set! */
		for (x = kscm->envir; x != kscm->NIL; x = kscm__cdr(kscm, x)) {
			for (y = kscm__car(kscm, x); y != kscm->NIL; y = kscm__cdr(kscm, y))
				if (kscm__caar(kscm, y) == kscm->code)
					break;
			if (y != kscm->NIL)
				break;
		}
		if (x != kscm->NIL) {
			kscm__cdar(kscm, y) = kscm->value;
			kscm__s_return(kscm, kscm->value);
		}
		else {
			kscm__error_1(kscm, "Unbounded variable", kscm->code);
		}

	case OP_BEGIN:		/* begin */
		if (!kscm__ispair(kscm, kscm->code)) {
			kscm__s_return(kscm, kscm->code);
		}
		if (kscm__cdr(kscm, kscm->code) != kscm->NIL) {
			kscm__s_save(kscm, OP_BEGIN, kscm->NIL, kscm__cdr(kscm, kscm->code));
		}
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, OP_EVAL);

	case OP_IF0:		/* if */
		kscm__s_save(kscm, OP_IF1, kscm->NIL, kscm__cdr(kscm, kscm->code));
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, OP_EVAL);

	case OP_IF1:		/* if */
		if (kscm__istrue(kscm, kscm->value))
			kscm->code = kscm__car(kscm, kscm->code);
		else
			kscm->code = kscm__cadr(kscm, kscm->code);	/* (if #f 1) ==> () because
						 * car(kscm->NIL) = kscm->NIL */
		kscm__s_goto(kscm, OP_EVAL);

	case OP_LET0:		/* let */
		kscm->args = kscm->NIL;
		kscm->value = kscm->code;
		kscm->code = kscm__issymbol(kscm, kscm__car(kscm, kscm->code)) ? kscm__cadr(kscm, kscm->code) : kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, OP_LET1);

	case OP_LET1:		/* let (caluculate parameters) */
		kscm->args = kscm__cons(kscm, kscm->value, kscm->args);
		if (kscm__ispair(kscm, kscm->code)) {	/* continue */
			kscm__s_save(kscm, OP_LET1, kscm->args, kscm__cdr(kscm, kscm->code));
			kscm->code = kscm__cadar(kscm, kscm->code);
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, OP_EVAL);
		}
		else {	/* end */
			kscm->args = kscm__reverse(kscm, kscm->args);
			kscm->code = kscm__car(kscm, kscm->args);
			kscm->args = kscm__cdr(kscm, kscm->args);
			kscm__s_goto(kscm, OP_LET2);
		}

	case OP_LET2:		/* let */
		kscm->envir = kscm__cons(kscm, kscm->NIL, kscm->envir);
		for (x = kscm__issymbol(kscm, kscm__car(kscm, kscm->code)) ? kscm__cadr(kscm, kscm->code) : kscm__car(kscm, kscm->code), y = kscm->args;
			y != kscm->NIL; x = kscm__cdr(kscm, x), y = kscm__cdr(kscm, y))
			kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, kscm__caar(kscm, x), kscm__car(kscm, y)), kscm__car(kscm, kscm->envir));
		if (kscm__issymbol(kscm, kscm__car(kscm, kscm->code))) {	/* named let */
			for (x = kscm__cadr(kscm, kscm->code), kscm->args = kscm->NIL; x != kscm->NIL; x = kscm__cdr(kscm, x))
				kscm->args = kscm__cons(kscm, kscm__caar(kscm, x), kscm->args);
			x = kscm__mk_closure(kscm, kscm__cons(kscm, kscm__reverse(kscm, kscm->args), kscm__cddr(kscm, kscm->code)), kscm->envir);
			kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, kscm__car(kscm, kscm->code), x), kscm__car(kscm, kscm->envir));
			kscm->code = kscm__cddr(kscm, kscm->code);
			kscm->args = kscm->NIL;
		}
		else {
			kscm->code = kscm__cdr(kscm, kscm->code);
			kscm->args = kscm->NIL;
		}
		kscm__s_goto(kscm, OP_BEGIN);

	case OP_LET0AST:	/* let* */
		if (kscm__car(kscm, kscm->code) == kscm->NIL) {
			kscm->envir = kscm__cons(kscm, kscm->NIL, kscm->envir);
			kscm->code = kscm__cdr(kscm, kscm->code);
			kscm__s_goto(kscm, OP_BEGIN);
		}
		kscm__s_save(kscm, OP_LET1AST, kscm__cdr(kscm, kscm->code), kscm__car(kscm, kscm->code));
		kscm->code = kscm__cadaar(kscm, kscm->code);
		kscm__s_goto(kscm, OP_EVAL);

	case OP_LET1AST:	/* let* (make new frame) */
		kscm->envir = kscm__cons(kscm, kscm->NIL, kscm->envir);
		kscm__s_goto(kscm, OP_LET2AST);

	case OP_LET2AST:	/* let* (caluculate parameters) */
		kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, kscm__caar(kscm, kscm->code), kscm->value), kscm__car(kscm, kscm->envir));
		kscm->code = kscm__cdr(kscm, kscm->code);
		if (kscm__ispair(kscm, kscm->code)) {	/* continue */
			kscm__s_save(kscm, OP_LET2AST, kscm->args, kscm->code);
			kscm->code = kscm__cadar(kscm, kscm->code);
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, OP_EVAL);
		}
		else {	/* end */
			kscm->code = kscm->args;
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, OP_BEGIN);
		}
	default:
		sprintf(kscm->strbuff, "%d is illegal operator", kscm->_operator);
		kscm__error_0(kscm, kscm->strbuff);
	}
	return kscm->T;
}


kscm_object_t kscm__opexe_1(kscm_t* kscm, register short op)
{
	register kscm_object_t x, y;

	switch (op) {
	case OP_LET0REC:	/* letrec */
		kscm->envir = kscm__cons(kscm, kscm->NIL, kscm->envir);
		kscm->args = kscm->NIL;
		kscm->value = kscm->code;
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, OP_LET1REC);

	case OP_LET1REC:	/* letrec (caluculate parameters) */
		kscm->args = kscm__cons(kscm, kscm->value, kscm->args);
		if (kscm__ispair(kscm, kscm->code)) {	/* continue */
			kscm__s_save(kscm, OP_LET1REC, kscm->args, kscm__cdr(kscm, kscm->code));
			kscm->code = kscm__cadar(kscm, kscm->code);
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, OP_EVAL);
		}
		else {	/* end */
			kscm->args = kscm__reverse(kscm, kscm->args);
			kscm->code = kscm__car(kscm, kscm->args);
			kscm->args = kscm__cdr(kscm, kscm->args);
			kscm__s_goto(kscm, OP_LET2REC);
		}

	case OP_LET2REC:	/* letrec */
		for (x = kscm__car(kscm, kscm->code), y = kscm->args; y != kscm->NIL; x = kscm__cdr(kscm, x), y = kscm__cdr(kscm, y))
			kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, kscm__caar(kscm, x), kscm__car(kscm, y)), kscm__car(kscm, kscm->envir));
		kscm->code = kscm__cdr(kscm, kscm->code);
		kscm->args = kscm->NIL;
		kscm__s_goto(kscm, OP_BEGIN);

	case OP_COND0:		/* cond */
		if (!kscm__ispair(kscm, kscm->code)) {
			kscm__error_0(kscm, "Syntax error in cond");
		}
		kscm__s_save(kscm, OP_COND1, kscm->NIL, kscm->code);
		kscm->code = kscm__caar(kscm, kscm->code);
		kscm__s_goto(kscm, OP_EVAL);

	case OP_COND1:		/* cond */
		if (kscm__istrue(kscm, kscm->value)) {
			if ((kscm->code = kscm__cdar(kscm, kscm->code)) == kscm->NIL) {
				kscm__s_return(kscm, kscm->value);
			}
			kscm__s_goto(kscm, OP_BEGIN);
		}
		else {
			if ((kscm->code = kscm__cdr(kscm, kscm->code)) == kscm->NIL) {
				kscm__s_return(kscm, kscm->NIL);
			}
			else {
				kscm__s_save(kscm, OP_COND1, kscm->NIL, kscm->code);
				kscm->code = kscm__caar(kscm, kscm->code);
				kscm__s_goto(kscm, OP_EVAL);
			}
		}

	case OP_DELAY:		/* delay */
		x = kscm__mk_closure(kscm, kscm__cons(kscm, kscm->NIL, kscm->code), kscm->envir);
		kscm__setpromise(kscm, x);
		kscm__s_return(kscm, x);

	case OP_AND0:		/* and */
		if (kscm->code == kscm->NIL) {
			kscm__s_return(kscm, kscm->T);
		}
		kscm__s_save(kscm, OP_AND1, kscm->NIL, kscm__cdr(kscm, kscm->code));
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, OP_EVAL);

	case OP_AND1:		/* and */
		if (kscm__isfalse(kscm, kscm->value)) {
			kscm__s_return(kscm, kscm->value);
		}
		else if (kscm->code == kscm->NIL) {
			kscm__s_return(kscm, kscm->value);
		}
		else {
			kscm__s_save(kscm, OP_AND1, kscm->NIL, kscm__cdr(kscm, kscm->code));
			kscm->code = kscm__car(kscm, kscm->code);
			kscm__s_goto(kscm, OP_EVAL);
		}

	case OP_OR0:		/* or */
		if (kscm->code == kscm->NIL) {
			kscm__s_return(kscm, kscm->F);
		}
		kscm__s_save(kscm, OP_OR1, kscm->NIL, kscm__cdr(kscm, kscm->code));
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, OP_EVAL);

	case OP_OR1:		/* or */
		if (kscm__istrue(kscm, kscm->value)) {
			kscm__s_return(kscm, kscm->value);
		}
		else if (kscm->code == kscm->NIL) {
			kscm__s_return(kscm, kscm->value);
		}
		else {
			kscm__s_save(kscm, OP_OR1, kscm->NIL, kscm__cdr(kscm, kscm->code));
			kscm->code = kscm__car(kscm, kscm->code);
			kscm__s_goto(kscm, OP_EVAL);
		}

	case OP_C0STREAM:	/* cons-stream */
		kscm__s_save(kscm, OP_C1STREAM, kscm->NIL, kscm__cdr(kscm, kscm->code));
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, OP_EVAL);

	case OP_C1STREAM:	/* cons-stream */
		kscm->args = kscm->value;	/* save value to register args for gc */
		x = kscm__mk_closure(kscm, kscm__cons(kscm, kscm->NIL, kscm->code), kscm->envir);
		kscm__setpromise(kscm, x);
		kscm__s_return(kscm, kscm__cons(kscm, kscm->args, x));

#ifdef KSCM_CONFIG_USE_MACRO
	case OP_0MACRO:	/* macro */
		x = kscm__car(kscm, kscm->code);
		kscm->code = kscm__cadr(kscm, kscm->code);
		if (!kscm__issymbol(kscm, x)) {
			kscm__error_0(kscm, "Variable is not symbol");
		}
		kscm__s_save(kscm, OP_1MACRO, kscm->NIL, x);
		kscm__s_goto(kscm, OP_EVAL);

	case OP_1MACRO:	/* macro */
		kscm__type(kscm, kscm->value) |= T_MACRO;
		for (x = kscm__car(kscm, kscm->envir); x != kscm->NIL; x = kscm__cdr(kscm, x))
			if (kscm__caar(kscm, x) == kscm->code)
				break;
		if (x != kscm->NIL)
			kscm__cdar(kscm, x) = kscm->value;
		else
			kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, kscm->code, kscm->value), kscm__car(kscm, kscm->envir));
		kscm__s_return(kscm, kscm->code);
#endif

	case OP_CASE0:		/* case */
		kscm__s_save(kscm, OP_CASE1, kscm->NIL, kscm__cdr(kscm, kscm->code));
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, OP_EVAL);

	case OP_CASE1:		/* case */
		for (x = kscm->code; x != kscm->NIL; x = kscm__cdr(kscm, x)) {
			if (!kscm__ispair(kscm, y = kscm__caar(kscm, x)))
				break;
			for (; y != kscm->NIL; y = kscm__cdr(kscm, y))
				if (kscm__eqv(kscm, kscm__car(kscm, y), kscm->value))
					break;
			if (y != kscm->NIL)
				break;
		}
		if (x != kscm->NIL) {
			if (kscm__ispair(kscm, kscm__caar(kscm, x))) {
				kscm->code = kscm__cdar(kscm, x);
				kscm__s_goto(kscm, OP_BEGIN);
			}
			else {/* else */
				kscm__s_save(kscm, OP_CASE2, kscm->NIL, kscm__cdar(kscm, x));
				kscm->code = kscm__caar(kscm, x);
				kscm__s_goto(kscm, OP_EVAL);
			}
		}
		else {
			kscm__s_return(kscm, kscm->NIL);
		}

	case OP_CASE2:		/* case */
		if (kscm__istrue(kscm, kscm->value)) {
			kscm__s_goto(kscm, OP_BEGIN);
		}
		else {
			kscm__s_return(kscm, kscm->NIL);
		}
	case OP_PAPPLY:	/* apply */
		kscm->code = kscm__car(kscm, kscm->args);
		kscm->args = kscm__cadr(kscm, kscm->args);
		kscm__s_goto(kscm, OP_APPLY);

	case OP_PEVAL:	/* eval */
		kscm->code = kscm__car(kscm, kscm->args);
		kscm->args = kscm->NIL;
		kscm__s_goto(kscm, OP_EVAL);

	case OP_CONTINUATION:	/* call-with-current-continuation */
		kscm->code = kscm__car(kscm, kscm->args);
		kscm->args = kscm__cons(kscm, kscm__mk_continuation(kscm, kscm->dump), kscm->NIL);
		kscm__s_goto(kscm, OP_APPLY);

	default:
		sprintf(kscm->strbuff, "%d is illegal operator", kscm->_operator);
		kscm__error_0(kscm, kscm->strbuff);
	}
	return kscm->T;
}


kscm_object_t kscm__opexe_2(kscm_t* kscm, register short op)
{
	register kscm_object_t x, y;
	register long v;

	switch (op) {
	case OP_ADD:		/* + */
		for (x = kscm->args, v = 0; x != kscm->NIL; x = kscm__cdr(kscm, x))
			v += kscm__ivalue(kscm, kscm__car(kscm, x));
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case OP_SUB:		/* - */
		for (x = kscm__cdr(kscm, kscm->args), v = kscm__ivalue(kscm, kscm__car(kscm, kscm->args)); x != kscm->NIL; x = kscm__cdr(kscm, x))
			v -= kscm__ivalue(kscm, kscm__car(kscm, x));
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case OP_MUL:		/* * */
		for (x = kscm->args, v = 1; x != kscm->NIL; x = kscm__cdr(kscm, x))
			v *= kscm__ivalue(kscm, kscm__car(kscm, x));
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case OP_DIV:		/* / */
		for (x = kscm__cdr(kscm, kscm->args), v = kscm__ivalue(kscm, kscm__car(kscm, kscm->args)); x != kscm->NIL; x = kscm__cdr(kscm, x)) {
			if (kscm__ivalue(kscm, kscm__car(kscm, x)) != 0)
				v /= kscm__ivalue(kscm, kscm__car(kscm, x));
			else {
				kscm__error_0(kscm, "Divided by zero");
			}
		}
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case OP_REM:		/* remainder */
		for (x = kscm__cdr(kscm, kscm->args), v = kscm__ivalue(kscm, kscm__car(kscm, kscm->args)); x != kscm->NIL; x = kscm__cdr(kscm, x)) {
			if (kscm__ivalue(kscm, kscm__car(kscm, x)) != 0)
				v %= kscm__ivalue(kscm, kscm__car(kscm, x));
			else {
				kscm__error_0(kscm, "Divided by zero");
			}
		}
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case OP_CAR:		/* car */
		if (kscm__ispair(kscm, kscm__car(kscm, kscm->args))) {
			kscm__s_return(kscm, kscm__caar(kscm, kscm->args));
		}
		else {
			kscm__error_0(kscm, "Unable to car for non-cons cell");
		}

	case OP_CDR:		/* cdr */
		if (kscm__ispair(kscm, kscm__car(kscm, kscm->args))) {
			kscm__s_return(kscm, kscm__cdar(kscm, kscm->args));
		}
		else {
			kscm__error_0(kscm, "Unable to cdr for non-cons cell");
		}

	case OP_CONS:		/* cons */
		kscm__cdr(kscm, kscm->args) = kscm__cadr(kscm, kscm->args);
		kscm__s_return(kscm, kscm->args);

	case OP_SETCAR:	/* set-car! */
		if (kscm__ispair(kscm, kscm__car(kscm, kscm->args))) {
			kscm__caar(kscm, kscm->args) = kscm__cadr(kscm, kscm->args);
			kscm__s_return(kscm, kscm__car(kscm, kscm->args));
		}
		else {
			kscm__error_0(kscm, "Unable to set-car! for non-cons cell");
		}

	case OP_SETCDR:	/* set-cdr! */
		if (kscm__ispair(kscm, kscm__car(kscm, kscm->args))) {
			kscm__cdar(kscm, kscm->args) = kscm__cadr(kscm, kscm->args);
			kscm__s_return(kscm, kscm__car(kscm, kscm->args));
		}
		else {
			kscm__error_0(kscm, "Unable to set-cdr! for non-cons cell");
		}

	default:
		sprintf(kscm->strbuff, "%d is illegal operator", kscm->_operator);
		kscm__error_0(kscm, kscm->strbuff);
	}
	return kscm->T;
}


kscm_object_t kscm__opexe_3(kscm_t* kscm, register short op)
{
	register kscm_object_t x, y;

	switch (op) {
	case OP_NOT:		/* not */
		kscm__s_retbool(kscm, kscm__isfalse(kscm, kscm__car(kscm, kscm->args)));
	case OP_BOOL:		/* boolean? */
		kscm__s_retbool(kscm, kscm__car(kscm, kscm->args) == kscm->F || kscm__car(kscm, kscm->args) == kscm->T);
	case OP_NULL:		/* null? */
		kscm__s_retbool(kscm, kscm__car(kscm, kscm->args) == kscm->NIL);
	case OP_ZEROP:		/* zero? */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) == 0);
	case OP_POSP:		/* positive? */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) > 0);
	case OP_NEGP:		/* negative? */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) < 0);
	case OP_NEQ:		/* = */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) == kscm__ivalue(kscm, kscm__cadr(kscm, kscm->args)));
	case OP_LESS:		/* < */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) < kscm__ivalue(kscm, kscm__cadr(kscm, kscm->args)));
	case OP_GRE:		/* > */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) > kscm__ivalue(kscm, kscm__cadr(kscm, kscm->args)));
	case OP_LEQ:		/* <= */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) <= kscm__ivalue(kscm, kscm__cadr(kscm, kscm->args)));
	case OP_GEQ:		/* >= */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) >= kscm__ivalue(kscm, kscm__cadr(kscm, kscm->args)));
	case OP_SYMBOL:	/* symbol? */
		kscm__s_retbool(kscm, kscm__issymbol(kscm, kscm__car(kscm, kscm->args)));
	case OP_NUMBER:	/* number? */
		kscm__s_retbool(kscm, kscm__isnumber(kscm, kscm__car(kscm, kscm->args)));
	case OP_STRING:	/* string? */
		kscm__s_retbool(kscm, kscm__isstring(kscm, kscm__car(kscm, kscm->args)));
	case OP_PROC:		/* procedure? */
		/*--
			 * continuation should be procedure by the example
			 * (call-with-current-continuation procedure?) ==> #t
				 * in R^3 report sec. 6.9
			 */
		kscm__s_retbool(kscm, kscm__isproc(kscm, kscm__car(kscm, kscm->args)) || kscm__isclosure(kscm, kscm__car(kscm, kscm->args))
			|| kscm__iscontinuation(kscm, kscm__car(kscm, kscm->args)));
	case OP_PAIR:		/* pair? */
		kscm__s_retbool(kscm, kscm__ispair(kscm, kscm__car(kscm, kscm->args)));
	case OP_EQ:		/* eq? */
		kscm__s_retbool(kscm, kscm__car(kscm, kscm->args) == kscm__cadr(kscm, kscm->args));
	case OP_EQV:		/* eqv? */
		kscm__s_retbool(kscm, kscm__eqv(kscm, kscm__car(kscm, kscm->args), kscm__cadr(kscm, kscm->args)));
	default:
		sprintf(kscm->strbuff, "%d is illegal operator", kscm->_operator);
		kscm__error_0(kscm, kscm->strbuff);
	}
	return kscm->T;
}


kscm_object_t kscm__opexe_4(kscm_t* kscm, register short op)
{
	register kscm_object_t x, y;

	switch (op) {
	case OP_FORCE:		/* force */
		kscm->code = kscm__car(kscm, kscm->args);
		if (kscm__ispromise(kscm, kscm->code)) {
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, OP_APPLY);
		}
		else {
			kscm__s_return(kscm, kscm->code);
		}

	case OP_WRITE:		/* write */
		kscm->print_flag = 1;
		kscm->args = kscm__car(kscm, kscm->args);
		kscm__s_goto(kscm, OP_P0LIST);

	case OP_DISPLAY:	/* display */
		kscm->print_flag = 0;
		kscm->args = kscm__car(kscm, kscm->args);
		kscm__s_goto(kscm, OP_P0LIST);

	case OP_NEWLINE:	/* newline */
		fprintf(kscm->outfp, "\n");
		kscm__s_return(kscm, kscm->T);

	case OP_ERR0:	/* error */
		if (!kscm__isstring(kscm, kscm__car(kscm, kscm->args))) {
			kscm__error_0(kscm, "error -- first argument must be string");
		}
		kscm->tmpfp = kscm->outfp;
		kscm->outfp = stderr;
		if (kscm->all_errors_fatal) {
			kscm__fatal_error(kscm, kscm__strvalue(kscm, kscm__car(kscm, kscm->args)), NULL, NULL, NULL);
		}
		fprintf(kscm->outfp, "Error: ");
		fprintf(kscm->outfp, "%s", kscm__strvalue(kscm, kscm__car(kscm, kscm->args)));
		kscm->args = kscm__cdr(kscm, kscm->args);
		kscm__s_goto(kscm, OP_ERR1);

	case OP_ERR1:	/* error */
		fprintf(kscm->outfp, " ");
		if (kscm->args != kscm->NIL) {
			kscm__s_save(kscm, OP_ERR1, kscm__cdr(kscm, kscm->args), kscm->NIL);
			kscm->args = kscm__car(kscm, kscm->args);
			kscm->print_flag = 1;
			kscm__s_goto(kscm, OP_P0LIST);
		}
		else {
			fprintf(kscm->outfp, "\n");
			kscm__flushinput(kscm);
			kscm->outfp = kscm->tmpfp;
			kscm__s_goto(kscm, OP_T0LVL);
		}

	case OP_REVERSE:	/* reverse */
		kscm__s_return(kscm, kscm__reverse(kscm, kscm__car(kscm, kscm->args)));

	case OP_APPEND:	/* append */
		kscm__s_return(kscm, kscm__append(kscm, kscm__car(kscm, kscm->args), kscm__cadr(kscm, kscm->args)));

	case OP_PUT:		/* put */
		if (!kscm__hasprop(kscm, kscm__car(kscm, kscm->args)) || !kscm__hasprop(kscm, kscm__cadr(kscm, kscm->args))) {
			kscm__error_0(kscm, "Illegal use of put");
		}
		for (x = kscm__symprop(kscm, kscm__car(kscm, kscm->args)), y = kscm__cadr(kscm, kscm->args); x != kscm->NIL; x = kscm__cdr(kscm, x))
			if (kscm__caar(kscm, x) == y)
				break;
		if (x != kscm->NIL)
			kscm__cdar(kscm, x) = kscm__caddr(kscm, kscm->args);
		else
			kscm__symprop(kscm, kscm__car(kscm, kscm->args)) = kscm__cons(kscm, kscm__cons(kscm, y, kscm__caddr(kscm, kscm->args)),
				kscm__symprop(kscm, kscm__car(kscm, kscm->args)));
		kscm__s_return(kscm, kscm->T);

	case OP_GET:		/* get */
		if (!kscm__hasprop(kscm, kscm__car(kscm, kscm->args)) || !kscm__hasprop(kscm, kscm__cadr(kscm, kscm->args))) {
			kscm__error_0(kscm, "Illegal use of get");
		}
		for (x = kscm__symprop(kscm, kscm__car(kscm, kscm->args)), y = kscm__cadr(kscm, kscm->args); x != kscm->NIL; x = kscm__cdr(kscm, x))
			if (kscm__caar(kscm, x) == y)
				break;
		if (x != kscm->NIL) {
			kscm__s_return(kscm, kscm__cdar(kscm, x));
		}
		else {
			kscm__s_return(kscm, kscm->NIL);
		}

	case OP_QUIT:		/* quit */
		return (kscm->NIL);

	case OP_GC:		/* gc */
		kscm__gc(kscm, kscm->NIL, kscm->NIL);
		kscm__s_return(kscm, kscm->T);

	case OP_GCVERB:		/* gc-verbose */
	{	int	was = kscm->gc_verbose;

	kscm->gc_verbose = (kscm__car(kscm, kscm->args) != kscm->F);
	kscm__s_retbool(kscm, was);
	}

	case OP_NEWSEGMENT:	/* new-segment */
		if (!kscm__isnumber(kscm, kscm__car(kscm, kscm->args))) {
			kscm__error_0(kscm, "new-segment -- argument must be number");
		}
		fprintf(kscm->outfp, "allocate %d new segments\n",
			kscm__alloc_cellseg(kscm, (int)kscm__ivalue(kscm, kscm__car(kscm, kscm->args))));
		kscm__s_return(kscm, kscm->T);
	}
}


kscm_object_t kscm__opexe_5(kscm_t* kscm, register short op)
{
	register kscm_object_t x, y;

	switch (op) {
		/* ========== reading part ========== */
	case OP_RDSEXPR:
		switch (kscm->tok) {
		case TOK_COMMENT:
			while (kscm__inchar(kscm) != '\n')
				;
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, OP_RDSEXPR);
		case TOK_LPAREN:
			kscm->tok = kscm__token(kscm);
			if (kscm->tok == TOK_RPAREN) {
				kscm__s_return(kscm, kscm->NIL);
			}
			else if (kscm->tok == TOK_DOT) {
				kscm__error_0(kscm, "syntax error -- illegal dot expression");
			}
			else {
				kscm__s_save(kscm, OP_RDLIST, kscm->NIL, kscm->NIL);
				kscm__s_goto(kscm, OP_RDSEXPR);
			}
		case TOK_QUOTE:
			kscm__s_save(kscm, OP_RDQUOTE, kscm->NIL, kscm->NIL);
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, OP_RDSEXPR);
#ifdef KSCM_CONFIG_USE_QQUOTE
		case TOK_BQUOTE:
			kscm__s_save(kscm, OP_RDQQUOTE, kscm->NIL, kscm->NIL);
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, OP_RDSEXPR);
		case TOK_COMMA:
			kscm__s_save(kscm, OP_RDUNQUOTE, kscm->NIL, kscm->NIL);
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, OP_RDSEXPR);
		case TOK_ATMARK:
			kscm__s_save(kscm, OP_RDUQTSP, kscm->NIL, kscm->NIL);
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, OP_RDSEXPR);
#endif
		case TOK_ATOM:
			kscm__s_return(kscm, kscm__mk_atom(kscm, kscm__readstr(kscm, "();\t\n ")));
		case TOK_DQUOTE:
			kscm__s_return(kscm, kscm__mk_string(kscm, kscm__readstrexp(kscm)));
		case TOK_SHARP:
			if ((x = kscm__mk_const(kscm, kscm__readstr(kscm, "();\t\n "))) == kscm->NIL) {
				kscm__error_0(kscm, "Undefined sharp expression");
			}
			else {
				kscm__s_return(kscm, x);
			}
		default:
			kscm__error_0(kscm, "syntax error -- illegal token");
		}
		break;

	case OP_RDLIST:
		kscm->args = kscm__cons(kscm, kscm->value, kscm->args);
		kscm->tok = kscm__token(kscm);
		if (kscm->tok == TOK_COMMENT) {
			while (kscm__inchar(kscm) != '\n')
				;
			kscm->tok = kscm__token(kscm);
		}
		if (kscm->tok == TOK_RPAREN) {
			kscm__s_return(kscm, kscm__non_alloc_rev(kscm, kscm->NIL, kscm->args));
		}
		else if (kscm->tok == TOK_DOT) {
			kscm__s_save(kscm, OP_RDDOT, kscm->args, kscm->NIL);
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, OP_RDSEXPR);
		}
		else {
			kscm__s_save(kscm, OP_RDLIST, kscm->args, kscm->NIL);;
			kscm__s_goto(kscm, OP_RDSEXPR);
		}

	case OP_RDDOT:
		if (kscm__token(kscm) != TOK_RPAREN) {
			kscm__error_0(kscm, "syntax error -- illegal dot expression");
		}
		else {
			kscm__s_return(kscm, kscm__non_alloc_rev(kscm, kscm->value, kscm->args));
		}

	case OP_RDQUOTE:
		kscm__s_return(kscm, kscm__cons(kscm, kscm->QUOTE, kscm__cons(kscm, kscm->value, kscm->NIL)));

#ifdef KSCM_CONFIG_USE_QQUOTE
	case OP_RDQQUOTE:
		kscm__s_return(kscm, kscm__cons(kscm, kscm->QQUOTE, kscm__cons(kscm, kscm->value, kscm->NIL)));

	case OP_RDUNQUOTE:
		kscm__s_return(kscm, kscm__cons(kscm, kscm->UNQUOTE, kscm__cons(kscm, kscm->value, kscm->NIL)));

	case OP_RDUQTSP:
		kscm__s_return(kscm, kscm__cons(kscm, kscm->UNQUOTESP, kscm__cons(kscm, kscm->value, kscm->NIL)));
#endif

		/* ========== printing part ========== */
	case OP_P0LIST:
		if (!kscm__ispair(kscm, kscm->args)) {
			kscm__printatom(kscm, kscm->args, kscm->print_flag);
			kscm__s_return(kscm, kscm->T);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->QUOTE && kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			fprintf(kscm->outfp, "'");
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0LIST);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->QQUOTE && kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			fprintf(kscm->outfp, "`");
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0LIST);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->UNQUOTE && kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			fprintf(kscm->outfp, ",");
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0LIST);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->UNQUOTESP && kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			fprintf(kscm->outfp, ",@");
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0LIST);
		}
		else {
			fprintf(kscm->outfp, "(");
			kscm__s_save(kscm, OP_P1LIST, kscm__cdr(kscm, kscm->args), kscm->NIL);
			kscm->args = kscm__car(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0LIST);
		}

	case OP_P1LIST:
		if (kscm__ispair(kscm, kscm->args)) {
			kscm__s_save(kscm, OP_P1LIST, kscm__cdr(kscm, kscm->args), kscm->NIL);
			fprintf(kscm->outfp, " ");
			kscm->args = kscm__car(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0LIST);
		}
		else {
			if (kscm->args != kscm->NIL) {
				fprintf(kscm->outfp, " . ");
				kscm__printatom(kscm, kscm->args, kscm->print_flag);
			}
			fprintf(kscm->outfp, ")");
			kscm__s_return(kscm, kscm->T);
		}

	default:
		sprintf(kscm->strbuff, "%d is illegal operator", kscm->_operator);
		kscm__error_0(kscm, kscm->strbuff);

	}
	return kscm->T;
}


kscm_object_t kscm__opexe_6(kscm_t* kscm, register short op)
{
	register kscm_object_t x, y;
	register long v;
	static long	w;
	char	buffer[32];

	switch (op) {
	case OP_LIST_LENGTH:	/* list-length */	/* a.k */
		for (x = kscm__car(kscm, kscm->args), v = 0; kscm__ispair(kscm, x); x = kscm__cdr(kscm, x))
			++v;
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case OP_ASSQ:		/* assq */	/* a.k */
		x = kscm__car(kscm, kscm->args);
		for (y = kscm__cadr(kscm, kscm->args); kscm__ispair(kscm, y); y = kscm__cdr(kscm, y)) {
			if (!kscm__ispair(kscm, kscm__car(kscm, y))) {
				kscm__error_0(kscm, "Unable to handle non pair element");
			}
			if (x == kscm__caar(kscm, y))
				break;
		}
		if (kscm__ispair(kscm, y)) {
			kscm__s_return(kscm, kscm__car(kscm, y));
		}
		else {
			kscm__s_return(kscm, kscm->F);
		}

	case OP_PRINT_WIDTH:	/* print-width */	/* a.k */
		w = 0;
		kscm->args = kscm__car(kscm, kscm->args);
		kscm->print_flag = -1;
		kscm__s_goto(kscm, OP_P0_WIDTH);

	case OP_P0_WIDTH:
		if (!kscm__ispair(kscm, kscm->args)) {
			w += kscm__printatom(kscm, kscm->args, kscm->print_flag);
			kscm__s_return(kscm, kscm__mk_number(kscm, w));
		}
		else if (kscm__car(kscm, kscm->args) == kscm->QUOTE
			&& kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			++w;
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0_WIDTH);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->QQUOTE
			&& kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			++w;
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0_WIDTH);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->UNQUOTE
			&& kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			++w;
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0_WIDTH);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->UNQUOTESP
			&& kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			w += 2;
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0_WIDTH);
		}
		else {
			++w;
			kscm__s_save(kscm, OP_P1_WIDTH, kscm__cdr(kscm, kscm->args), kscm->NIL);
			kscm->args = kscm__car(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0_WIDTH);
		}

	case OP_P1_WIDTH:
		if (kscm__ispair(kscm, kscm->args)) {
			kscm__s_save(kscm, OP_P1_WIDTH, kscm__cdr(kscm, kscm->args), kscm->NIL);
			++w;
			kscm->args = kscm__car(kscm, kscm->args);
			kscm__s_goto(kscm, OP_P0_WIDTH);
		}
		else {
			if (kscm->args != kscm->NIL)
				w += 3 + kscm__printatom(kscm, kscm->args, kscm->print_flag);
			++w;
			kscm__s_return(kscm, kscm__mk_number(kscm, w));
		}

	case OP_GET_CLOSURE:	/* get-closure-code */	/* a.k */
		kscm->args = kscm__car(kscm, kscm->args);
		if (kscm->args == kscm->NIL) {
			kscm__s_return(kscm, kscm->F);
		}
		else if (kscm__isclosure(kscm, kscm->args)) {
			kscm__s_return(kscm, kscm__cons(kscm, kscm->LAMBDA, kscm__closure_code(kscm, kscm->value)));
#ifdef KSCM_CONFIG_USE_MACRO
		}
		else if (kscm__ismacro(kscm, kscm->args)) {
			kscm__s_return(kscm, kscm__cons(kscm, kscm->LAMBDA, kscm__closure_code(kscm, kscm->value)));
#endif
		}
		else {
			kscm__s_return(kscm, kscm->F);
		}
	case OP_CLOSUREP:		/* closure? */
		/*
		 * Note, macro object is also a closure.
		 * Therefore, (closure? <#MACRO>) ==> #t
		 */
		if (kscm__car(kscm, kscm->args) == kscm->NIL) {
			kscm__s_return(kscm, kscm->F);
		}
		kscm__s_retbool(kscm, kscm__isclosure(kscm, kscm__car(kscm, kscm->args)));
#ifdef KSCM_CONFIG_USE_MACRO
	case OP_MACROP:		/* macro? */
		if (kscm__car(kscm, kscm->args) == kscm->NIL) {
			kscm__s_return(kscm, kscm->F);
		}
		kscm__s_retbool(kscm, kscm__ismacro(kscm, kscm__car(kscm, kscm->args)));
#endif
	default:
		sprintf(kscm->strbuff, "%d is illegal operator", kscm->_operator);
		kscm__error_0(kscm, kscm->strbuff);
	}
	return kscm->T;	/* NOTREACHED */
}


kscm_object_t(*kscm__shared_dispatch_table[])(kscm_t* kscm, register short op) = {
	&kscm__opexe_0,	/* OP_LOAD = 0, */
	&kscm__opexe_0,	/* OP_T0LVL, */
	&kscm__opexe_0,	/* OP_T1LVL, */
	&kscm__opexe_0,	/* OP_READ, */
	&kscm__opexe_0,	/* OP_VALUEPRINT, */
	&kscm__opexe_0,	/* OP_EVAL, */
	&kscm__opexe_0,	/* OP_E0ARGS, */
	&kscm__opexe_0,	/* OP_E1ARGS, */
	&kscm__opexe_0,	/* OP_APPLY, */
	&kscm__opexe_0,	/* OP_DOMACRO, */

	&kscm__opexe_0,	/* OP_LAMBDA, */
	&kscm__opexe_0,	/* OP_QUOTE, */
	&kscm__opexe_0,	/* OP_DEF0, */
	&kscm__opexe_0,	/* OP_DEF1, */
	&kscm__opexe_0,	/* OP_BEGIN, */
	&kscm__opexe_0,	/* OP_IF0, */
	&kscm__opexe_0,	/* OP_IF1, */
	&kscm__opexe_0,	/* OP_SET0, */
	&kscm__opexe_0,	/* OP_SET1, */
	&kscm__opexe_0,	/* OP_LET0, */
	&kscm__opexe_0,	/* OP_LET1, */
	&kscm__opexe_0,	/* OP_LET2, */
	&kscm__opexe_0,	/* OP_LET0AST, */
	&kscm__opexe_0,	/* OP_LET1AST, */
	&kscm__opexe_0,	/* OP_LET2AST, */

	&kscm__opexe_1,	/* OP_LET0REC, */
	&kscm__opexe_1,	/* OP_LET1REC, */
	&kscm__opexe_1,	/* OP_LETREC2, */
	&kscm__opexe_1,	/* OP_COND0, */
	&kscm__opexe_1,	/* OP_COND1, */
	&kscm__opexe_1,	/* OP_DELAY, */
	&kscm__opexe_1,	/* OP_AND0, */
	&kscm__opexe_1,	/* OP_AND1, */
	&kscm__opexe_1,	/* OP_OR0, */
	&kscm__opexe_1,	/* OP_OR1, */
	&kscm__opexe_1,	/* OP_C0STREAM, */
	&kscm__opexe_1,	/* OP_C1STREAM, */
	&kscm__opexe_1,	/* OP_0MACRO, */
	&kscm__opexe_1,	/* OP_1MACRO, */
	&kscm__opexe_1,	/* OP_CASE0, */
	&kscm__opexe_1,	/* OP_CASE1, */
	&kscm__opexe_1,	/* OP_CASE2, */

	&kscm__opexe_1,	/* OP_PEVAL, */
	&kscm__opexe_1,	/* OP_PAPPLY, */
	&kscm__opexe_1,	/* OP_CONTINUATION, */

	&kscm__opexe_2,	/* OP_ADD, */
	&kscm__opexe_2,	/* OP_SUB, */
	&kscm__opexe_2,	/* OP_MUL, */
	&kscm__opexe_2,	/* OP_DIV, */
	&kscm__opexe_2,	/* OP_REM, */
	&kscm__opexe_2,	/* OP_CAR, */
	&kscm__opexe_2,	/* OP_CDR, */
	&kscm__opexe_2,	/* OP_CONS, */
	&kscm__opexe_2,	/* OP_SETCAR, */
	&kscm__opexe_2,	/* OP_SETCDR, */

	&kscm__opexe_3,	/* OP_NOT, */
	&kscm__opexe_3,	/* OP_BOOL, */
	&kscm__opexe_3,	/* OP_NULL, */
	&kscm__opexe_3,	/* OP_ZEROP, */
	&kscm__opexe_3,	/* OP_POSP, */
	&kscm__opexe_3,	/* OP_NEGP, */
	&kscm__opexe_3,	/* OP_NEQ, */
	&kscm__opexe_3,	/* OP_LESS, */
	&kscm__opexe_3,	/* OP_GRE, */
	&kscm__opexe_3,	/* OP_LEQ, */
	&kscm__opexe_3,	/* OP_GEQ, */
	&kscm__opexe_3,	/* OP_SYMBOL, */
	&kscm__opexe_3,	/* OP_NUMBER, */
	&kscm__opexe_3,	/* OP_STRING, */
	&kscm__opexe_3,	/* OP_PROC, */
	&kscm__opexe_3,	/* OP_PAIR, */
	&kscm__opexe_3,	/* OP_EQ, */
	&kscm__opexe_3,	/* OP_EQV, */

	&kscm__opexe_4,	/* OP_FORCE, */
	&kscm__opexe_4,	/* OP_WRITE, */
	&kscm__opexe_4,	/* OP_DISPLAY, */
	&kscm__opexe_4,	/* OP_NEWLINE, */
	&kscm__opexe_4,	/* OP_ERR0, */
	&kscm__opexe_4,	/* OP_ERR1, */
	&kscm__opexe_4,	/* OP_REVERSE, */
	&kscm__opexe_4,	/* OP_APPEND, */
	&kscm__opexe_4,	/* OP_PUT, */
	&kscm__opexe_4,	/* OP_GET, */
	&kscm__opexe_4,	/* OP_QUIT, */
	&kscm__opexe_4,	/* OP_GC, */
	&kscm__opexe_4,	/* OP_GCVERB, */
	&kscm__opexe_4,	/* OP_NEWSEGMENT, */

	&kscm__opexe_5,	/* OP_RDSEXPR, */
	&kscm__opexe_5,	/* OP_RDLIST, */
	&kscm__opexe_5,	/* OP_RDDOT, */
	&kscm__opexe_5,	/* OP_RDQUOTE, */
	&kscm__opexe_5,	/* OP_RDQQUOTE, */
	&kscm__opexe_5,	/* OP_RDUNQUOTE, */
	&kscm__opexe_5,	/* OP_RDUQTSP, */
	&kscm__opexe_5,	/* OP_P0LIST, */
	&kscm__opexe_5,	/* OP_P1LIST, */

	&kscm__opexe_6,	/* OP_LIST_LENGTH, */
	&kscm__opexe_6,	/* OP_ASSQ, */
	&kscm__opexe_6,	/* OP_PRINT_WIDTH, */
	&kscm__opexe_6,	/* OP_P0_WIDTH, */
	&kscm__opexe_6,	/* OP_P1_WIDTH, */
	&kscm__opexe_6,	/* OP_GET_CLOSURE, */
	&kscm__opexe_6,	/* OP_CLOSUREP, */
#ifdef KSCM_CONFIG_USE_MACRO
	&kscm__opexe_6,	/* OP_MACROP, */
#endif

};


/* kernel of this intepreter */
kscm_object_t kscm__eval_cycle(kscm_t* kscm, register short op)
{

	kscm->_operator = op;
	for (;;)
		if ((*kscm__shared_dispatch_table[kscm->_operator])(kscm, kscm->_operator) == kscm->NIL)
			return kscm->NIL;
}

/* ========== Initialization of internal keywords ========== */

void kscm__mk_syntax(kscm_t* kscm, unsigned short op, const char *name)
{
	kscm_object_t x;

	x = kscm__cons(kscm, kscm__mk_string(kscm, name), kscm->NIL);
	kscm__type(kscm, x) = (T_SYNTAX | T_SYMBOL);
	kscm__syntaxnum(kscm, x) = op;
	kscm->oblist = kscm__cons(kscm, x, kscm->oblist);
}

void kscm__mk_proc(kscm_t* kscm, unsigned short op, const char *name)
{
	kscm_object_t x, y;

	x = kscm__mk_symbol(kscm, name);
	y = kscm__get_cell(kscm, kscm->NIL, kscm->NIL);
	kscm__type(kscm, y) = (T_PROC | T_ATOM);
	kscm__ivalue(kscm, y) = (long)op;
	kscm__car(kscm, kscm->global_env) = kscm__cons(kscm, kscm__cons(kscm, x, y), kscm__car(kscm, kscm->global_env));
}


void kscm__init_vars_global(kscm_t* kscm)
{
	kscm_object_t x;

	/* init input/output file */
	kscm->infp = stdin;
	kscm->outfp = stdout;
	/* init kscm->NIL */
	kscm__type(kscm, kscm->NIL) = (T_ATOM | MARK);
	kscm__car(kscm, kscm->NIL) = kscm__cdr(kscm, kscm->NIL) = kscm->NIL;
	/* init T */
	kscm__type(kscm, kscm->T) = (T_ATOM | MARK);
	kscm__car(kscm, kscm->T) = kscm__cdr(kscm, kscm->T) = kscm->T;
	/* init F */
	kscm__type(kscm, kscm->F) = (T_ATOM | MARK);
	kscm__car(kscm, kscm->F) = kscm__cdr(kscm, kscm->F) = kscm->F;
	/* init global_env */
	kscm->global_env = kscm__cons(kscm, kscm->NIL, kscm->NIL);
	/* init else */
	x = kscm__mk_symbol(kscm, "else");
	kscm__car(kscm, kscm->global_env) = kscm__cons(kscm, kscm__cons(kscm, x, kscm->T), kscm__car(kscm, kscm->global_env));
}


void kscm__init_syntax(kscm_t* kscm)
{
	/* init syntax */
	kscm__mk_syntax(kscm, OP_LAMBDA, "lambda");
	kscm__mk_syntax(kscm, OP_QUOTE, "quote");
	kscm__mk_syntax(kscm, OP_DEF0, "define");
	kscm__mk_syntax(kscm, OP_IF0, "if");
	kscm__mk_syntax(kscm, OP_BEGIN, "begin");
	kscm__mk_syntax(kscm, OP_SET0, "set!");
	kscm__mk_syntax(kscm, OP_LET0, "let");
	kscm__mk_syntax(kscm, OP_LET0AST, "let*");
	kscm__mk_syntax(kscm, OP_LET0REC, "letrec");
	kscm__mk_syntax(kscm, OP_COND0, "cond");
	kscm__mk_syntax(kscm, OP_DELAY, "delay");
	kscm__mk_syntax(kscm, OP_AND0, "and");
	kscm__mk_syntax(kscm, OP_OR0, "or");
	kscm__mk_syntax(kscm, OP_C0STREAM, "cons-stream");
#ifdef KSCM_CONFIG_USE_MACRO
	kscm__mk_syntax(kscm, OP_0MACRO, "macro");
#endif
	kscm__mk_syntax(kscm, OP_CASE0, "case");
}


void kscm__init_procs(kscm_t* kscm)
{
	/* init procedure */
	kscm__mk_proc(kscm, OP_PEVAL, "eval");
	kscm__mk_proc(kscm, OP_PAPPLY, "apply");
	kscm__mk_proc(kscm, OP_CONTINUATION, "call-with-current-continuation");
	kscm__mk_proc(kscm, OP_FORCE, "force");
	kscm__mk_proc(kscm, OP_CAR, "car");
	kscm__mk_proc(kscm, OP_CDR, "cdr");
	kscm__mk_proc(kscm, OP_CONS, "cons");
	kscm__mk_proc(kscm, OP_SETCAR, "set-car!");
	kscm__mk_proc(kscm, OP_SETCDR, "set-cdr!");
	kscm__mk_proc(kscm, OP_ADD, "+");
	kscm__mk_proc(kscm, OP_SUB, "-");
	kscm__mk_proc(kscm, OP_MUL, "*");
	kscm__mk_proc(kscm, OP_DIV, "/");
	kscm__mk_proc(kscm, OP_REM, "remainder");
	kscm__mk_proc(kscm, OP_NOT, "not");
	kscm__mk_proc(kscm, OP_BOOL, "boolean?");
	kscm__mk_proc(kscm, OP_SYMBOL, "symbol?");
	kscm__mk_proc(kscm, OP_NUMBER, "number?");
	kscm__mk_proc(kscm, OP_STRING, "string?");
	kscm__mk_proc(kscm, OP_PROC, "procedure?");
	kscm__mk_proc(kscm, OP_PAIR, "pair?");
	kscm__mk_proc(kscm, OP_EQV, "eqv?");
	kscm__mk_proc(kscm, OP_EQ, "eq?");
	kscm__mk_proc(kscm, OP_NULL, "null?");
	kscm__mk_proc(kscm, OP_ZEROP, "zero?");
	kscm__mk_proc(kscm, OP_POSP, "positive?");
	kscm__mk_proc(kscm, OP_NEGP, "negative?");
	kscm__mk_proc(kscm, OP_NEQ, "=");
	kscm__mk_proc(kscm, OP_LESS, "<");
	kscm__mk_proc(kscm, OP_GRE, ">");
	kscm__mk_proc(kscm, OP_LEQ, "<=");
	kscm__mk_proc(kscm, OP_GEQ, ">=");
	kscm__mk_proc(kscm, OP_READ, "read");
	kscm__mk_proc(kscm, OP_WRITE, "write");
	kscm__mk_proc(kscm, OP_DISPLAY, "display");
	kscm__mk_proc(kscm, OP_NEWLINE, "newline");
	kscm__mk_proc(kscm, OP_LOAD, "load");
	kscm__mk_proc(kscm, OP_ERR0, "error");
	kscm__mk_proc(kscm, OP_REVERSE, "reverse");
	kscm__mk_proc(kscm, OP_APPEND, "append");
	kscm__mk_proc(kscm, OP_PUT, "put");
	kscm__mk_proc(kscm, OP_GET, "get");
	kscm__mk_proc(kscm, OP_GC, "gc");
	kscm__mk_proc(kscm, OP_GCVERB, "gc-verbose");
	kscm__mk_proc(kscm, OP_NEWSEGMENT, "new-segment");
	kscm__mk_proc(kscm, OP_LIST_LENGTH, "list-length");	/* a.k */
	kscm__mk_proc(kscm, OP_ASSQ, "assq");	/* a.k */
	kscm__mk_proc(kscm, OP_PRINT_WIDTH, "print-width");	/* a.k */
	kscm__mk_proc(kscm, OP_GET_CLOSURE, "get-closure-code");	/* a.k */
	kscm__mk_proc(kscm, OP_CLOSUREP, "closure?");	/* a.k */
#ifdef KSCM_CONFIG_USE_MACRO
	kscm__mk_proc(kscm, OP_MACROP, "macro?");	/* a.k */
#endif
	kscm__mk_proc(kscm, OP_QUIT, "quit");
}


/* initialize several globals */
void kscm__init_globals(kscm_t* kscm)
{
	kscm__init_vars_global(kscm);
	kscm__init_syntax(kscm);
	kscm__init_procs(kscm);
	/* intialization of global pointers to special symbols */
	kscm->LAMBDA = kscm__mk_symbol(kscm, "lambda");
	kscm->QUOTE = kscm__mk_symbol(kscm, "quote");
#ifdef KSCM_CONFIG_USE_QQUOTE
	kscm->QQUOTE = kscm__mk_symbol(kscm, "quasiquote");
	kscm->UNQUOTE = kscm__mk_symbol(kscm, "unquote");
	kscm->UNQUOTESP = kscm__mk_symbol(kscm, "unquote-splicing");
#endif

}

/* ========== Error ==========  */

void kscm__fatal_error(kscm_t* kscm, const char *fmt, const char *a, const char *b, const char *c)
{
	fprintf(stderr, "Fatal error: ");
	fprintf(stderr, fmt, a, b, c);
	fprintf(stderr, "\n");
	exit(1);
}

#ifdef KSCM_CONFIG_USE_SETJMP
void kscm__error(kscm_t* kscm, const char *fmt, const char *a, const char *b, const char *c)
{
	fprintf(stderr, "Error: ");
	fprintf(stderr, fmt, a, b, c);
	fprintf(stderr, "\n");
	kscm__flushinput(kscm);
	longjmp(kscm->error_jmp, OP_T0LVL);
}

#endif

/* ========== Main ========== */

#ifdef CMDLINE
int main(int argc, char **argv)
#else
int main()
#endif
{
	short   i;
	short   op = (short)OP_LOAD;

#ifdef CMDLINE
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-e") == 0) {
			all_errors_fatal = 1;
		}
		else if (strcmp(argv[i], "-q") == 0) {
			quiet = 1;
		}
	}
#endif

	kscm_t* kscm = (kscm_t*) calloc(1, sizeof(kscm_t));

	if (!kscm->quiet)
		printf(KSCM_CONFIG_BANNER);
	kscm__init_scheme(kscm);
	kscm->args = kscm__cons(kscm, kscm__mk_string(kscm, KSCM_CONFIG_INITFILE), kscm->NIL);
#ifdef KSCM_CONFIG_USE_SETJMP
	op = setjmp(kscm->error_jmp);
#endif
	kscm__eval_cycle(kscm, op);
	exit(0);
}

#ifdef KSCM_PLUSPLUS
}
#endif