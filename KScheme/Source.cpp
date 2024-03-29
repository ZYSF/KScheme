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
// If KSCM_CONFIG_USE_PRECISE is defined, overflow detection will occur in basic math functions
#define KSCM_CONFIG_USE_PRECISE
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
//  #define VERBOSE 	/* define this if you want verbose GC */
#define	KSCM_CONFIG_AVOID_HACK_LOOP	/* define this if your compiler is poor
			 * enougth to complain "do { } while (0)"
			 * construction.
			 */
#define KSCM_CONFIG_USE_SETJMP	/* undef this if you do not want to use setjmp() */
#define KSCM_CONFIG_USE_QQUOTE	/* undef this if you do not need quasiquote */
#define KSCM_CONFIG_USE_MACRO	/* undef this if you do not need macro */
#define KSCM_CONFIG_USE_PERSIST /* undef this if you do not need persistence */
#define KSCM_CONFIG_USE_PRECISE	/* undef this if you do not need overflow detection and precise integer size */
#define KSCM_CONFIG_USE_STRUCTS	/* undef this if you do not need additional structure types (buffers & abstractions) */
#define KSCM_CONFIG_USE_FLOATS  /* undef this if you do not need floating-point functionality (i.e. undefine this if you're running in kernel mode) */
#define KSCM_CONFIG_USE_OBJECTS	/* undef this if you do not need object-oriented/vector features (these are handy but may complicate simple implementations) */
#define KSCM_CONFIG_USE_UTF8	/* undef this if you do not need Unicode support */

#define KSCM_CONFIG_MAXLOADS 20 /* the maximum depth of the load stack */

#ifdef KSCM_CONFIG_USE_PRECISE
#include <stdint.h>
#endif

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
#define KSCM_CONFIG_CELL_SEGSIZE    10000	/* # of cells in one segment */
#define KSCM_CONFIG_CELL_NSEGMENT   10000	/* # of segments for cells */
#define KSCM_CONFIG_STR_SEGSIZE     10000	/* bytes of one string segment */
#define KSCM_CONFIG_STR_NSEGMENT    10000	/* # of segments for strings */
#endif

#define KSCM_CONFIG_BANNER "Hello, This is KScheme (kscm) 0.2, based on Mini-Scheme Interpreter Version 0.85p1.\n"

#define KSCM_CONFIG_PERSIST_MAGIC	"KSCM"
#define KSCM_CONFIG_PERSIST_VERSION	2

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


/* ========== Evaluation Cycle ========== */

/* operator code */
#define	KSCM_OP_LOAD			0
#define	KSCM_OP_T0LVL			1
#define	KSCM_OP_T1LVL			2
#define	KSCM_OP_READ			3
#define	KSCM_OP_VALUEPRINT		4
#define	KSCM_OP_EVAL			5
#define	KSCM_OP_E0ARGS			6
#define	KSCM_OP_E1ARGS			7
#define	KSCM_OP_APPLY			8
#define	KSCM_OP_DOMACRO			9

#define	KSCM_OP_LAMBDA			10
#define	KSCM_OP_QUOTE			11
#define	KSCM_OP_DEF0			12
#define	KSCM_OP_DEF1			13
#define	KSCM_OP_BEGIN			14
#define	KSCM_OP_IF0				15
#define	KSCM_OP_IF1				16
#define	KSCM_OP_SET0			17
#define	KSCM_OP_SET1			18
#define	KSCM_OP_LET0			19
#define	KSCM_OP_LET1			20
#define	KSCM_OP_LET2			21
#define	KSCM_OP_LET0AST			22
#define	KSCM_OP_LET1AST			23
#define	KSCM_OP_LET2AST			24
#define	KSCM_OP_LET0REC			25
#define	KSCM_OP_LET1REC			26
#define	KSCM_OP_LET2REC			27
#define	KSCM_OP_COND0			28
#define	KSCM_OP_COND1			29
#define	KSCM_OP_DELAY			30
#define	KSCM_OP_AND0			31
#define	KSCM_OP_AND1			32
#define	KSCM_OP_OR0				33
#define	KSCM_OP_OR1				34
#define	KSCM_OP_C0STREAM		35
#define	KSCM_OP_C1STREAM		36
#define	KSCM_OP_0MACRO			37
#define	KSCM_OP_1MACRO			38
#define	KSCM_OP_CASE0			39
#define	KSCM_OP_CASE1			40
#define	KSCM_OP_CASE2			41

#define	KSCM_OP_PEVAL			42
#define	KSCM_OP_PAPPLY			43
#define	KSCM_OP_CONTINUATION	44
#define	KSCM_OP_ADD				45
#define	KSCM_OP_SUB				46
#define	KSCM_OP_MUL				47
#define	KSCM_OP_DIV				48
#define	KSCM_OP_REM				49
#define	KSCM_OP_CAR				50
#define	KSCM_OP_CDR				51
#define	KSCM_OP_CONS			52
#define	KSCM_OP_SETCAR			53
#define	KSCM_OP_SETCDR			54
#define	KSCM_OP_NOT				55
#define	KSCM_OP_BOOL			56
#define	KSCM_OP_NULL			57
#define	KSCM_OP_ZEROP			58
#define	KSCM_OP_POSP			59
#define	KSCM_OP_NEGP			60
#define	KSCM_OP_NEQ				61
#define	KSCM_OP_LESS			62
#define	KSCM_OP_GRE				63
#define	KSCM_OP_LEQ				64
#define	KSCM_OP_GEQ				65
#define	KSCM_OP_SYMBOL			66
#define	KSCM_OP_NUMBER			67
#define	KSCM_OP_STRING			68
#define	KSCM_OP_PROC			69
#define	KSCM_OP_PAIR			70
#define	KSCM_OP_EQ				71
#define	KSCM_OP_EQV				72
#define	KSCM_OP_FORCE			73
#define	KSCM_OP_WRITE			74
#define	KSCM_OP_DISPLAY			75
#define	KSCM_OP_NEWLINE			76
#define	KSCM_OP_ERR0			77
#define	KSCM_OP_ERR1			78
#define	KSCM_OP_REVERSE			79
#define	KSCM_OP_APPEND			80
#define	KSCM_OP_PUT				81
#define	KSCM_OP_GET				82
#define	KSCM_OP_QUIT			83
#define	KSCM_OP_GC				84
#define	KSCM_OP_GCVERB			85
#define	KSCM_OP_NEWSEGMENT		86

#define	KSCM_OP_RDSEXPR			87
#define	KSCM_OP_RDLIST			88
#define	KSCM_OP_RDDOT			89
#define	KSCM_OP_RDQUOTE			90
#define	KSCM_OP_RDQQUOTE		91
#define	KSCM_OP_RDUNQUOTE		92
#define	KSCM_OP_RDUQTSP			93

#define	KSCM_OP_P0LIST			94
#define	KSCM_OP_P1LIST			95

#define	KSCM_OP_LIST_LENGTH		96
#define	KSCM_OP_ASSQ			97
#define	KSCM_OP_PRINT_WIDTH		98
#define	KSCM_OP_P0_WIDTH		99
#define	KSCM_OP_P1_WIDTH		100
#define	KSCM_OP_GET_CLOSURE		101
#define	KSCM_OP_CLOSUREP		102
#define	KSCM_OP_MACROP			103

#define KSCM_OP_STRCAT			104
#define KSCM_OP_STRLEN			105
#define KSCM_OP_STRGET			106
#define KSCM_OP_SAVE_STATE		107
#define KSCM_OP_RESUME_STATE	108
#define KSCM_OP_BUFFER			109
#define KSCM_OP_BUFFER_NEW		110
#define KSCM_OP_BUFFER_LEN		111
#define KSCM_OP_BUFFER_GET		112
#define KSCM_OP_BUFFER_SET		113
#define KSCM_OP_ABSTRACTION			114
#define KSCM_OP_ABSTRACTION_NEW		115
#define KSCM_OP_ABSTRACTION_TYPE		116
#define KSCM_OP_ABSTRACTION_VALUE		117
#define KSCM_OP_OBJECT			118
#define KSCM_OP_OBJECT_NEW		119
#define KSCM_OP_OBJECT_LEN		120
#define KSCM_OP_OBJECT_GET		121
#define KSCM_OP_OBJECT_SET		122
#define KSCM_OP_OBJECT_RETYPE	123
#define KSCM_OP_SYMBOL_TO_STRING	124
#define KSCM_OP_BUFFER_LOAD		125
#define KSCM_OP_BUFFER_SAVE		126

#define KSCM_TOK_LPAREN  0
#define KSCM_TOK_RPAREN  1
#define KSCM_TOK_DOT     2
#define KSCM_TOK_ATOM    3
#define KSCM_TOK_QUOTE   4
#define KSCM_TOK_COMMENT 5
#define KSCM_TOK_DQUOTE  6
#ifdef KSCM_CONFIG_USE_QQUOTE
# define KSCM_TOK_BQUOTE  7
# define KSCM_TOK_COMMA   8
# define KSCM_TOK_ATMARK  9
#endif
#define KSCM_TOK_SHARP   10

typedef struct kscm kscm_t;

/* cell structure */
struct kscm_cell {
	unsigned long _flag; // TODO: This should probably always be 32-bit
	union {
		struct {
			char* _svalue;
			short   _keynum;
		} _string;
		struct {
			long    _ivalue; // TODO: This should probably always be 32-bit, i.e. representing a flexible "small int" value
		} _number;
		struct {
			struct kscm_cell* _car;
			struct kscm_cell* _cdr;
		} _cons;
#ifdef KSCM_CONFIG_USE_STRUCTS
		struct {
			unsigned long _length;
			unsigned char* _data;
		} _buffer;
#endif
#ifdef KSCM_CONFIG_USE_FLOATS
		struct {
			double _dvalue;
		} _float64;
#endif
#ifdef KSCM_CONFIG_USE_OBJECTS
		struct {
			struct kscm_cell* _type;
			long _count;
			long _gccount;
			struct kscm_cell** _elements;
		} _objx;
#endif
	} _object;
};

typedef struct kscm_cell* kscm_object_t;

#define KSCM_PERSIST_TINT32			1
#define KSCM_PERSIST_TSTRING		2
#define KSCM_PERSIST_TSYMBOL		3
#define KSCM_PERSIST_TPAIR			4
#define KSCM_PERSIST_TPROC			5
#define KSCM_PERSIST_TCLOSURE		6
#define KSCM_PERSIST_TSYNTAX		7
#define KSCM_PERSIST_TCONTINUATION	8
#define KSCM_PERSIST_TBUFFER		9
#define KSCM_PERSIST_TABSTRACTION	10
#define KSCM_PERSIST_TOBJX			11
#define KSCM_PERSIST_TFLOAT32_RESERVED	12
#define KSCM_PERSIST_TFLOAT64		13
#define KSCM_PERSIST_TUINT64_RESERVED	14
#define KSCM_PERSIST_TINT64_RESERVED	15

#define KSCM_T_STRING         1	/* 0000000000000001 */
#define KSCM_T_NUMBER         2	/* 0000000000000010 */
#define KSCM_T_SYMBOL         4	/* 0000000000000100 */
#define KSCM_T_SYNTAX         8	/* 0000000000001000 */
#define KSCM_T_PROC          16	/* 0000000000010000 */
#define KSCM_T_PAIR          32	/* 0000000000100000 */
#define KSCM_T_CLOSURE       64	/* 0000000001000000 */
#define KSCM_T_CONTINUATION 128	/* 0000000010000000 */
#ifdef KSCM_CONFIG_USE_MACRO
# define KSCM_T_MACRO        256/* 0000000100000000 */
#endif
#define KSCM_T_PROMISE      512	/* 0000001000000000 */
#define KSCM_T_BUFFER		1024/* 0000010000000000 */
#define KSCM_T_ABSTRACTION	2048/* 0000100000000000 */
#define KSCM_T_OBJX			4096/* 0001000000000000 */
#define KSCM_T_FLOAT64		8192/* 0010000000000000 */
#define KSCM_T_ATOM       16384	/* 0100000000000000 */	/* only for gc */
#define KSCM_CLRATOM      49151	/* 1011111111111111 */	/* only for gc */
#define KSCM_MARK         32768	/* 1000000000000000 */
#define KSCM_UNMARK       32767	/* 0111111111111111 */

/* macros for cell operations */
#define kscm__type(kscm,p)         ((p)->_flag)

#define kscm__isstring(kscm,p)     (kscm__type(kscm, p)&KSCM_T_STRING)
#define kscm__strvalue(kscm,p)     ((p)->_object._string._svalue)
#define kscm__keynum(kscm,p)       ((p)->_object._string._keynum)

#define kscm__isnumber(kscm,p)     (kscm__type(kscm, p)&KSCM_T_NUMBER)
#define kscm__ivalue(kscm,p)       ((p)->_object._number._ivalue)

#ifdef KSCM_CONFIG_USE_FLOATS
#define kscm__isfloat64(kscm,p)     (kscm__type(kscm, p)&KSCM_T_FLOAT64)
#define kscm__dvalue(kscm,p)       ((p)->_object._float64._dvalue)
#endif

#define kscm__ispair(kscm,p)       (kscm__type(kscm,p)&KSCM_T_PAIR)
#define kscm__car(kscm,p)          ((p)->_object._cons._car)
#define kscm__cdr(kscm,p)          ((p)->_object._cons._cdr)

#define kscm__issymbol(kscm,p)     (kscm__type(kscm, p)&KSCM_T_SYMBOL)
#define kscm__symname(kscm,p)      kscm__strvalue(kscm, kscm__car(kscm, p))
#define kscm__hasprop(kscm,p)      (kscm__type(kscm, p)&KSCM_T_SYMBOL)
#define kscm__symprop(kscm,p)      kscm__cdr(kscm, p)

#define kscm__issyntax(kscm,p)     (kscm__type(kscm, p)&KSCM_T_SYNTAX)
#define kscm__isproc(kscm,p)       (kscm__type(kscm, p)&KSCM_T_PROC)
#define kscm__syntaxname(kscm,p)   kscm__strvalue(kscm, kscm__car(kscm, p))
#define kscm__syntaxnum(kscm,p)    kscm__keynum(kscm, kscm__car(kscm, p))
#define kscm__procnum(kscm,p)      kscm__ivalue(kscm, p)

#define kscm__isclosure(kscm,p)    (kscm__type(kscm, p)&KSCM_T_CLOSURE)
#ifdef KSCM_CONFIG_USE_MACRO
# define kscm__ismacro(kscm,p)      (kscm__type(kscm, p)&KSCM_T_MACRO)
#endif
#define kscm__closure_code(kscm,p) kscm__car(kscm, p)
#define kscm__closure_env(kscm,p)  kscm__cdr(kscm, p)

#define kscm__iscontinuation(kscm,p) (kscm__type(kscm, p)&KSCM_T_CONTINUATION)
#define kscm__cont_dump(kscm,p)    kscm__cdr(kscm, p)

#define kscm__ispromise(kscm,p)    (kscm__type(kscm,p)&KSCM_T_PROMISE)
#define kscm__setpromise(kscm,p)   kscm__type(kscm,p) |= KSCM_T_PROMISE

#ifdef KSCM_CONFIG_USE_STRUCTS
#define kscm__isbuffer(kscm,p)		(kscm__type(kscm,p)&KSCM_T_BUFFER)
#define kscm__isabstraction(kscm,p)	(kscm__type(kscm,p)&KSCM_T_ABSTRACTION)
#endif

#ifdef KSCM_CONFIG_USE_OBJECTS
#define kscm__isobjx(kscm,p)		(kscm__type(kscm,p)&KSCM_T_OBJX)
#endif

#define kscm__isatom(kscm,p)       (kscm__type(kscm,p)&KSCM_T_ATOM)
#define kscm__setatom(kscm,p)      kscm__type(kscm,p) |= KSCM_T_ATOM
#define kscm__clratom(kscm,p)      kscm__type(kscm,p) &= KSCM_CLRATOM

#define kscm__ismark(kscm,p)       (kscm__type(kscm, p)&KSCM_MARK)
#define kscm__setmark(kscm,p)      kscm__type(kscm, p) |= KSCM_MARK
#define kscm__clrmark(kscm,p)      kscm__type(kscm, p) &= KSCM_UNMARK

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
	int     last_cell_seg;// = -1;
	//char* str_seg[KSCM_CONFIG_STR_NSEGMENT];
	//int     str_seglast;// = -1;

	/* We use 4 registers (actually, some more registers are used internally). */
	kscm_object_t args;			/* register for arguments of function */
	kscm_object_t envir;			/* stack register for current environment */
	kscm_object_t code;			/* register for current code */
	kscm_object_t dump;			/* stack register for next evaluation */

	/* The VM is currently single-threaded, but to facilitate compatibility with future/other versions
	 * some information is associated with the "main thread" as though it were one thread in a multi-threaded
	 * environment.
	 */
	int _threadstate;
	kscm_object_t _threadname;
	kscm_object_t _threadopts;
	kscm_object_t _threadobject;

	/* Right now, the state format just contains the number of bytes per reference (either 4 or 8). */
	int _stateformat;

	struct kscm_cell _NIL;
	kscm_object_t NIL;// = &_NIL;		/* special cell representing empty cell */
	struct kscm_cell _T;
	kscm_object_t T;// = &_T;		/* special cell representing #t */
	struct kscm_cell _F;
	kscm_object_t F;// = &_F;		/* special cell representing #f */
	kscm_object_t oblist;// = &_NIL;		/* pointer to symbol table */
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

	//FILE* infp;			/* input file */
	FILE* inputs[KSCM_CONFIG_MAXLOADS];
	int inputtop; // = 0; (TODO: Remove the other initialisers - assume calloc or similar clears memory prior during manual initialisation)
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
/*int kscm__alloc_strseg(kscm_t* kscm, int n)
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
}*/

void kscm__fatal_error(kscm_t* kscm, const char* msg, const char* a, const char* b, const char* c);
void kscm__error(kscm_t* kscm, const char* msg, const char* a, const char* b, const char* c);
void kscm__init_globals(kscm_t* kscm);
kscm_object_t kscm__mk_string(kscm_t* kscm, const char* str);

/* initialization of Mini-Scheme */
void kscm__init_scheme(kscm_t* kscm)
{
	register kscm_object_t i;

	kscm->last_cell_seg = -1;
	//kscm->str_seglast = -1;
	kscm->NIL = &kscm->_NIL;
	kscm->T = &kscm->_T;
	kscm->F = &kscm->_F;
	kscm->oblist = &kscm->_NIL;
	kscm->free_cell = &kscm->_NIL;

	if (kscm__alloc_cellseg(kscm, KSCM_CONFIG_FIRST_CELLSEGS) != KSCM_CONFIG_FIRST_CELLSEGS)
		kscm__fatal_error(kscm, "Unable to allocate initial cell segments", NULL, NULL, NULL);
	/*if (!kscm__alloc_strseg(kscm, 1))
		kscm__fatal_error(kscm, "Unable to allocate initial string segments", NULL, NULL, NULL);*/
#ifdef VERBOSE
	kscm->gc_verbose = 1;
#else
	kscm->gc_verbose = 0;
#endif
	kscm->_stateformat = 4;
	kscm->_threadstate = 1;
	kscm->_threadobject = kscm->NIL;
	kscm->_threadname = kscm__mk_string(kscm, "main");
	kscm->_threadopts = kscm__mk_string(kscm, "");
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

	kscm__type(kscm, x) = KSCM_T_PAIR;
	kscm__car(kscm, x) = a;
	kscm__cdr(kscm, x) = b;
	return (x);
}

/* get number atom */
kscm_object_t kscm__mk_number(kscm_t* kscm, register long num)
{
	register kscm_object_t x = kscm__get_cell(kscm, kscm->NIL, kscm->NIL);

	kscm__type(kscm, x) = (KSCM_T_NUMBER | KSCM_T_ATOM);
	kscm__ivalue(kscm, x) = num;
	return (x);
}

#ifdef KSCM_CONFIG_USE_FLOATS
kscm_object_t kscm__mk_float64(kscm_t* kscm, double value) {
	kscm_object_t x = kscm__get_cell(kscm, kscm->NIL, kscm->NIL);

	kscm__type(kscm, x) = (KSCM_T_FLOAT64 | KSCM_T_ATOM);
	x->_object._float64._dvalue = value;

	return x;
}
#endif

/* allocate name to string area */
//char* kscm__store_string(kscm_t* kscm, const char *name)
//{
//	register char* q = NULL;
//	register short i;
//	long    len, remain;
//
//	/* first check name has already listed */
//	for (i = 0; i <= kscm->str_seglast; i++)
//		for (q = kscm->str_seg[i]; *q != (char)(-1); ) {
//			if (!strcmp(q, name))
//				goto FOUND;
//			while (*q++)
//				;	/* get next string */
//		}
//	len = strlen(name) + 2;
//	// TODO: Replace legacy types, it's starting to get ugly. -Zak
//	remain = (long long)KSCM_CONFIG_STR_SEGSIZE - ((long long)q - (long long)kscm->str_seg[kscm->str_seglast]);
//	if (remain < len) {
//		if (!kscm__alloc_strseg(kscm, 1))
//			kscm__fatal_error(kscm, "run out of string area", NULL, NULL, NULL);
//		q = kscm->str_seg[kscm->str_seglast];
//		/*if ((long long)KSCM_CONFIG_STR_SEGSIZE - ((long long)q - (long long)kscm->str_seg[kscm->str_seglast])) {
//			fprintf(stderr, "String in question's total length is %d", len);
//			kscm__fatal_error(kscm, "string too big", NULL, NULL, NULL);
//		}*/
//	}
//	strcpy(q, name);
//FOUND:
//	return (q);
//}

/* get new string */
kscm_object_t kscm__mk_string(kscm_t* kscm, const char *str)
{
	register kscm_object_t x = kscm__get_cell(kscm, kscm->NIL, kscm->NIL);

	kscm__strvalue(kscm, x) = _strdup(str);//kscm__store_string(kscm, str);
	kscm__type(kscm, x) = (KSCM_T_STRING | KSCM_T_ATOM);
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
		kscm__type(kscm, x) = KSCM_T_SYMBOL;
		kscm->oblist = kscm__cons(kscm, x, kscm->oblist);
		return (x);
	}
}

#ifdef KSCM_CONFIG_USE_STRUCTS
kscm_object_t kscm__mk_buffer(kscm_t* kscm, long len) {
	void* d = calloc(1, len);
	if (d == NULL) {
		return kscm->NIL;
	}
	kscm_object_t result = kscm__get_cell(kscm, kscm->NIL, kscm->NIL);
	kscm__type(kscm, result) = (KSCM_T_BUFFER | KSCM_T_ATOM);
	result->_object._buffer._length = len;
	result->_object._buffer._data = (unsigned char*) d;
	return result;
}
kscm_object_t kscm__mk_abstraction(kscm_t* kscm, register kscm_object_t a, register kscm_object_t b)
{
	register kscm_object_t x = kscm__get_cell(kscm, a, b);

	kscm__type(kscm, x) = KSCM_T_ABSTRACTION;
	kscm__car(kscm, x) = a;
	kscm__cdr(kscm, x) = b;
	return (x);
}
#endif

#ifdef KSCM_CONFIG_USE_OBJECTS
kscm_object_t kscm__mk_objx(kscm_t* kscm, kscm_object_t typ, long len) {
	void* d = calloc(sizeof(kscm_object_t), len);
	if (d == NULL) {
		return kscm->NIL;
	}
	kscm_object_t result = kscm__get_cell(kscm, typ, kscm->NIL);
	kscm__type(kscm, result) = KSCM_T_OBJX;
	result->_object._objx._type = typ;
	result->_object._objx._count = len;
	result->_object._objx._elements = (kscm_object_t*)d;
	long i;
	for (i = 0; i < len; i++) {
		result->_object._objx._elements[i] = kscm->NIL;
	}
	return result;
}
#endif

#ifdef KSCM_CONFIG_USE_PRECISE
int kscm__safedigit(kscm_t* kscm, int base, char d) {
	if (base <= 10) {
		if (d >= '0' && d < '0' + base) {
			return d - '0';
		}
		else {
			return -1;
		}
	}
	else if (base == 16) {
		if (d >= '0' && d < '0' + 10) {
			return d - '0';
		}
		else if (d >= 'a' && d < 'a' + 6) {
			return 10 + (d - 'a');
		}
		else if (d >= 'A' && d < 'A' + 6) {
			return 10 + (d - 'A');
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}
}
kscm_object_t kscm__mk_safenum(kscm_t* kscm, int base, const char* src) {
	char c;
	const char* p;
	int32_t v = 0;
	int negate = 0;
	p = src;
	if (p[0] == '-') {
		negate = 1;
		p++;
	}
	else if (p[0] == '+') {
		p++;
	}
	if (kscm__safedigit(kscm, base, p[0]) < 0) {
		return kscm->F;
	}
	while (kscm__safedigit(kscm, base, p[0]) >= 0) {
		int64_t lv = ((int64_t)v) * base + kscm__safedigit(kscm, base, p[0]);
		v = (int32_t)lv;
		//printf("Adding digit %d to get %d\n", kscm__safedigit(kscm, base, p[0]), v);
		if (((int64_t)v) != lv) {
			return kscm->F;
		}
		p++;
	}

	if (negate) {
		v = -v;
	}
	
	return kscm__mk_number(kscm, v);
}
#endif

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
#ifdef KSCM_CONFIG_USE_FLOATS
	bool isFloat = false;
	bool hasE = false;
	bool hasESign = false;
	char prev = ' ';
	for (; (c = *p) != 0; ++p) {
		if (!isdigit(c)) {
			if (!isFloat && c == '.') {
				isFloat = true;
			}
			else if (isFloat && !hasE && (c == 'e' || c == 'E')) {
				hasE = true;
			}
			else if (isFloat && hasE && (prev == 'e' || prev == 'E') && (c == '+' || c == '-')) {
				hasESign = true;
			}
			else {
				return (kscm__mk_symbol(kscm, q));
			}
		}
		prev = c;
	}
	if (isFloat) {
		return kscm__mk_float64(kscm, atof(q));
	}
#else
	for (; (c = *p) != 0; ++p)
		if (!isdigit(c))
			return (kscm__mk_symbol(kscm, q));
#endif
#ifdef KSCM_CONFIG_USE_PRECISE
	kscm_object_t result = kscm__mk_safenum(kscm, 10, q);
	if (result == kscm->F) {
		//fprintf(stderr, "WARNING: Math overflow in '%s'\n", q);
		result = kscm__cons(kscm, kscm__mk_symbol(kscm, "parse-number"), kscm__cons(kscm, kscm__mk_string(kscm, q), kscm->NIL));
	}
	return result;
#else
	return (kscm__mk_number(kscm, atol(q)));
#endif
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
 *
 * NOTE: The implementation is complicated a bit when using object-oriented/vector extensions.
 * I've decided to just use recursive marking in this case, but I've added some documentation to
 * the original algorithm as well in case anyone wants to update it.
 */
#ifdef KSCM_CONFIG_USE_OBJECTS
#define KSCM_GC_MAXREC 100000
void kscm__recursivemark(kscm_t* kscm, kscm_object_t a, int recursionlevel) {
	if (recursionlevel > KSCM_GC_MAXREC) {
		fprintf(stderr, "WARNING: Garbage collector is recursing like a motherfucker\n");
		recursionlevel = 0; // We just reset it though, the warning should be enough to show if it's becoming a problem
	}

	if (kscm__ismark(kscm, a)) {
		return;
	}

	kscm__setmark(kscm, a);

	if (kscm__isatom(kscm, a)) {
		return;
	}

	if (kscm__isobjx(kscm, a)) {
		kscm__recursivemark(kscm, a->_object._objx._type, recursionlevel + 1);
		int i;
		for (i = 0; i < a->_object._objx._count; i++) {
			//printf("Recursively marking object at index %d\n", i);
			kscm__recursivemark(kscm, a->_object._objx._elements[i], recursionlevel + 1);
		}
	} else { /* Is pair or pair-like abstraction*/
		kscm__recursivemark(kscm, kscm__car(kscm, a), recursionlevel + 1);
		kscm__recursivemark(kscm, kscm__cdr(kscm, a), recursionlevel + 1);
	}
}
void kscm__mark(kscm_t* kscm, kscm_object_t a) {
	kscm__recursivemark(kscm, a, 1);
}
#else
void kscm__mark(kscm_t* kscm, kscm_object_t a)
{
	register kscm_object_t t; /* Used to track the previous object. This object will in turn be used to track it's previous. */
	register kscm_object_t q; /* Used as a temporary value to hold our subreferences. */
	register kscm_object_t p; /* Points to the current object. */

/* E1: Start of algorithm. Reset t and p. */
E1:	t = (kscm_object_t)0;
	p = a;
/* E2: Start by marking p (i.e. marking it as "keep this cell"). */
E2:	kscm__setmark(kscm, p);
/* E3: Check type. If it's an atom (not built out of references to other cells) we can skip marking references. */
E3:	if (kscm__isatom(kscm, p))
goto E6;
/* E4: Mark first reference ("car" or equivalent) if it's not already marked. */
E4:	
	q = kscm__car(kscm, p);
	if (q && !kscm__ismark(kscm, q)) {
		kscm__setatom(kscm, p);
		kscm__car(kscm, p) = t;
		t = p;
		p = q;
		goto E2;
	}
/* E5: Mark second/nth references if they're not already marked. */
E5:	q = kscm__cdr(kscm, p);
if (q && !kscm__ismark(kscm, q)) {
	kscm__cdr(kscm, p) = t;
	t = p;
	p = q;
	goto E2;
}
/* E6: This object is now fully marked. If there's no previous object, we can just return. Otherwise,
 * we reload the previous object (and set the new previous to the one stored in "car" or equivalent),
 * and continue marking it's subreferences.
 */
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
#endif


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

	/* mark thread values (this implementation is single-threaded but we keep them anyway) */
	kscm__mark(kscm, kscm->_threadname);
	kscm__mark(kscm, kscm->_threadopts);
	kscm__mark(kscm, kscm->_threadobject);

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
				if (kscm__isstring(kscm, p)) {
					if (p->_object._string._svalue != NULL) {
						free(p->_object._string._svalue);
					}
				}
#ifdef KSCM_CONFIG_USE_STRUCTS
				if (kscm__isbuffer(kscm, p)) {
					if (p->_object._buffer._data != NULL) {
						//fprintf(stderr, "Freeing a buffer of %d length\n", p->_object._buffer._length);
						free(p->_object._buffer._data);
						p->_object._buffer._data = NULL;
					}
				}
#endif
#ifdef KSCM_CONFIG_USE_OBJECTS
				if (kscm__isobjx(kscm, p)) {
					if (p->_object._objx._elements != NULL) {
						free(p->_object._objx._elements);
						p->_object._objx._elements = NULL;
					}
				}
#endif
				// TODO: Should probably clear the whole structure before setting defaults or adding to free list
				// (this could help avoid bugs if larger-than-pair structures aren't cleared or reset properly elsewhere)
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

/* get new character from input file */
int     kscm__inchar(kscm_t* kscm)
{
	if (kscm->currentline >= kscm->endline) {	/* input buffer is empty */
		if (feof(kscm->inputs[kscm->inputtop])) {
			fclose(kscm->inputs[kscm->inputtop]);
			if (kscm->inputtop > 0) { // return to outer input
				kscm->inputs[kscm->inputtop] = NULL;
				kscm->inputtop--;
				if (kscm->inputs[kscm->inputtop] == stdin) {
					if (!kscm->quiet)
						{printf(KSCM_CONFIG_PROMPT);fflush(stdout);}
				}
			}
			else { // go back to the top-level
				kscm->inputs[kscm->inputtop] = stdin;
				if (!kscm->quiet)
					{printf(KSCM_CONFIG_PROMPT);fflush(stdout);}
			}
		}
		strcpy(kscm->linebuff, "\n"); // TODO: Why's this here? -Zak.
		if (fgets(kscm->currentline = kscm->linebuff, LINESIZE, kscm->inputs[kscm->inputtop]) == NULL)
			if (kscm->inputs[kscm->inputtop] == stdin) {
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
void kscm__resetinput(kscm_t* kscm)
{
	/*if (kscm->inputs[kscm->inputtop] != stdin) {
		fclose(kscm->inputs[kscm->inputtop]);
		kscm->inputs[kscm->inputtop] = stdin;
	}*/
	while (kscm->inputtop > 0 || kscm->inputs[kscm->inputtop] != stdin) {
		fclose(kscm->inputs[kscm->inputtop]);
		if (kscm->inputtop > 0) { // return to outer input
			kscm->inputs[kscm->inputtop] = NULL;
			kscm->inputtop--;
		}
		else { // go back to the top-level
			kscm->inputs[kscm->inputtop] = stdin;
			if (!kscm->quiet)
				{printf(KSCM_CONFIG_PROMPT);fflush(stdout);}
		}
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
		return (KSCM_TOK_LPAREN);
	case ')':
		return (KSCM_TOK_RPAREN);
	case '.':
		return (KSCM_TOK_DOT);
	case '\'':
		return (KSCM_TOK_QUOTE);
	case ';':
		return (KSCM_TOK_COMMENT);
	case '"':
		return (KSCM_TOK_DQUOTE);
#ifdef KSCM_CONFIG_USE_QQUOTE
	case BACKQUOTE:
		return (KSCM_TOK_BQUOTE);
	case ',':
		if (kscm__inchar(kscm) == '@')
			return (KSCM_TOK_ATMARK);
		else {
			kscm__backchar(kscm);
			return (KSCM_TOK_COMMA);
		}
#endif
	case '#':
		return (KSCM_TOK_SHARP);
	default:
		kscm__backchar(kscm);
		return (KSCM_TOK_ATOM);
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
	else if (kscm__isfloat64(kscm, l)) {
		p = kscm->strbuff;
		sprintf(p, "%f", l->_object._float64._dvalue);
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
#ifdef KSCM_CONFIG_USE_STRUCTS
	}
	else if (kscm__isbuffer(kscm, l)) {
		p = (char*)(void*)"#<BUFFER>";
	}
	else if (kscm__isabstraction(kscm, l)) {
		p = (char*)(void*)"#<ABSTRACTION>";
#endif
#ifdef KSCM_CONFIG_USE_OBJECTS
	}
	else if (kscm__isobjx(kscm, l)) {
		p = (char*)(void*)"#<OBJECT>";
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

	kscm__type(kscm, x) = KSCM_T_CLOSURE;
	kscm__car(kscm, x) = c;
	kscm__cdr(kscm, x) = e;
	return (x);
}

/* make continuation. */
kscm_object_t kscm__mk_continuation(kscm_t* kscm, register kscm_object_t d)
{
	register kscm_object_t x = kscm__get_cell(kscm, kscm->NIL, d);

	kscm__type(kscm, x) = KSCM_T_CONTINUATION;
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
			return (!strcmp(kscm__strvalue(kscm, a), kscm__strvalue(kscm, b)));
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
    kscm->_operator = (short)KSCM_OP_ERR0;                 \
    return kscm->T; KSCM__END

#define kscm__error_1(kscm,s, a) KSCM__BEGIN                    \
    kscm->args = kscm__cons(kscm, (a), kscm->NIL);                     \
    kscm->args = kscm__cons(kscm, kscm__mk_string(kscm, (s)), kscm->args);         \
    kscm->_operator = (short)KSCM_OP_ERR0;                 \
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



kscm_object_t kscm__opexe_0(kscm_t* kscm, register short op)
{
	register kscm_object_t x;
	register kscm_object_t y = NULL;

	switch (op) {
	case KSCM_OP_LOAD:		/* load */
		if (!kscm__isstring(kscm, kscm__car(kscm, kscm->args))) {
			kscm__error_0(kscm, "load -- argument is not string");
		}
		if (kscm->inputtop + 1 >= KSCM_CONFIG_MAXLOADS) {
			kscm__error_0(kscm, "load -- depth of loaded files has reached the KSCM_CONFIG_MAXLOADS value");
		}
		if ((kscm->inputs[kscm->inputtop + 1] = fopen(kscm__strvalue(kscm, kscm__car(kscm, kscm->args)), "r")) == NULL) {
			//kscm->inputs[kscm->inputtop] = stdin;
			kscm__error_1(kscm, "Unable to open", kscm__car(kscm, kscm->args));
		}
		kscm->inputtop++;
		if (!kscm->quiet)
			fprintf(kscm->outfp, "loading %s", kscm__strvalue(kscm, kscm__car(kscm, kscm->args)));
		kscm__s_goto(kscm, KSCM_OP_T0LVL);

	case KSCM_OP_T0LVL:	/* top level */
		if (!kscm->quiet)
			fprintf(kscm->outfp, "\n");
		kscm->dump = kscm->NIL;
		kscm->envir = kscm->global_env;
		kscm__s_save(kscm, KSCM_OP_VALUEPRINT, kscm->NIL, kscm->NIL);
		kscm__s_save(kscm, KSCM_OP_T1LVL, kscm->NIL, kscm->NIL);
		if (kscm->inputs[kscm->inputtop] == stdin && !kscm->quiet)
			{printf(KSCM_CONFIG_PROMPT);fflush(stdout);}
		kscm__s_goto(kscm, KSCM_OP_READ);

	case KSCM_OP_T1LVL:	/* top level */
		kscm->code = kscm->value;
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_READ:		/* read */
		kscm->tok = kscm__token(kscm);
		kscm__s_goto(kscm, KSCM_OP_RDSEXPR);

	case KSCM_OP_VALUEPRINT:	/* print evalution result */
		kscm->print_flag = 1;
		kscm->args = kscm->value;
		if (kscm->quiet) {
			kscm__s_goto(kscm, KSCM_OP_T0LVL);
		}
		else {
			kscm__s_save(kscm, KSCM_OP_T0LVL, kscm->NIL, kscm->NIL);
			kscm__s_goto(kscm, KSCM_OP_P0LIST);
		}

	case KSCM_OP_EVAL:		/* main part of evalution */
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
				kscm__s_save(kscm, KSCM_OP_E0ARGS, kscm->NIL, kscm->code);
#else
				s_save(kscm, KSCM_OP_E1ARGS, kscm->NIL, cdr(code));
#endif
				kscm->code = kscm__car(kscm, kscm->code);
				kscm__s_goto(kscm, KSCM_OP_EVAL);
			}
		}
		else {
			kscm__s_return(kscm, kscm->code);
		}

#ifdef KSCM_CONFIG_USE_MACRO
	case KSCM_OP_E0ARGS:	/* eval arguments */
		if (kscm__ismacro(kscm, kscm->value)) {	/* macro expansion */
			kscm__s_save(kscm, KSCM_OP_DOMACRO, kscm->NIL, kscm->NIL);
			kscm->args = kscm__cons(kscm, kscm->code, kscm->NIL);
			kscm->code = kscm->value;
			kscm__s_goto(kscm, KSCM_OP_APPLY);
		}
		else {
			kscm->code = kscm__cdr(kscm, kscm->code);
			kscm__s_goto(kscm, KSCM_OP_E1ARGS);
		}
#endif

	case KSCM_OP_E1ARGS:	/* eval arguments */
		kscm->args = kscm__cons(kscm, kscm->value, kscm->args);
		if (kscm__ispair(kscm, kscm->code)) {	/* continue */
			kscm__s_save(kscm, KSCM_OP_E1ARGS, kscm->args, kscm__cdr(kscm, kscm->code));
			kscm->code = kscm__car(kscm, kscm->code);
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, KSCM_OP_EVAL);
		}
		else {	/* end */
			kscm->args = kscm__reverse(kscm, kscm->args);
			kscm->code = kscm__car(kscm, kscm->args);
			kscm->args = kscm__cdr(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_APPLY);
		}

	case KSCM_OP_APPLY:		/* apply 'code' to 'args' */
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
			kscm__s_goto(kscm, KSCM_OP_BEGIN);
		}
		else if (kscm__iscontinuation(kscm, kscm->code)) {	/* CONTINUATION */
			kscm->dump = kscm__cont_dump(kscm, kscm->code);
			kscm__s_return(kscm, kscm->args != kscm->NIL ? kscm__car(kscm, kscm->args) : kscm->NIL);
		}
		else {
			kscm__error_0(kscm, "Illegal function");
		}

#ifdef KSCM_CONFIG_USE_MACRO
	case KSCM_OP_DOMACRO:	/* do macro */
		kscm->code = kscm->value;
		kscm__s_goto(kscm, KSCM_OP_EVAL);
#endif

	case KSCM_OP_LAMBDA:	/* lambda */
		kscm__s_return(kscm, kscm__mk_closure(kscm, kscm->code, kscm->envir));

	case KSCM_OP_QUOTE:		/* quote */
		kscm__s_return(kscm, kscm__car(kscm, kscm->code));

	case KSCM_OP_DEF0:	/* define */
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
		kscm__s_save(kscm, KSCM_OP_DEF1, kscm->NIL, x);
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_DEF1:	/* define */
		for (x = kscm__car(kscm, kscm->envir); x != kscm->NIL; x = kscm__cdr(kscm, x))
			if (kscm__caar(kscm, x) == kscm->code)
				break;
		if (x != kscm->NIL)
			kscm__cdar(kscm, x) = kscm->value;
		else
			kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, kscm->code, kscm->value), kscm__car(kscm, kscm->envir));
		kscm__s_return(kscm, kscm->code);

	case KSCM_OP_SET0:		/* set! */
		kscm__s_save(kscm, KSCM_OP_SET1, kscm->NIL, kscm__car(kscm, kscm->code));
		kscm->code = kscm__cadr(kscm, kscm->code);
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_SET1:		/* set! */
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

	case KSCM_OP_BEGIN:		/* begin */
		if (!kscm__ispair(kscm, kscm->code)) {
			kscm__s_return(kscm, kscm->code);
		}
		if (kscm__cdr(kscm, kscm->code) != kscm->NIL) {
			kscm__s_save(kscm, KSCM_OP_BEGIN, kscm->NIL, kscm__cdr(kscm, kscm->code));
		}
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_IF0:		/* if */
		kscm__s_save(kscm, KSCM_OP_IF1, kscm->NIL, kscm__cdr(kscm, kscm->code));
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_IF1:		/* if */
		if (kscm__istrue(kscm, kscm->value))
			kscm->code = kscm__car(kscm, kscm->code);
		else
			kscm->code = kscm__cadr(kscm, kscm->code);	/* (if #f 1) ==> () because
						 * car(kscm->NIL) = kscm->NIL */
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_LET0:		/* let */
		kscm->args = kscm->NIL;
		kscm->value = kscm->code;
		kscm->code = kscm__issymbol(kscm, kscm__car(kscm, kscm->code)) ? kscm__cadr(kscm, kscm->code) : kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, KSCM_OP_LET1);

	case KSCM_OP_LET1:		/* let (caluculate parameters) */
		kscm->args = kscm__cons(kscm, kscm->value, kscm->args);
		if (kscm__ispair(kscm, kscm->code)) {	/* continue */
			kscm__s_save(kscm, KSCM_OP_LET1, kscm->args, kscm__cdr(kscm, kscm->code));
			kscm->code = kscm__cadar(kscm, kscm->code);
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, KSCM_OP_EVAL);
		}
		else {	/* end */
			kscm->args = kscm__reverse(kscm, kscm->args);
			kscm->code = kscm__car(kscm, kscm->args);
			kscm->args = kscm__cdr(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_LET2);
		}

	case KSCM_OP_LET2:		/* let */
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
		kscm__s_goto(kscm, KSCM_OP_BEGIN);

	case KSCM_OP_LET0AST:	/* let* */
		if (kscm__car(kscm, kscm->code) == kscm->NIL) {
			kscm->envir = kscm__cons(kscm, kscm->NIL, kscm->envir);
			kscm->code = kscm__cdr(kscm, kscm->code);
			kscm__s_goto(kscm, KSCM_OP_BEGIN);
		}
		kscm__s_save(kscm, KSCM_OP_LET1AST, kscm__cdr(kscm, kscm->code), kscm__car(kscm, kscm->code));
		kscm->code = kscm__cadaar(kscm, kscm->code);
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_LET1AST:	/* let* (make new frame) */
		kscm->envir = kscm__cons(kscm, kscm->NIL, kscm->envir);
		kscm__s_goto(kscm, KSCM_OP_LET2AST);

	case KSCM_OP_LET2AST:	/* let* (caluculate parameters) */
		kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, kscm__caar(kscm, kscm->code), kscm->value), kscm__car(kscm, kscm->envir));
		kscm->code = kscm__cdr(kscm, kscm->code);
		if (kscm__ispair(kscm, kscm->code)) {	/* continue */
			kscm__s_save(kscm, KSCM_OP_LET2AST, kscm->args, kscm->code);
			kscm->code = kscm__cadar(kscm, kscm->code);
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, KSCM_OP_EVAL);
		}
		else {	/* end */
			kscm->code = kscm->args;
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, KSCM_OP_BEGIN);
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
	case KSCM_OP_LET0REC:	/* letrec */
		kscm->envir = kscm__cons(kscm, kscm->NIL, kscm->envir);
		kscm->args = kscm->NIL;
		kscm->value = kscm->code;
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, KSCM_OP_LET1REC);

	case KSCM_OP_LET1REC:	/* letrec (caluculate parameters) */
		kscm->args = kscm__cons(kscm, kscm->value, kscm->args);
		if (kscm__ispair(kscm, kscm->code)) {	/* continue */
			kscm__s_save(kscm, KSCM_OP_LET1REC, kscm->args, kscm__cdr(kscm, kscm->code));
			kscm->code = kscm__cadar(kscm, kscm->code);
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, KSCM_OP_EVAL);
		}
		else {	/* end */
			kscm->args = kscm__reverse(kscm, kscm->args);
			kscm->code = kscm__car(kscm, kscm->args);
			kscm->args = kscm__cdr(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_LET2REC);
		}

	case KSCM_OP_LET2REC:	/* letrec */
		for (x = kscm__car(kscm, kscm->code), y = kscm->args; y != kscm->NIL; x = kscm__cdr(kscm, x), y = kscm__cdr(kscm, y))
			kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, kscm__caar(kscm, x), kscm__car(kscm, y)), kscm__car(kscm, kscm->envir));
		kscm->code = kscm__cdr(kscm, kscm->code);
		kscm->args = kscm->NIL;
		kscm__s_goto(kscm, KSCM_OP_BEGIN);

	case KSCM_OP_COND0:		/* cond */
		if (!kscm__ispair(kscm, kscm->code)) {
			kscm__error_0(kscm, "Syntax error in cond");
		}
		kscm__s_save(kscm, KSCM_OP_COND1, kscm->NIL, kscm->code);
		kscm->code = kscm__caar(kscm, kscm->code);
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_COND1:		/* cond */
		if (kscm__istrue(kscm, kscm->value)) {
			if ((kscm->code = kscm__cdar(kscm, kscm->code)) == kscm->NIL) {
				kscm__s_return(kscm, kscm->value);
			}
			kscm__s_goto(kscm, KSCM_OP_BEGIN);
		}
		else {
			if ((kscm->code = kscm__cdr(kscm, kscm->code)) == kscm->NIL) {
				kscm__s_return(kscm, kscm->NIL);
			}
			else {
				kscm__s_save(kscm, KSCM_OP_COND1, kscm->NIL, kscm->code);
				kscm->code = kscm__caar(kscm, kscm->code);
				kscm__s_goto(kscm, KSCM_OP_EVAL);
			}
		}

	case KSCM_OP_DELAY:		/* delay */
		x = kscm__mk_closure(kscm, kscm__cons(kscm, kscm->NIL, kscm->code), kscm->envir);
		kscm__setpromise(kscm, x);
		kscm__s_return(kscm, x);

	case KSCM_OP_AND0:		/* and */
		if (kscm->code == kscm->NIL) {
			kscm__s_return(kscm, kscm->T);
		}
		kscm__s_save(kscm, KSCM_OP_AND1, kscm->NIL, kscm__cdr(kscm, kscm->code));
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_AND1:		/* and */
		if (kscm__isfalse(kscm, kscm->value)) {
			kscm__s_return(kscm, kscm->value);
		}
		else if (kscm->code == kscm->NIL) {
			kscm__s_return(kscm, kscm->value);
		}
		else {
			kscm__s_save(kscm, KSCM_OP_AND1, kscm->NIL, kscm__cdr(kscm, kscm->code));
			kscm->code = kscm__car(kscm, kscm->code);
			kscm__s_goto(kscm, KSCM_OP_EVAL);
		}

	case KSCM_OP_OR0:		/* or */
		if (kscm->code == kscm->NIL) {
			kscm__s_return(kscm, kscm->F);
		}
		kscm__s_save(kscm, KSCM_OP_OR1, kscm->NIL, kscm__cdr(kscm, kscm->code));
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_OR1:		/* or */
		if (kscm__istrue(kscm, kscm->value)) {
			kscm__s_return(kscm, kscm->value);
		}
		else if (kscm->code == kscm->NIL) {
			kscm__s_return(kscm, kscm->value);
		}
		else {
			kscm__s_save(kscm, KSCM_OP_OR1, kscm->NIL, kscm__cdr(kscm, kscm->code));
			kscm->code = kscm__car(kscm, kscm->code);
			kscm__s_goto(kscm, KSCM_OP_EVAL);
		}

	case KSCM_OP_C0STREAM:	/* cons-stream */
		kscm__s_save(kscm, KSCM_OP_C1STREAM, kscm->NIL, kscm__cdr(kscm, kscm->code));
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_C1STREAM:	/* cons-stream */
		kscm->args = kscm->value;	/* save value to register args for gc */
		x = kscm__mk_closure(kscm, kscm__cons(kscm, kscm->NIL, kscm->code), kscm->envir);
		kscm__setpromise(kscm, x);
		kscm__s_return(kscm, kscm__cons(kscm, kscm->args, x));

#ifdef KSCM_CONFIG_USE_MACRO
	case KSCM_OP_0MACRO:	/* macro */
		x = kscm__car(kscm, kscm->code);
		kscm->code = kscm__cadr(kscm, kscm->code);
		if (!kscm__issymbol(kscm, x)) {
			kscm__error_0(kscm, "Variable is not symbol");
		}
		kscm__s_save(kscm, KSCM_OP_1MACRO, kscm->NIL, x);
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_1MACRO:	/* macro */
		kscm__type(kscm, kscm->value) |= KSCM_T_MACRO;
		for (x = kscm__car(kscm, kscm->envir); x != kscm->NIL; x = kscm__cdr(kscm, x))
			if (kscm__caar(kscm, x) == kscm->code)
				break;
		if (x != kscm->NIL)
			kscm__cdar(kscm, x) = kscm->value;
		else
			kscm__car(kscm, kscm->envir) = kscm__cons(kscm, kscm__cons(kscm, kscm->code, kscm->value), kscm__car(kscm, kscm->envir));
		kscm__s_return(kscm, kscm->code);
#endif

	case KSCM_OP_CASE0:		/* case */
		kscm__s_save(kscm, KSCM_OP_CASE1, kscm->NIL, kscm__cdr(kscm, kscm->code));
		kscm->code = kscm__car(kscm, kscm->code);
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_CASE1:		/* case */
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
				kscm__s_goto(kscm, KSCM_OP_BEGIN);
			}
			else {/* else */
				kscm__s_save(kscm, KSCM_OP_CASE2, kscm->NIL, kscm__cdar(kscm, x));
				kscm->code = kscm__caar(kscm, x);
				kscm__s_goto(kscm, KSCM_OP_EVAL);
			}
		}
		else {
			kscm__s_return(kscm, kscm->NIL);
		}

	case KSCM_OP_CASE2:		/* case */
		if (kscm__istrue(kscm, kscm->value)) {
			kscm__s_goto(kscm, KSCM_OP_BEGIN);
		}
		else {
			kscm__s_return(kscm, kscm->NIL);
		}
	case KSCM_OP_PAPPLY:	/* apply */
		kscm->code = kscm__car(kscm, kscm->args);
		kscm->args = kscm__cadr(kscm, kscm->args);
		kscm__s_goto(kscm, KSCM_OP_APPLY);

	case KSCM_OP_PEVAL:	/* eval */
		kscm->code = kscm__car(kscm, kscm->args);
		kscm->args = kscm->NIL;
		kscm__s_goto(kscm, KSCM_OP_EVAL);

	case KSCM_OP_CONTINUATION:	/* call-with-current-continuation */
		kscm->code = kscm__car(kscm, kscm->args);
		kscm->args = kscm__cons(kscm, kscm__mk_continuation(kscm, kscm->dump), kscm->NIL);
		kscm__s_goto(kscm, KSCM_OP_APPLY);

	default:
		sprintf(kscm->strbuff, "%d is illegal operator", kscm->_operator);
		kscm__error_0(kscm, kscm->strbuff);
	}
	return kscm->T;
}


kscm_object_t kscm__opexe_2(kscm_t* kscm, register short op)
{
	register kscm_object_t x, y;
#ifdef KSCM_CONFIG_USE_PRECISE
	int32_t v;
#else
	register long v;
#endif

	switch (op) {
	case KSCM_OP_ADD:		/* + */
		for (x = kscm->args, v = 0; x != kscm->NIL; x = kscm__cdr(kscm, x)) {
			if (!kscm__isnumber(kscm, kscm__car(kscm, x))) {
				kscm__s_retbool(kscm, 0);
			}
#ifdef KSCM_CONFIG_USE_PRECISE
			int64_t lv = ((int64_t)v) + ((int64_t)kscm__ivalue(kscm, kscm__car(kscm, x)));
			v = (int32_t)lv;
			if (((int64_t)v) != lv) {
				kscm__s_retbool(kscm, 0);
			}
#else
			v += kscm__ivalue(kscm, kscm__car(kscm, x));
#endif
		}
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case KSCM_OP_SUB:		/* - */
		for (x = kscm__cdr(kscm, kscm->args), v = kscm__ivalue(kscm, kscm__car(kscm, kscm->args)); x != kscm->NIL; x = kscm__cdr(kscm, x)) {
			if (!kscm__isnumber(kscm, kscm__car(kscm, x))) {
				kscm__s_retbool(kscm, 0);
			}
#ifdef KSCM_CONFIG_USE_PRECISE
			int64_t lv = ((int64_t)v) - ((int64_t)kscm__ivalue(kscm, kscm__car(kscm, x)));
			v = (int32_t)lv;
			if (((int64_t)v) != lv) {
				kscm__s_retbool(kscm, 0);
			}
#else
			v -= kscm__ivalue(kscm, kscm__car(kscm, x));
#endif
		}
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case KSCM_OP_MUL:		/* * */
		for (x = kscm->args, v = 1; x != kscm->NIL; x = kscm__cdr(kscm, x)) {
			if (!kscm__isnumber(kscm, kscm__car(kscm, x))) {
				kscm__s_retbool(kscm, 0);
			}
#ifdef KSCM_CONFIG_USE_PRECISE
			int64_t lv = ((int64_t)v) * ((int64_t)kscm__ivalue(kscm, kscm__car(kscm, x)));
			v = (int32_t)lv;
			if (((int64_t)v) != lv) {
				kscm__s_retbool(kscm, 0);
			}
#else
			v *= kscm__ivalue(kscm, kscm__car(kscm, x));
#endif
		}
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case KSCM_OP_DIV:		/* / */
		for (x = kscm__cdr(kscm, kscm->args), v = kscm__ivalue(kscm, kscm__car(kscm, kscm->args)); x != kscm->NIL; x = kscm__cdr(kscm, x)) {
			if (!kscm__isnumber(kscm, kscm__car(kscm, x))) {
				kscm__s_retbool(kscm, 0);
			}
			if (kscm__ivalue(kscm, kscm__car(kscm, x)) != 0)
				v /= kscm__ivalue(kscm, kscm__car(kscm, x));
			else {
#ifdef KSCM_CONFIG_USE_PRECISE
				kscm__s_retbool(kscm, 0);
#else
				kscm__error_0(kscm, "Divided by zero");
#endif
			}
		}
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case KSCM_OP_REM:		/* remainder */
		for (x = kscm__cdr(kscm, kscm->args), v = kscm__ivalue(kscm, kscm__car(kscm, kscm->args)); x != kscm->NIL; x = kscm__cdr(kscm, x)) {
			if (!kscm__isnumber(kscm, kscm__car(kscm, x))) {
				kscm__s_retbool(kscm, 0);
			}
			if (kscm__ivalue(kscm, kscm__car(kscm, x)) != 0)
				v %= kscm__ivalue(kscm, kscm__car(kscm, x));
			else {
#ifdef KSCM_CONFIG_USE_PRECISE
				kscm__s_retbool(kscm, 0);
#else
				kscm__error_0(kscm, "Divided by zero");
#endif
			}
		}
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case KSCM_OP_CAR:		/* car */
		if (kscm__ispair(kscm, kscm__car(kscm, kscm->args))) {
			kscm__s_return(kscm, kscm__caar(kscm, kscm->args));
		}
		else {
			kscm__error_0(kscm, "Unable to car for non-cons cell");
		}

	case KSCM_OP_CDR:		/* cdr */
		if (kscm__ispair(kscm, kscm__car(kscm, kscm->args))) {
			kscm__s_return(kscm, kscm__cdar(kscm, kscm->args));
		}
		else {
			kscm__error_0(kscm, "Unable to cdr for non-cons cell");
		}

	case KSCM_OP_CONS:		/* cons */
		kscm__cdr(kscm, kscm->args) = kscm__cadr(kscm, kscm->args);
		kscm__s_return(kscm, kscm->args);

	case KSCM_OP_SETCAR:	/* set-car! */
		if (kscm__ispair(kscm, kscm__car(kscm, kscm->args))) {
			kscm__caar(kscm, kscm->args) = kscm__cadr(kscm, kscm->args);
			kscm__s_return(kscm, kscm__car(kscm, kscm->args));
		}
		else {
			kscm__error_0(kscm, "Unable to set-car! for non-cons cell");
		}

	case KSCM_OP_SETCDR:	/* set-cdr! */
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
	case KSCM_OP_NOT:		/* not */
		kscm__s_retbool(kscm, kscm__isfalse(kscm, kscm__car(kscm, kscm->args)));
	case KSCM_OP_BOOL:		/* boolean? */
		kscm__s_retbool(kscm, kscm__car(kscm, kscm->args) == kscm->F || kscm__car(kscm, kscm->args) == kscm->T);
	case KSCM_OP_NULL:		/* null? */
		kscm__s_retbool(kscm, kscm__car(kscm, kscm->args) == kscm->NIL);
	case KSCM_OP_ZEROP:		/* zero? */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) == 0);
	case KSCM_OP_POSP:		/* positive? */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) > 0);
	case KSCM_OP_NEGP:		/* negative? */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) < 0);
	case KSCM_OP_NEQ:		/* = */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) == kscm__ivalue(kscm, kscm__cadr(kscm, kscm->args)));
	case KSCM_OP_LESS:		/* < */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) < kscm__ivalue(kscm, kscm__cadr(kscm, kscm->args)));
	case KSCM_OP_GRE:		/* > */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) > kscm__ivalue(kscm, kscm__cadr(kscm, kscm->args)));
	case KSCM_OP_LEQ:		/* <= */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) <= kscm__ivalue(kscm, kscm__cadr(kscm, kscm->args)));
	case KSCM_OP_GEQ:		/* >= */
		kscm__s_retbool(kscm, kscm__ivalue(kscm, kscm__car(kscm, kscm->args)) >= kscm__ivalue(kscm, kscm__cadr(kscm, kscm->args)));
	case KSCM_OP_SYMBOL:	/* symbol? */
		kscm__s_retbool(kscm, kscm__issymbol(kscm, kscm__car(kscm, kscm->args)));
	case KSCM_OP_NUMBER:	/* number? */
		kscm__s_retbool(kscm, kscm__isnumber(kscm, kscm__car(kscm, kscm->args)));
	case KSCM_OP_STRING:	/* string? */
		kscm__s_retbool(kscm, kscm__isstring(kscm, kscm__car(kscm, kscm->args)));
	case KSCM_OP_PROC:		/* procedure? */
		/*--
			 * continuation should be procedure by the example
			 * (call-with-current-continuation procedure?) ==> #t
				 * in R^3 report sec. 6.9
			 */
		kscm__s_retbool(kscm, kscm__isproc(kscm, kscm__car(kscm, kscm->args)) || kscm__isclosure(kscm, kscm__car(kscm, kscm->args))
			|| kscm__iscontinuation(kscm, kscm__car(kscm, kscm->args)));
	case KSCM_OP_PAIR:		/* pair? */
		kscm__s_retbool(kscm, kscm__ispair(kscm, kscm__car(kscm, kscm->args)));
	case KSCM_OP_EQ:		/* eq? */
		kscm__s_retbool(kscm, kscm__car(kscm, kscm->args) == kscm__cadr(kscm, kscm->args));
	case KSCM_OP_EQV:		/* eqv? */
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
	case KSCM_OP_FORCE:		/* force */
		kscm->code = kscm__car(kscm, kscm->args);
		if (kscm__ispromise(kscm, kscm->code)) {
			kscm->args = kscm->NIL;
			kscm__s_goto(kscm, KSCM_OP_APPLY);
		}
		else {
			kscm__s_return(kscm, kscm->code);
		}

	case KSCM_OP_WRITE:		/* write */
		kscm->print_flag = 1;
		kscm->args = kscm__car(kscm, kscm->args);
		kscm__s_goto(kscm, KSCM_OP_P0LIST);

	case KSCM_OP_DISPLAY:	/* display */
		kscm->print_flag = 0;
		kscm->args = kscm__car(kscm, kscm->args);
		kscm__s_goto(kscm, KSCM_OP_P0LIST);

	case KSCM_OP_NEWLINE:	/* newline */
		fprintf(kscm->outfp, "\n");
		kscm__s_return(kscm, kscm->T);

	case KSCM_OP_ERR0:	/* error */
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
		kscm__s_goto(kscm, KSCM_OP_ERR1);

	case KSCM_OP_ERR1:	/* error */
		fprintf(kscm->outfp, " ");
		if (kscm->args != kscm->NIL) {
			kscm__s_save(kscm, KSCM_OP_ERR1, kscm__cdr(kscm, kscm->args), kscm->NIL);
			kscm->args = kscm__car(kscm, kscm->args);
			kscm->print_flag = 1;
			kscm__s_goto(kscm, KSCM_OP_P0LIST);
		}
		else {
			fprintf(kscm->outfp, "\n");
			kscm__resetinput(kscm);
			kscm->outfp = kscm->tmpfp;
			kscm__s_goto(kscm, KSCM_OP_T0LVL);
		}

	case KSCM_OP_REVERSE:	/* reverse */
		kscm__s_return(kscm, kscm__reverse(kscm, kscm__car(kscm, kscm->args)));

	case KSCM_OP_APPEND:	/* append */
		kscm__s_return(kscm, kscm__append(kscm, kscm__car(kscm, kscm->args), kscm__cadr(kscm, kscm->args)));

	case KSCM_OP_PUT:		/* put */
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

	case KSCM_OP_GET:		/* get */
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

	case KSCM_OP_QUIT:		/* quit */
		return (kscm->NIL);

	case KSCM_OP_GC:		/* gc */
		kscm__gc(kscm, kscm->NIL, kscm->NIL);
		kscm__s_return(kscm, kscm->T);

	case KSCM_OP_GCVERB:		/* gc-verbose */
	{	int	was = kscm->gc_verbose;

	kscm->gc_verbose = (kscm__car(kscm, kscm->args) != kscm->F);
	kscm__s_retbool(kscm, was);
	}

	case KSCM_OP_NEWSEGMENT:	/* new-segment */
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
	case KSCM_OP_RDSEXPR:
		switch (kscm->tok) {
		case KSCM_TOK_COMMENT:
			while (kscm__inchar(kscm) != '\n')
				;
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, KSCM_OP_RDSEXPR);
		case KSCM_TOK_LPAREN:
			kscm->tok = kscm__token(kscm);
			if (kscm->tok == KSCM_TOK_RPAREN) {
				kscm__s_return(kscm, kscm->NIL);
			}
			else if (kscm->tok == KSCM_TOK_DOT) {
				kscm__error_0(kscm, "syntax error -- illegal dot expression");
			}
			else {
				kscm__s_save(kscm, KSCM_OP_RDLIST, kscm->NIL, kscm->NIL);
				kscm__s_goto(kscm, KSCM_OP_RDSEXPR);
			}
		case KSCM_TOK_QUOTE:
			kscm__s_save(kscm, KSCM_OP_RDQUOTE, kscm->NIL, kscm->NIL);
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, KSCM_OP_RDSEXPR);
#ifdef KSCM_CONFIG_USE_QQUOTE
		case KSCM_TOK_BQUOTE:
			kscm__s_save(kscm, KSCM_OP_RDQQUOTE, kscm->NIL, kscm->NIL);
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, KSCM_OP_RDSEXPR);
		case KSCM_TOK_COMMA:
			kscm__s_save(kscm, KSCM_OP_RDUNQUOTE, kscm->NIL, kscm->NIL);
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, KSCM_OP_RDSEXPR);
		case KSCM_TOK_ATMARK:
			kscm__s_save(kscm, KSCM_OP_RDUQTSP, kscm->NIL, kscm->NIL);
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, KSCM_OP_RDSEXPR);
#endif
		case KSCM_TOK_ATOM:
			kscm__s_return(kscm, kscm__mk_atom(kscm, kscm__readstr(kscm, "();\t\n ")));
		case KSCM_TOK_DQUOTE:
			kscm__s_return(kscm, kscm__mk_string(kscm, kscm__readstrexp(kscm)));
		case KSCM_TOK_SHARP:
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

	case KSCM_OP_RDLIST:
		kscm->args = kscm__cons(kscm, kscm->value, kscm->args);
		kscm->tok = kscm__token(kscm);
		if (kscm->tok == KSCM_TOK_COMMENT) {
			while (kscm__inchar(kscm) != '\n')
				;
			kscm->tok = kscm__token(kscm);
		}
		if (kscm->tok == KSCM_TOK_RPAREN) {
			kscm__s_return(kscm, kscm__non_alloc_rev(kscm, kscm->NIL, kscm->args));
		}
		else if (kscm->tok == KSCM_TOK_DOT) {
			kscm__s_save(kscm, KSCM_OP_RDDOT, kscm->args, kscm->NIL);
			kscm->tok = kscm__token(kscm);
			kscm__s_goto(kscm, KSCM_OP_RDSEXPR);
		}
		else {
			kscm__s_save(kscm, KSCM_OP_RDLIST, kscm->args, kscm->NIL);;
			kscm__s_goto(kscm, KSCM_OP_RDSEXPR);
		}

	case KSCM_OP_RDDOT:
		if (kscm__token(kscm) != KSCM_TOK_RPAREN) {
			kscm__error_0(kscm, "syntax error -- illegal dot expression");
		}
		else {
			kscm__s_return(kscm, kscm__non_alloc_rev(kscm, kscm->value, kscm->args));
		}

	case KSCM_OP_RDQUOTE:
		kscm__s_return(kscm, kscm__cons(kscm, kscm->QUOTE, kscm__cons(kscm, kscm->value, kscm->NIL)));

#ifdef KSCM_CONFIG_USE_QQUOTE
	case KSCM_OP_RDQQUOTE:
		kscm__s_return(kscm, kscm__cons(kscm, kscm->QQUOTE, kscm__cons(kscm, kscm->value, kscm->NIL)));

	case KSCM_OP_RDUNQUOTE:
		kscm__s_return(kscm, kscm__cons(kscm, kscm->UNQUOTE, kscm__cons(kscm, kscm->value, kscm->NIL)));

	case KSCM_OP_RDUQTSP:
		kscm__s_return(kscm, kscm__cons(kscm, kscm->UNQUOTESP, kscm__cons(kscm, kscm->value, kscm->NIL)));
#endif

		/* ========== printing part ========== */
	case KSCM_OP_P0LIST:
		if (!kscm__ispair(kscm, kscm->args)) {
			kscm__printatom(kscm, kscm->args, kscm->print_flag);
			kscm__s_return(kscm, kscm->T);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->QUOTE && kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			fprintf(kscm->outfp, "'");
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0LIST);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->QQUOTE && kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			fprintf(kscm->outfp, "`");
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0LIST);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->UNQUOTE && kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			fprintf(kscm->outfp, ",");
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0LIST);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->UNQUOTESP && kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			fprintf(kscm->outfp, ",@");
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0LIST);
		}
		else {
			fprintf(kscm->outfp, "(");
			kscm__s_save(kscm, KSCM_OP_P1LIST, kscm__cdr(kscm, kscm->args), kscm->NIL);
			kscm->args = kscm__car(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0LIST);
		}

	case KSCM_OP_P1LIST:
		if (kscm__ispair(kscm, kscm->args)) {
			kscm__s_save(kscm, KSCM_OP_P1LIST, kscm__cdr(kscm, kscm->args), kscm->NIL);
			fprintf(kscm->outfp, " ");
			kscm->args = kscm__car(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0LIST);
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
	case KSCM_OP_LIST_LENGTH:	/* list-length */	/* a.k */
		for (x = kscm__car(kscm, kscm->args), v = 0; kscm__ispair(kscm, x); x = kscm__cdr(kscm, x))
			++v;
		kscm__s_return(kscm, kscm__mk_number(kscm, v));

	case KSCM_OP_ASSQ:		/* assq */	/* a.k */
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

	case KSCM_OP_PRINT_WIDTH:	/* print-width */	/* a.k */
		w = 0;
		kscm->args = kscm__car(kscm, kscm->args);
		kscm->print_flag = -1;
		kscm__s_goto(kscm, KSCM_OP_P0_WIDTH);

	case KSCM_OP_P0_WIDTH:
		if (!kscm__ispair(kscm, kscm->args)) {
			w += kscm__printatom(kscm, kscm->args, kscm->print_flag);
			kscm__s_return(kscm, kscm__mk_number(kscm, w));
		}
		else if (kscm__car(kscm, kscm->args) == kscm->QUOTE
			&& kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			++w;
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0_WIDTH);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->QQUOTE
			&& kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			++w;
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0_WIDTH);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->UNQUOTE
			&& kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			++w;
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0_WIDTH);
		}
		else if (kscm__car(kscm, kscm->args) == kscm->UNQUOTESP
			&& kscm__ok_abbrev(kscm, kscm__cdr(kscm, kscm->args))) {
			w += 2;
			kscm->args = kscm__cadr(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0_WIDTH);
		}
		else {
			++w;
			kscm__s_save(kscm, KSCM_OP_P1_WIDTH, kscm__cdr(kscm, kscm->args), kscm->NIL);
			kscm->args = kscm__car(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0_WIDTH);
		}

	case KSCM_OP_P1_WIDTH:
		if (kscm__ispair(kscm, kscm->args)) {
			kscm__s_save(kscm, KSCM_OP_P1_WIDTH, kscm__cdr(kscm, kscm->args), kscm->NIL);
			++w;
			kscm->args = kscm__car(kscm, kscm->args);
			kscm__s_goto(kscm, KSCM_OP_P0_WIDTH);
		}
		else {
			if (kscm->args != kscm->NIL)
				w += 3 + kscm__printatom(kscm, kscm->args, kscm->print_flag);
			++w;
			kscm__s_return(kscm, kscm__mk_number(kscm, w));
		}

	case KSCM_OP_GET_CLOSURE:	/* get-closure-code */	/* a.k */
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
	case KSCM_OP_CLOSUREP:		/* closure? */
		/*
		 * Note, macro object is also a closure.
		 * Therefore, (closure? <#MACRO>) ==> #t
		 */
		if (kscm__car(kscm, kscm->args) == kscm->NIL) {
			kscm__s_return(kscm, kscm->F);
		}
		kscm__s_retbool(kscm, kscm__isclosure(kscm, kscm__car(kscm, kscm->args)));
#ifdef KSCM_CONFIG_USE_MACRO
	case KSCM_OP_MACROP:		/* macro? */
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

#ifdef KSCM_CONFIG_USE_PERSIST
unsigned int kscm_get_persistent_address(kscm_t* kscm, kscm_object_t obj) {
	int i;
	if (obj == kscm->NIL) {
		return 0;
	} else if (obj == kscm->F) {
		return 1;
	} else if (obj == kscm->T) {
		return 2;
	}

	uintptr_t addr = (uintptr_t)obj;
	//fprintf(stderr, "addr is %d\n", addr);
	for (i = 0; i <= kscm->last_cell_seg; i++) {
		uintptr_t segaddr = (uintptr_t)kscm->cell_seg[i];
		//fprintf(stderr, "segaddr %d is %d\n", i, segaddr);
		if (segaddr != 0 && addr >= segaddr && addr < segaddr + (KSCM_CONFIG_CELL_SEGSIZE * sizeof(struct kscm_cell))) {
			//fprintf(stderr, "it's in here!\n");
			int segment = i + 1;
			uintptr_t offset = addr - segaddr;
			if ((offset % sizeof(struct kscm_cell))) {
				return -1;
			}
			int index = offset / sizeof(struct kscm_cell);
			return (segment * KSCM_CONFIG_CELL_SEGSIZE) + index;
		}
	}

	return -1;
}
int kscm__fwrite_byte(kscm_t* kscm, FILE* f, char val) {
	fputc(val, f);
	return 1;
}
int kscm__fwrite_int(kscm_t* kscm, FILE* f, int val) {
	//fprintf(stderr, "writing %x %x %x %x\n", val & 0xFF, (val >> 8) & 0xFF, (val >> 16) & 0xFF, (val >> 24) & 0xFF);
	fputc((val & 0xFF), f);
	fputc(((val >> 8) & 0xFF), f);
	fputc(((val >> 16) & 0xFF), f);
	fputc(((val >> 24) & 0xFF), f);
	return 4;
}
int kscm__fwrite_strl(kscm_t* kscm, FILE* f, const char* str, int len) {
	if (str == NULL) {
		return kscm__fwrite_int(kscm, f, 0);
	}
	int written = kscm__fwrite_int(kscm, f, len);
	if (written != 4) {
		return written;
	}
	int i;
	for (i = 0; i < len; i++) {
		fputc(str[i], f);
		written++;
	}
	return written;
}
int kscm__fwrite_str(kscm_t* kscm, FILE* f, const char* str) {
	return kscm__fwrite_strl(kscm, f, str, strlen(str));
}

int kscm_save_state(kscm_t* kscm, const char* filename, const char* opts) {
	if (filename == NULL || strlen(filename) < 1) {
		fprintf(stderr, "Filename expected\n");
		return -1;
	}
	FILE* f = fopen(filename, "wb");
	if (f == NULL) {
		fprintf(stderr, "Failed to open '%s' for writing\n", filename);
		return -1; // Not saved
	}
	fprintf(stderr, "Saving state to '%s' opts '%s'...\n", filename, opts);

	kscm__fwrite_str(kscm, f, KSCM_CONFIG_PERSIST_MAGIC);
	//fclose(f); if (1) return 0;
	kscm__fwrite_int(kscm, f, KSCM_CONFIG_PERSIST_VERSION);
	
	// Bytes per id (higher bits may be reused later for flags)
	kscm__fwrite_int(kscm, f, kscm->_stateformat);

	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->NIL));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->F));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->T));

	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->oblist));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->global_env));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->LAMBDA));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->QUOTE));

	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->QQUOTE));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->UNQUOTE));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->UNQUOTESP));

	/* NOTE: The format has been reorganised (as of "version 2" of the format) to allow for
	 * multithreading, which isn't supported on this implementation, but now thread-specific
	 * data is stored separately.
	 */
	/* Begin by writing the number of threads (always 1, for now). */
	kscm__fwrite_int(kscm, f, 1);
	kscm__fwrite_int(kscm, f, kscm->_threadstate);
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->_threadname));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->_threadopts));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->_threadobject));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->args));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->envir));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->code));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->dump));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->value));
	kscm__fwrite_int(kscm, f, kscm->_operator);

	//fprintf(stderr, "Persistent addresses: ->T=%d ->F=%d ->NIL=%d\n", kscm_get_persistent_address(kscm, kscm->T), kscm_get_persistent_address(kscm, kscm->F), kscm_get_persistent_address(kscm, kscm->NIL));

	//fprintf(stderr, "Persistent addresses: ->args=%d ->envir=%d ->code=%d ->dump=%d\n", kscm_get_persistent_address(kscm, kscm->args), kscm_get_persistent_address(kscm, kscm->envir), kscm_get_persistent_address(kscm, kscm->code), kscm_get_persistent_address(kscm, kscm->dump));

	int s;
	for (s = 0; s <= kscm->last_cell_seg; s++) {
		int i;
		for (i = 0; i < KSCM_CONFIG_CELL_SEGSIZE; i++) {
			kscm_object_t obj = &(kscm->cell_seg[s][i]);
			if (obj->_flag == 0) { // free ?
				//fprintf(stderr, "Object at %d:%d is free\n", s, i);
			}
			else {
				//fprintf(stderr, "Object at %d:%d is non-free\n", s, i);
				kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, obj));
				if (kscm__isnumber(kscm, obj)) {
					if (!kscm__isatom(kscm, obj)) {
						fprintf(stderr, "int isn't atom\n");
						exit(-1);
					}
					kscm__fwrite_byte(kscm, f, KSCM_PERSIST_TINT32);
					kscm__fwrite_int(kscm, f, kscm__ivalue(kscm, obj));
				}
				else if (kscm__issymbol(kscm, obj)) {
					if (kscm__isatom(kscm, obj)) {
						fprintf(stderr, "symbol is atom\n");
						exit(-1);
					}
					kscm__fwrite_byte(kscm, f, kscm__issyntax(kscm, obj) ? KSCM_PERSIST_TSYNTAX : KSCM_PERSIST_TSYMBOL);
					kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm__car(kscm, obj)));
					kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm__cdr(kscm, obj)));
				}
				else if (kscm__isstring(kscm, obj)) {
					if (!kscm__isatom(kscm, obj)) {
						fprintf(stderr, "string isn't atom\n");
						exit(-1);
					}
					kscm__fwrite_byte(kscm, f, KSCM_PERSIST_TSTRING);
					kscm__fwrite_int(kscm, f, obj->_object._string._keynum);
					kscm__fwrite_str(kscm, f, kscm__strvalue(kscm, obj));
				}
				else if (kscm__ispair(kscm, obj)) {
					if (kscm__isatom(kscm, obj)) {
						fprintf(stderr, "pair is atom\n");
						exit(-1);
					}
					kscm__fwrite_byte(kscm, f, KSCM_PERSIST_TPAIR);
					kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm__car(kscm, obj)));
					kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm__cdr(kscm, obj)));
				}
				else if (kscm__isproc(kscm, obj)) {
					if (!kscm__isatom(kscm, obj)) {
						fprintf(stderr, "proc isn't atom\n");
						exit(-1);
					}
					kscm__fwrite_byte(kscm, f, KSCM_PERSIST_TPROC);
					kscm__fwrite_int(kscm, f, kscm__ivalue(kscm, obj));
				}
				else if (kscm__isclosure(kscm, obj)) {
					if (kscm__isatom(kscm, obj)) {
						fprintf(stderr, "closure is atom\n");
						exit(-1);
					}
					kscm__fwrite_byte(kscm, f, KSCM_PERSIST_TCLOSURE);
					kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm__car(kscm, obj)));
					kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm__cdr(kscm, obj)));
				}
				else if (kscm__iscontinuation(kscm, obj)) {
					if (kscm__isatom(kscm, obj)) {
						fprintf(stderr, "closure is atom\n");
						exit(-1);
					}
					kscm__fwrite_byte(kscm, f, KSCM_PERSIST_TCONTINUATION);
					kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm__car(kscm, obj)));
					kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm__cdr(kscm, obj)));
				}
#ifdef KSCM_CONFIG_USE_STRUCTS
				else if (kscm__isabstraction(kscm, obj)) {
					if (kscm__isatom(kscm, obj)) {
						fprintf(stderr, "abstraction is atom\n");
						exit(-1);
					}
					kscm__fwrite_byte(kscm, f, KSCM_PERSIST_TABSTRACTION);
					kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm__car(kscm, obj)));
					kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm__cdr(kscm, obj)));
				}
				else if (kscm__isbuffer(kscm, obj)) {
					if (!kscm__isatom(kscm, obj)) {
						fprintf(stderr, "buffer isn't atom\n");
						exit(-1);
					}
					kscm__fwrite_byte(kscm, f, KSCM_PERSIST_TBUFFER);
					kscm__fwrite_strl(kscm, f, (const char*)obj->_object._buffer._data, obj->_object._buffer._length);
				}
#endif
#ifdef KSCM_CONFIG_USE_FLOATS
				else if (kscm__isfloat64(kscm, obj)) {
					char* bytes = (char*)(void*)&obj->_object._float64._dvalue;
					kscm__fwrite_byte(kscm, f, KSCM_PERSIST_TFLOAT64);
					// NOTE: This assumes floating-point endian is the same on all platforms
					kscm__fwrite_byte(kscm, f, bytes[0]);
					kscm__fwrite_byte(kscm, f, bytes[1]);
					kscm__fwrite_byte(kscm, f, bytes[2]);
					kscm__fwrite_byte(kscm, f, bytes[3]);
					kscm__fwrite_byte(kscm, f, bytes[4]);
					kscm__fwrite_byte(kscm, f, bytes[5]);
					kscm__fwrite_byte(kscm, f, bytes[6]);
					kscm__fwrite_byte(kscm, f, bytes[7]);
				}
#endif
#ifdef KSCM_CONFIG_USE_OBJECTS
				else if (kscm__isobjx(kscm, obj)) {
					if (kscm__isatom(kscm, obj)) {
						fprintf(stderr, "object is atom\n");
						exit(-1);
					}
					kscm__fwrite_byte(kscm, f, KSCM_PERSIST_TOBJX);
					kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, obj->_object._objx._type));
					kscm__fwrite_int(kscm, f, obj->_object._objx._count);
					int i;
					for (i = 0; i < obj->_object._objx._count; i++) {
						kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, obj->_object._objx._elements[i]));
					}
				}
#endif
				else {
					fprintf(stderr, "Object at %d:%d is non-free but unknown type: %d\n", s, i, obj->_flag);
					return -1; // Not saved
				}
			}
		}
	}

	kscm__fwrite_int(kscm, f, 0); // Zero address to indicate end of objects
	kscm__fwrite_int(kscm, f, 0); // Additional zero to indicate no additional data (may be extended in future versions)
	fclose(f);
	return 0; // Saved
}

int kscm__fread_byte(kscm_t* kscm, FILE* f, char* result) {
	int r = fgetc(f);
	*result = r;
	return 1;
}
int kscm__fread_int(kscm_t* kscm, FILE* f, int* result) {
	*result = 0;
	*result |= (fgetc(f) & 0xff);
	*result |= (fgetc(f) & 0xff) << 8;
	*result |= (fgetc(f) & 0xff) << 16;
	*result |= (fgetc(f) & 0xff) << 24;
	return 4;
}
int kscm__fread_str(kscm_t* kscm, FILE* f, const char** result) {
	int len;
	if (kscm__fread_int(kscm, f, &len) != 4) {
		*result = NULL;
		return 0;
	}
	//fprintf(stderr, "Got length %d\n", len);
	if (len > 1000) {
		*result = NULL;
		return 0;
	}
	*result = (const char*) calloc(len + 1, 1);
	if (*result == NULL) {
		return 0;
	}
	int i;
	for (i = 0; i < len; i++) {
		((* (char**)result)[i]) = (char)fgetc(f);
	}
	return len;
}

kscm_object_t kscm_get_object_address(kscm_t* kscm, int persistent_address) {
	if (persistent_address == 0) {
		return kscm->NIL;
	} else if (persistent_address == 1) {
		return kscm->F;
	} else if (persistent_address == 2) {
		return kscm->T;
	}
	int idx = persistent_address % KSCM_CONFIG_CELL_SEGSIZE;
	int segnum = persistent_address / KSCM_CONFIG_CELL_SEGSIZE;
	if (segnum < 1) {
		return NULL;
	}
	segnum--;
	if (segnum >= KSCM_CONFIG_CELL_NSEGMENT) {
		return NULL;
	}
	while (segnum > kscm->last_cell_seg) {
		kscm->last_cell_seg++;
		kscm->cell_seg[kscm->last_cell_seg] = (kscm_object_t) calloc(KSCM_CONFIG_CELL_SEGSIZE, sizeof(struct kscm_cell));
	}
	return &kscm->cell_seg[segnum][idx];
}

int kscm_resume_state(kscm_t* kscm, const char* filename, const char* opts) {
	if (filename == NULL || strlen(filename) < 1) {
		fprintf(stderr, "Filename expected\n");
		return -1;
	}
	FILE* f = fopen(filename, "rb");
	if (f == NULL) {
		fprintf(stderr, "Failed to open '%s' for reading\n", filename);
		return -1; // Not saved
	}
	fprintf(stderr, "Reading state from '%s' opts '%s'...\n", filename, opts);

	const char* tmpstr;
	int tmpint;
	kscm__fread_str(kscm, f, &tmpstr);

	if (tmpstr == NULL || strcmp(tmpstr, KSCM_CONFIG_PERSIST_MAGIC) != 0) {
		fprintf(stderr, "Failed to read '%s': Bad magic string\n", filename);
		free((void*)tmpstr);
		return -1;
	}
	free((void*)tmpstr);
	kscm__fread_int(kscm, f, &tmpint);
	if (tmpint != KSCM_CONFIG_PERSIST_VERSION) {
		fprintf(stderr, "Failed to read '%s': Bad version number, expected %d but got %d\n", filename, KSCM_CONFIG_PERSIST_VERSION, tmpint);
		return -1;
	}
	kscm__fread_int(kscm, f, &tmpint);
	if (tmpint != 4) {
		fprintf(stderr, "Failed to read '%s': Bad format options, expected 4 but got %d\n", filename, tmpint);
		return -1;
	}
	kscm->_stateformat = tmpint;
	kscm__fread_int(kscm, f, &tmpint);
	if (tmpint != 0) {
		fprintf(stderr, "Failed to read '%s': Bad NIL index, expected %d but got %d\n", filename, 0, tmpint);
		return -1;
	}
	kscm__fread_int(kscm, f, &tmpint);
	if (tmpint != 1) {
		fprintf(stderr, "Failed to read '%s': Bad F index, expected %d but got %d\n", filename, 1, tmpint);
		return -1;
	}
	kscm__fread_int(kscm, f, &tmpint);
	if (tmpint != 2) {
		fprintf(stderr, "Failed to read '%s': Bad T index, expected %d but got %d\n", filename, 2, tmpint);
		return -1;
	}

	kscm__fread_int(kscm, f, &tmpint);
	kscm->oblist = kscm_get_object_address(kscm, tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->global_env = kscm_get_object_address(kscm, tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->LAMBDA = kscm_get_object_address(kscm, tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->QUOTE = kscm_get_object_address(kscm, tmpint);

	kscm__fread_int(kscm, f, &tmpint);
	kscm->QQUOTE = kscm_get_object_address(kscm, tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->UNQUOTE = kscm_get_object_address(kscm, tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->UNQUOTESP = kscm_get_object_address(kscm, tmpint);

	kscm__fread_int(kscm, f, &tmpint);
	if (tmpint != 1) {
		fprintf(stderr, "Failed to read '%s': Bad number of threads, this VM only supports 1 thread but got %d\n", filename, tmpint);
		return -1;
	}
	/*
	
	kscm__fwrite_int(kscm, f, 1);
	kscm__fwrite_int(kscm, f, kscm->_threadstate);
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->_threadname));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->_threadopts));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->_threadobject));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->args));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->envir));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->code));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->dump));
	kscm__fwrite_int(kscm, f, kscm_get_persistent_address(kscm, kscm->value));
	kscm__fwrite_int(kscm, f, kscm->_operator);
	*/

	kscm__fread_int(kscm, f, &tmpint);
	kscm->_threadstate = tmpint;
	kscm__fread_int(kscm, f, &tmpint);
	kscm->_threadname = kscm_get_object_address(kscm, tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->_threadopts = kscm_get_object_address(kscm, tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->_threadobject = kscm_get_object_address(kscm, tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->args = kscm_get_object_address(kscm, tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->envir = kscm_get_object_address(kscm, tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->code = kscm_get_object_address(kscm, tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->dump = kscm_get_object_address(kscm, tmpint);
	
	/* At least for the main thread, the value/operator options are discarded. */
	kscm__fread_int(kscm, f, &tmpint);
	kscm__fread_int(kscm, f, &tmpint);
	kscm->value = kscm->NIL; //kscm_get_object_address(kscm, tmpint);
	kscm->_operator = 0;

	//fprintf(stderr, "Persistent addresses: ->T=%d ->F=%d ->NIL=%d\n", kscm_get_persistent_address(kscm, kscm->T), kscm_get_persistent_address(kscm, kscm->F), kscm_get_persistent_address(kscm, kscm->NIL));

	//fprintf(stderr, "Persistent addresses: ->args=%d ->envir=%d ->code=%d ->dump=%d\n", kscm_get_persistent_address(kscm, kscm->args), kscm_get_persistent_address(kscm, kscm->envir), kscm_get_persistent_address(kscm, kscm->code), kscm_get_persistent_address(kscm, kscm->dump));

	/* Clear all of the cell memory. Any new blocks that get automatically allocated will be cleared upon allocation. */
	int i;
	for (i = 0; i <= kscm->last_cell_seg; i++) {
		memset(kscm->cell_seg[i], 0, sizeof(struct kscm_cell) * KSCM_CONFIG_CELL_SEGSIZE);
	}

	int objid;
	do {
		if (kscm__fread_int(kscm, f, &objid) != 4) {
			fprintf(stderr, "WTFERR1\n");
			exit(-1);
			return -1;
		}
		if (objid == 0) break;

		kscm_object_t obj = kscm_get_object_address(kscm, objid);
		if (obj == NULL) {
			fprintf(stderr, "WTFERR2\n");
			exit(-1);
			return -1;
		}
		char typ = 0;
		if (kscm__fread_byte(kscm, f, &typ) != 1) {
			fprintf(stderr, "WTFERR3\n");
			exit(-1);
			return -1;
		}
		//fprintf(stderr, "Got type %d\n", typ);
		const char* tmpstr;
		switch (typ) {
		case KSCM_PERSIST_TINT32:
			obj->_flag = KSCM_T_NUMBER | KSCM_T_ATOM;
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._number._ivalue = tmpint;
			break;
		case KSCM_PERSIST_TSTRING:
			obj->_flag = KSCM_T_STRING | KSCM_T_ATOM;
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._string._keynum = tmpint;
			tmpint = kscm__fread_str(kscm, f, &tmpstr);
			if (tmpint < 0) {
				fprintf(stderr, "WTFERR4\n");
				exit(-1);
				return -1;
			}
			//fprintf(stderr, "Got str len %d '%s'\n", tmpint, tmpstr);
			obj->_object._string._svalue = _strdup(tmpstr);//kscm__store_string(kscm, tmpstr);
			free((void*)tmpstr);
			break;
		case KSCM_PERSIST_TSYMBOL:
			obj->_flag = KSCM_T_SYMBOL;
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._car = kscm_get_object_address(kscm, tmpint);
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._cdr = kscm_get_object_address(kscm, tmpint);
			break;
		case KSCM_PERSIST_TPAIR:
			obj->_flag = KSCM_T_PAIR;
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._car = kscm_get_object_address(kscm, tmpint);
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._cdr = kscm_get_object_address(kscm, tmpint);
			break;
		case KSCM_PERSIST_TPROC:
			obj->_flag = KSCM_T_PROC | KSCM_T_ATOM;
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._number._ivalue = tmpint;
			break;
		case KSCM_PERSIST_TCLOSURE:
			obj->_flag = KSCM_T_CLOSURE;
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._car = kscm_get_object_address(kscm, tmpint);
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._cdr = kscm_get_object_address(kscm, tmpint);
			break;
		case KSCM_PERSIST_TSYNTAX:
			obj->_flag = KSCM_T_SYMBOL | KSCM_T_SYNTAX;
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._car = kscm_get_object_address(kscm, tmpint);
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._cdr = kscm_get_object_address(kscm, tmpint);
			break;
		case KSCM_PERSIST_TCONTINUATION:
			obj->_flag = KSCM_T_CONTINUATION;
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._car = kscm_get_object_address(kscm, tmpint);
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._cdr = kscm_get_object_address(kscm, tmpint);
			break;
#ifdef KSCM_CONFIG_USE_STRUCTS
		case KSCM_PERSIST_TABSTRACTION:
			obj->_flag = KSCM_T_ABSTRACTION;
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._car = kscm_get_object_address(kscm, tmpint);
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._cons._cdr = kscm_get_object_address(kscm, tmpint);
			break;
		case KSCM_PERSIST_TBUFFER:
			obj->_flag = KSCM_T_BUFFER | KSCM_T_ATOM;
			tmpint = kscm__fread_str(kscm, f, &tmpstr);
			obj->_object._buffer._length = tmpint;
			if (tmpint < 0) {
				fprintf(stderr, "WTFERR4\n");
				exit(-1);
				return -1;
			}
			//fprintf(stderr, "Got str len %d '%s'\n", tmpint, tmpstr);
			obj->_object._buffer._data = (unsigned char*) tmpstr; //kscm__store_string(kscm, tmpstr);
			//free((void*)tmpstr);
			break;
#endif
#ifdef KSCM_CONFIG_USE_FLOATS
		case KSCM_PERSIST_TFLOAT64: {
			obj->_flag = KSCM_T_FLOAT64 | KSCM_T_ATOM;
			char* bytes = (char*)(void*)&obj->_object._float64._dvalue;
			kscm__fread_byte(kscm, f, &bytes[0]);
			kscm__fread_byte(kscm, f, &bytes[1]);
			kscm__fread_byte(kscm, f, &bytes[2]);
			kscm__fread_byte(kscm, f, &bytes[3]);
			kscm__fread_byte(kscm, f, &bytes[4]);
			kscm__fread_byte(kscm, f, &bytes[5]);
			kscm__fread_byte(kscm, f, &bytes[6]);
			kscm__fread_byte(kscm, f, &bytes[7]);
		} break;
#endif
#ifdef KSCM_CONFIG_USE_STRUCTS
		case KSCM_PERSIST_TOBJX:
			obj->_flag = KSCM_T_OBJX;
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._objx._type = kscm_get_object_address(kscm, tmpint);
			kscm__fread_int(kscm, f, &tmpint);
			obj->_object._objx._count = tmpint;
			obj->_object._objx._elements = (kscm_object_t*) calloc(sizeof(kscm_object_t), obj->_object._objx._count);
			// TODO Check non-null (ideally check size is sane before attempting to allocate/fill)
			for (i = 0; i < obj->_object._objx._count; i++) {
				kscm__fread_int(kscm, f, &tmpint);
				obj->_object._objx._elements[i] = kscm_get_object_address(kscm, tmpint);
			}
			break;
#endif
		default:
			fprintf(stderr, "Unknown object type #%d\n", typ);
			exit(1);
			if (1) return 0;
		}
	} while (objid != 0);

	kscm->free_cell = kscm->NIL;
	kscm->fcells = 0;
	return 0; // Resumed
}
/* From ifdef KSCM_CONFIG_USE_PERSIST */
#endif

kscm_object_t kscm__opexe_7(kscm_t* kscm, register short op)
{
	register kscm_object_t x, y, z;
	char* str1;
	char* str2;
	char* str3;
	register long v;
	static long	w;

	switch (op) {
	case KSCM_OP_STRCAT:
		x = kscm__car(kscm, kscm->args);
		y = kscm__cadr(kscm, kscm->args);
		if (!kscm__isstring(kscm, x) || !kscm__isstring(kscm, y)) {
			kscm__s_retbool(kscm, 0);
		}
		str1 = kscm__strvalue(kscm, x);
		str2 = kscm__strvalue(kscm, y);
		str3 = (char*)calloc(strlen(str1) + strlen(str2) + 1, 1);
		if (str3 == NULL) {
			kscm__s_retbool(kscm, 0);
		}
		strcat(str3, str1);
		strcat(str3, str2);
		z = kscm__mk_string(kscm, str3);
		free(str3);
		kscm__s_return(kscm, z);
	case KSCM_OP_STRLEN:
		x = kscm__car(kscm, kscm->args);
		if (!kscm__isstring(kscm, x)) {
			kscm__s_retbool(kscm, 0);
		}
		str1 = kscm__strvalue(kscm, x);
		kscm__s_return(kscm, kscm__mk_number(kscm, strlen(str1)));
	case KSCM_OP_STRGET:
		x = kscm__car(kscm, kscm->args);
		y = kscm__cadr(kscm, kscm->args);
		if (!kscm__isstring(kscm, x) || !kscm__isnumber(kscm, y)) {
			kscm__s_retbool(kscm, 0);
		}
		str1 = kscm__strvalue(kscm, x);
		v = kscm__ivalue(kscm, y);
		if (v < 0 || v >= strlen(str1)) {
			kscm__s_retbool(kscm, 0);
		}
		kscm__s_return(kscm, kscm__mk_number(kscm, ((int)str1[v]) & 0xFF));
#ifdef KSCM_CONFIG_USE_PERSIST
	case KSCM_OP_SAVE_STATE:
		x = kscm__car(kscm, kscm->args);
		y = kscm__cadr(kscm, kscm->args);
		str1 = kscm__strvalue(kscm, x);
		str2 = kscm__strvalue(kscm, y);
		if (kscm_save_state(kscm, str1, str2) == 0) {
			kscm__s_return(kscm, kscm__mk_symbol(kscm, "saved"));
		} else {
			kscm__s_retbool(kscm, 0);
		}
	case KSCM_OP_RESUME_STATE:
		x = kscm__car(kscm, kscm->args);
		y = kscm__cadr(kscm, kscm->args);
		str1 = kscm__strvalue(kscm, x);
		str2 = kscm__strvalue(kscm, y);
		if (kscm_resume_state(kscm, str1, str2) == 0) {
			kscm__s_return(kscm, kscm__mk_symbol(kscm, "resumed"));
		} else {
			kscm__s_retbool(kscm, 0);
		}
#endif
#ifdef KSCM_CONFIG_USE_STRUCTS
	case KSCM_OP_BUFFER:
		kscm__s_retbool(kscm, kscm__isbuffer(kscm, kscm__car(kscm, kscm->args)));
	case KSCM_OP_BUFFER_NEW:
		x = kscm__car(kscm, kscm->args);
		if (kscm__isstring(kscm, x)) {
			const char* strval = kscm__strvalue(kscm, x);
			size_t slen = strlen(strval);
			y = kscm__mk_buffer(kscm, slen);
			size_t iter;
			for (iter = 0; iter < slen; iter++) {
				y->_object._buffer._data[iter] = strval[iter];
			}
			kscm__s_return(kscm, y);
		} else if (!kscm__isnumber(kscm, x)) {
			kscm__s_return(kscm, kscm->NIL);
		}
		kscm__s_return(kscm, kscm__mk_buffer(kscm, kscm__ivalue(kscm, x)));
	case KSCM_OP_BUFFER_LEN:
		x = kscm__car(kscm, kscm->args);
		if (!kscm__isbuffer(kscm, x)) {
			kscm__s_return(kscm, kscm->F);
		}
		kscm__s_return(kscm, kscm__mk_number(kscm, x->_object._buffer._length));
	case KSCM_OP_BUFFER_GET:
		x = kscm__car(kscm, kscm->args);
		y = kscm__cadr(kscm, kscm->args);
		if (!kscm__isbuffer(kscm, x) || !kscm__isnumber(kscm, y)) {
			kscm__s_return(kscm, kscm->F);
		}
		v = kscm__ivalue(kscm, y);
		if (v < 0 || v >= x->_object._buffer._length) {
			kscm__s_return(kscm, kscm->F);
		}
		kscm__s_return(kscm, kscm__mk_number(kscm, ((long)x->_object._buffer._data[v]) & 0xFF));
	case KSCM_OP_BUFFER_SET:
		x = kscm__car(kscm, kscm->args);
		y = kscm__cadr(kscm, kscm->args);
		z = kscm__caddr(kscm, kscm->args);
		if (!kscm__isbuffer(kscm, x) || !kscm__isnumber(kscm, y) || !kscm__isnumber(kscm, z)) {
			kscm__s_return(kscm, kscm->F);
		}
		v = kscm__ivalue(kscm, y);
		if (v < 0 || v >= x->_object._buffer._length) {
			kscm__s_return(kscm, kscm->F);
		}
		x->_object._buffer._data[v] = (unsigned char)kscm__ivalue(kscm, z);
		kscm__s_return(kscm, kscm->T);
	case KSCM_OP_BUFFER_LOAD:
		kscm__s_return(kscm, kscm__mk_string(kscm, "TODO"));
	case KSCM_OP_BUFFER_SAVE:
		kscm__s_return(kscm, kscm__mk_string(kscm, "TODO"));
	case KSCM_OP_ABSTRACTION:
		kscm__s_retbool(kscm, kscm__isabstraction(kscm, kscm__car(kscm, kscm->args)));
	case KSCM_OP_ABSTRACTION_NEW:
		x = kscm__car(kscm, kscm->args);
		y = kscm__cadr(kscm, kscm->args);
		kscm__s_return(kscm, kscm__mk_abstraction(kscm, x, y));
	case KSCM_OP_ABSTRACTION_TYPE:
		x = kscm__car(kscm, kscm->args);
#ifdef KSCM_CONFIG_USE_OBJECTS
		if (kscm__isobjx(kscm, x)) {
			kscm__s_return(kscm, x->_object._objx._type);
		}
#endif
		if (!kscm__isabstraction(kscm, x)) {
			kscm__s_retbool(kscm, 0);
		}
		kscm__s_return(kscm, kscm__car(kscm, x));
	case KSCM_OP_ABSTRACTION_VALUE:
		x = kscm__car(kscm, kscm->args);
		if (!kscm__isabstraction(kscm, x)) {
			kscm__s_retbool(kscm, 0);
		}
		kscm__s_return(kscm, kscm__cdr(kscm, x));
#endif
#ifdef KSCM_CONFIG_USE_OBJECTS
	case KSCM_OP_OBJECT:
		kscm__s_retbool(kscm, kscm__isobjx(kscm, kscm__car(kscm, kscm->args)));
	case KSCM_OP_OBJECT_NEW:
		x = kscm__car(kscm, kscm->args);
		y = kscm__cadr(kscm, kscm->args);
		if (!kscm__isnumber(kscm, y)) {
			kscm__s_return(kscm, kscm->F);
		}
		kscm__s_return(kscm, kscm__mk_objx(kscm, x, kscm__ivalue(kscm, y)));
	case KSCM_OP_OBJECT_LEN:
		x = kscm__car(kscm, kscm->args);
		if (!kscm__isobjx(kscm, x)) {
			kscm__s_return(kscm, kscm->F);
		}
		kscm__s_return(kscm, kscm__mk_number(kscm, x->_object._objx._count));
	case KSCM_OP_OBJECT_GET:
		x = kscm__car(kscm, kscm->args);
		y = kscm__cadr(kscm, kscm->args);
		if (!kscm__isobjx(kscm, x) || !kscm__isnumber(kscm, y)) {
			kscm__s_return(kscm, kscm->F);
		}
		v = kscm__ivalue(kscm, y);
		if (v < 0 || v >= x->_object._objx._count) {
			kscm__s_return(kscm, kscm->F);
		}
		kscm__s_return(kscm, x->_object._objx._elements[v]);
	case KSCM_OP_OBJECT_SET:
		x = kscm__car(kscm, kscm->args);
		y = kscm__cadr(kscm, kscm->args);
		z = kscm__caddr(kscm, kscm->args);
		if (!kscm__isobjx(kscm, x) || !kscm__isnumber(kscm, y)) {
			kscm__s_return(kscm, kscm->F);
		}
		v = kscm__ivalue(kscm, y);
		if (v < 0 || v >= x->_object._objx._count) {
			kscm__s_return(kscm, kscm->F);
		}
		x->_object._objx._elements[v] = z;
		kscm__s_return(kscm, kscm->T);
	case KSCM_OP_OBJECT_RETYPE:
		x = kscm__car(kscm, kscm->args);
		y = kscm__cadr(kscm, kscm->args);
		if (!kscm__isobjx(kscm, x)) {
			kscm__s_return(kscm, kscm->F);
		}
		x->_object._objx._type = y;
		kscm__s_return(kscm, kscm->T);
#endif
	case KSCM_OP_SYMBOL_TO_STRING:
#ifdef KSCM_CONFIG_USE_STRUCTS
		/* We handle buffer->string in the same function if structs are enabled. */
		if (kscm__isbuffer(kscm, kscm__car(kscm, kscm->args))) {
			x = kscm__car(kscm, kscm->args);
			char* tmp_buffer = (char*) calloc(x->_object._buffer._length, 1);
			if (tmp_buffer == NULL) {
				kscm__s_return(kscm, kscm->NIL);
			}
			size_t iter;
			for (iter = 0; iter < x->_object._buffer._length; iter++) {
				tmp_buffer[iter] = x->_object._buffer._data[iter];
			}
			/* Note: All the messaround above was only to keep the string-creation API consistent. 
			 * It would be easy to optimise the buffer->string case by creating the string object manually.
			 */
			y = kscm__mk_string(kscm, tmp_buffer);
			free(tmp_buffer);
			kscm__s_return(kscm, y);
		}
#endif
		if (kscm__issymbol(kscm, kscm__car(kscm, kscm->args))) {
			x = kscm__caar(kscm, kscm->args);
			kscm__s_return(kscm, x);
		}
		else {
			kscm__s_return(kscm, kscm->F);
		}
	default:
		sprintf(kscm->strbuff, "%d is illegal operator", kscm->_operator);
		kscm__error_0(kscm, kscm->strbuff);
	}
	return kscm->T;	/* NOTREACHED */
}

kscm_object_t(*kscm__shared_dispatch_table[])(kscm_t* kscm, register short op) = {
	&kscm__opexe_0,	/* KSCM_OP_LOAD = 0, */
	&kscm__opexe_0,	/* KSCM_OP_T0LVL, */
	&kscm__opexe_0,	/* KSCM_OP_T1LVL, */
	&kscm__opexe_0,	/* KSCM_OP_READ, */
	&kscm__opexe_0,	/* KSCM_OP_VALUEPRINT, */
	&kscm__opexe_0,	/* KSCM_OP_EVAL, */
	&kscm__opexe_0,	/* KSCM_OP_E0ARGS, */
	&kscm__opexe_0,	/* KSCM_OP_E1ARGS, */
	&kscm__opexe_0,	/* KSCM_OP_APPLY, */
	&kscm__opexe_0,	/* KSCM_OP_DOMACRO, */

	&kscm__opexe_0,	/* KSCM_OP_LAMBDA, */
	&kscm__opexe_0,	/* KSCM_OP_QUOTE, */
	&kscm__opexe_0,	/* KSCM_OP_DEF0, */
	&kscm__opexe_0,	/* KSCM_OP_DEF1, */
	&kscm__opexe_0,	/* KSCM_OP_BEGIN, */
	&kscm__opexe_0,	/* KSCM_OP_IF0, */
	&kscm__opexe_0,	/* KSCM_OP_IF1, */
	&kscm__opexe_0,	/* KSCM_OP_SET0, */
	&kscm__opexe_0,	/* KSCM_OP_SET1, */
	&kscm__opexe_0,	/* KSCM_OP_LET0, */
	&kscm__opexe_0,	/* KSCM_OP_LET1, */
	&kscm__opexe_0,	/* KSCM_OP_LET2, */
	&kscm__opexe_0,	/* KSCM_OP_LET0AST, */
	&kscm__opexe_0,	/* KSCM_OP_LET1AST, */
	&kscm__opexe_0,	/* KSCM_OP_LET2AST, */

	&kscm__opexe_1,	/* KSCM_OP_LET0REC, */
	&kscm__opexe_1,	/* KSCM_OP_LET1REC, */
	&kscm__opexe_1,	/* OP_LETREC2, */
	&kscm__opexe_1,	/* KSCM_OP_COND0, */
	&kscm__opexe_1,	/* KSCM_OP_COND1, */
	&kscm__opexe_1,	/* KSCM_OP_DELAY, */
	&kscm__opexe_1,	/* KSCM_OP_AND0, */
	&kscm__opexe_1,	/* KSCM_OP_AND1, */
	&kscm__opexe_1,	/* KSCM_OP_OR0, */
	&kscm__opexe_1,	/* KSCM_OP_OR1, */
	&kscm__opexe_1,	/* KSCM_OP_C0STREAM, */
	&kscm__opexe_1,	/* KSCM_OP_C1STREAM, */
	&kscm__opexe_1,	/* KSCM_OP_0MACRO, */
	&kscm__opexe_1,	/* KSCM_OP_1MACRO, */
	&kscm__opexe_1,	/* KSCM_OP_CASE0, */
	&kscm__opexe_1,	/* KSCM_OP_CASE1, */
	&kscm__opexe_1,	/* KSCM_OP_CASE2, */

	&kscm__opexe_1,	/* KSCM_OP_PEVAL, */
	&kscm__opexe_1,	/* KSCM_OP_PAPPLY, */
	&kscm__opexe_1,	/* KSCM_OP_CONTINUATION, */

	&kscm__opexe_2,	/* KSCM_OP_ADD, */
	&kscm__opexe_2,	/* KSCM_OP_SUB, */
	&kscm__opexe_2,	/* KSCM_OP_MUL, */
	&kscm__opexe_2,	/* KSCM_OP_DIV, */
	&kscm__opexe_2,	/* KSCM_OP_REM, */
	&kscm__opexe_2,	/* KSCM_OP_CAR, */
	&kscm__opexe_2,	/* KSCM_OP_CDR, */
	&kscm__opexe_2,	/* KSCM_OP_CONS, */
	&kscm__opexe_2,	/* KSCM_OP_SETCAR, */
	&kscm__opexe_2,	/* KSCM_OP_SETCDR, */

	&kscm__opexe_3,	/* KSCM_OP_NOT, */
	&kscm__opexe_3,	/* KSCM_OP_BOOL, */
	&kscm__opexe_3,	/* KSCM_OP_NULL, */
	&kscm__opexe_3,	/* KSCM_OP_ZEROP, */
	&kscm__opexe_3,	/* KSCM_OP_POSP, */
	&kscm__opexe_3,	/* KSCM_OP_NEGP, */
	&kscm__opexe_3,	/* KSCM_OP_NEQ, */
	&kscm__opexe_3,	/* KSCM_OP_LESS, */
	&kscm__opexe_3,	/* KSCM_OP_GRE, */
	&kscm__opexe_3,	/* KSCM_OP_LEQ, */
	&kscm__opexe_3,	/* KSCM_OP_GEQ, */
	&kscm__opexe_3,	/* KSCM_OP_SYMBOL, */
	&kscm__opexe_3,	/* KSCM_OP_NUMBER, */
	&kscm__opexe_3,	/* KSCM_OP_STRING, */
	&kscm__opexe_3,	/* KSCM_OP_PROC, */
	&kscm__opexe_3,	/* KSCM_OP_PAIR, */
	&kscm__opexe_3,	/* KSCM_OP_EQ, */
	&kscm__opexe_3,	/* KSCM_OP_EQV, */

	&kscm__opexe_4,	/* KSCM_OP_FORCE, */
	&kscm__opexe_4,	/* KSCM_OP_WRITE, */
	&kscm__opexe_4,	/* KSCM_OP_DISPLAY, */
	&kscm__opexe_4,	/* KSCM_OP_NEWLINE, */
	&kscm__opexe_4,	/* KSCM_OP_ERR0, */
	&kscm__opexe_4,	/* KSCM_OP_ERR1, */
	&kscm__opexe_4,	/* KSCM_OP_REVERSE, */
	&kscm__opexe_4,	/* KSCM_OP_APPEND, */
	&kscm__opexe_4,	/* KSCM_OP_PUT, */
	&kscm__opexe_4,	/* KSCM_OP_GET, */
	&kscm__opexe_4,	/* KSCM_OP_QUIT, */
	&kscm__opexe_4,	/* KSCM_OP_GC, */
	&kscm__opexe_4,	/* KSCM_OP_GCVERB, */
	&kscm__opexe_4,	/* KSCM_OP_NEWSEGMENT, */

	&kscm__opexe_5,	/* KSCM_OP_RDSEXPR, */
	&kscm__opexe_5,	/* KSCM_OP_RDLIST, */
	&kscm__opexe_5,	/* KSCM_OP_RDDOT, */
	&kscm__opexe_5,	/* KSCM_OP_RDQUOTE, */
	&kscm__opexe_5,	/* KSCM_OP_RDQQUOTE, */
	&kscm__opexe_5,	/* KSCM_OP_RDUNQUOTE, */
	&kscm__opexe_5,	/* KSCM_OP_RDUQTSP, */
	&kscm__opexe_5,	/* KSCM_OP_P0LIST, */
	&kscm__opexe_5,	/* KSCM_OP_P1LIST, */

	&kscm__opexe_6,	/* KSCM_OP_LIST_LENGTH, */
	&kscm__opexe_6,	/* KSCM_OP_ASSQ, */
	&kscm__opexe_6,	/* KSCM_OP_PRINT_WIDTH, */
	&kscm__opexe_6,	/* KSCM_OP_P0_WIDTH, */
	&kscm__opexe_6,	/* KSCM_OP_P1_WIDTH, */
	&kscm__opexe_6,	/* KSCM_OP_GET_CLOSURE, */
	&kscm__opexe_6,	/* KSCM_OP_CLOSUREP, */
//#ifdef KSCM_CONFIG_USE_MACRO
	&kscm__opexe_6,	/* KSCM_OP_MACROP, */
//#endif Removed ifdef to keep ordering consistent. -Zak.
	&kscm__opexe_7, /* KSCM_OP_STRCAT, */
	&kscm__opexe_7, /* KSCM_OP_STRLEN, */
	&kscm__opexe_7, /* KSCM_OP_STRGET, */
	&kscm__opexe_7, /* KSCM_OP_SAVE_STATE, */
	&kscm__opexe_7, /* KSCM_OP_RESUME_STATE, */
	&kscm__opexe_7, /* KSCM_OP_BUFFER, */
	&kscm__opexe_7, /* KSCM_OP_BUFFER_NEW, */
	&kscm__opexe_7, /* KSCM_OP_BUFFER_LEN, */
	&kscm__opexe_7, /* KSCM_OP_BUFFER_GET, */
	&kscm__opexe_7, /* KSCM_OP_BUFFER_SET, */
	&kscm__opexe_7, /* KSCM_OP_ABSTRACTION, */
	&kscm__opexe_7, /* KSCM_OP_ABSTRACTION_NEW, */
	&kscm__opexe_7, /* KSCM_OP_ABSTRACTION_LEN, */
	&kscm__opexe_7, /* KSCM_OP_ABSTRACTION_GET, */
	&kscm__opexe_7, /* KSCM_OP_OBJECT */
	&kscm__opexe_7, /* KSCM_OP_OBJECT_NEW */
	&kscm__opexe_7, /* KSCM_OP_OBJECT_LEN */
	&kscm__opexe_7, /* KSCM_OP_OBJECT_GET */
	&kscm__opexe_7, /* KSCM_OP_OBJECT_SET */
	&kscm__opexe_7, /* KSCM_OP_OBJECT_RETYPE */
	&kscm__opexe_7, /* KSCM_OP_SYMBOL_TO_STRING */
	&kscm__opexe_7, /* KSCM_OP_BUFFER_LOAD */
	&kscm__opexe_7, /* KSCM_OP_BUFFER_SAVE */
};

/* These and the commented-out parts of kscm__eval_cycle can be re-enabled if you need to make sure the interpreter is running.
int fixme_reccheck = 0;
int fixme_ops = 0;
*/
/* kernel of this intepreter */
kscm_object_t kscm__eval_cycle(kscm_t* kscm, register short op)
{
	/*fixme_reccheck++;
	if (fixme_reccheck > 1) {
		fprintf(stderr, "Warning reccheck=%d\n", fixme_reccheck);
	}*/
	kscm->_operator = op;
	for (;;) {
		if ((*kscm__shared_dispatch_table[kscm->_operator])(kscm, kscm->_operator) == kscm->NIL) {
			//fixme_reccheck--;
			return kscm->NIL;
		}
		/*if ((fixme_ops % 1000) == 0) {
			fprintf(stderr, "Just did op %d\n", fixme_ops);
		}
		fixme_ops++;*/
	}
}

/* ========== Initialization of internal keywords ========== */

void kscm__mk_syntax(kscm_t* kscm, unsigned short op, const char *name)
{
	kscm_object_t x;

	x = kscm__cons(kscm, kscm__mk_string(kscm, name), kscm->NIL);
	kscm__type(kscm, x) = (KSCM_T_SYNTAX | KSCM_T_SYMBOL);
	kscm__syntaxnum(kscm, x) = op;
	kscm->oblist = kscm__cons(kscm, x, kscm->oblist);
}

void kscm__mk_proc(kscm_t* kscm, unsigned short op, const char *name)
{
	kscm_object_t x, y;

	x = kscm__mk_symbol(kscm, name);
	y = kscm__get_cell(kscm, kscm->NIL, kscm->NIL);
	kscm__type(kscm, y) = (KSCM_T_PROC | KSCM_T_ATOM);
	kscm__ivalue(kscm, y) = (long)op;
	kscm__car(kscm, kscm->global_env) = kscm__cons(kscm, kscm__cons(kscm, x, y), kscm__car(kscm, kscm->global_env));
}


void kscm__init_vars_global(kscm_t* kscm)
{
	kscm_object_t x;

	/* init input/output file */
	kscm->inputs[kscm->inputtop] = stdin;
	kscm->outfp = stdout;
	/* init kscm->NIL */
	kscm__type(kscm, kscm->NIL) = (KSCM_T_ATOM | KSCM_MARK);
	kscm__car(kscm, kscm->NIL) = kscm__cdr(kscm, kscm->NIL) = kscm->NIL;
	/* init T */
	kscm__type(kscm, kscm->T) = (KSCM_T_ATOM | KSCM_MARK);
	kscm__car(kscm, kscm->T) = kscm__cdr(kscm, kscm->T) = kscm->T;
	/* init F */
	kscm__type(kscm, kscm->F) = (KSCM_T_ATOM | KSCM_MARK);
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
	kscm__mk_syntax(kscm, KSCM_OP_LAMBDA, "lambda");
	kscm__mk_syntax(kscm, KSCM_OP_QUOTE, "quote");
	kscm__mk_syntax(kscm, KSCM_OP_DEF0, "define");
	kscm__mk_syntax(kscm, KSCM_OP_IF0, "if");
	kscm__mk_syntax(kscm, KSCM_OP_BEGIN, "begin");
	kscm__mk_syntax(kscm, KSCM_OP_SET0, "set!");
	kscm__mk_syntax(kscm, KSCM_OP_LET0, "let");
	kscm__mk_syntax(kscm, KSCM_OP_LET0AST, "let*");
	kscm__mk_syntax(kscm, KSCM_OP_LET0REC, "letrec");
	kscm__mk_syntax(kscm, KSCM_OP_COND0, "cond");
	kscm__mk_syntax(kscm, KSCM_OP_DELAY, "delay");
	kscm__mk_syntax(kscm, KSCM_OP_AND0, "and");
	kscm__mk_syntax(kscm, KSCM_OP_OR0, "or");
	kscm__mk_syntax(kscm, KSCM_OP_C0STREAM, "cons-stream");
#ifdef KSCM_CONFIG_USE_MACRO
	kscm__mk_syntax(kscm, KSCM_OP_0MACRO, "macro");
#endif
	kscm__mk_syntax(kscm, KSCM_OP_CASE0, "case");
}


void kscm__init_procs(kscm_t* kscm)
{
	/* init procedure */
	kscm__mk_proc(kscm, KSCM_OP_PEVAL, "eval");
	kscm__mk_proc(kscm, KSCM_OP_PAPPLY, "apply");
	kscm__mk_proc(kscm, KSCM_OP_CONTINUATION, "call-with-current-continuation");
	kscm__mk_proc(kscm, KSCM_OP_FORCE, "force");
	kscm__mk_proc(kscm, KSCM_OP_CAR, "car");
	kscm__mk_proc(kscm, KSCM_OP_CDR, "cdr");
	kscm__mk_proc(kscm, KSCM_OP_CONS, "cons");
	kscm__mk_proc(kscm, KSCM_OP_SETCAR, "set-car!");
	kscm__mk_proc(kscm, KSCM_OP_SETCDR, "set-cdr!");
	kscm__mk_proc(kscm, KSCM_OP_ADD, "+");
	kscm__mk_proc(kscm, KSCM_OP_SUB, "-");
	kscm__mk_proc(kscm, KSCM_OP_MUL, "*");
	kscm__mk_proc(kscm, KSCM_OP_DIV, "/");
	kscm__mk_proc(kscm, KSCM_OP_REM, "remainder");
	kscm__mk_proc(kscm, KSCM_OP_NOT, "not");
	kscm__mk_proc(kscm, KSCM_OP_BOOL, "boolean?");
	kscm__mk_proc(kscm, KSCM_OP_SYMBOL, "symbol?");
	kscm__mk_proc(kscm, KSCM_OP_NUMBER, "number?");
	kscm__mk_proc(kscm, KSCM_OP_STRING, "string?");
	kscm__mk_proc(kscm, KSCM_OP_PROC, "procedure?");
	kscm__mk_proc(kscm, KSCM_OP_PAIR, "pair?");
	kscm__mk_proc(kscm, KSCM_OP_EQV, "eqv?");
	kscm__mk_proc(kscm, KSCM_OP_EQ, "eq?");
	kscm__mk_proc(kscm, KSCM_OP_NULL, "null?");
	kscm__mk_proc(kscm, KSCM_OP_ZEROP, "zero?");
	kscm__mk_proc(kscm, KSCM_OP_POSP, "positive?");
	kscm__mk_proc(kscm, KSCM_OP_NEGP, "negative?");
	kscm__mk_proc(kscm, KSCM_OP_NEQ, "=");
	kscm__mk_proc(kscm, KSCM_OP_LESS, "<");
	kscm__mk_proc(kscm, KSCM_OP_GRE, ">");
	kscm__mk_proc(kscm, KSCM_OP_LEQ, "<=");
	kscm__mk_proc(kscm, KSCM_OP_GEQ, ">=");
	kscm__mk_proc(kscm, KSCM_OP_READ, "read");
	kscm__mk_proc(kscm, KSCM_OP_WRITE, "write");
	kscm__mk_proc(kscm, KSCM_OP_DISPLAY, "display");
	kscm__mk_proc(kscm, KSCM_OP_NEWLINE, "newline");
	kscm__mk_proc(kscm, KSCM_OP_LOAD, "load");
	kscm__mk_proc(kscm, KSCM_OP_ERR0, "error");
	kscm__mk_proc(kscm, KSCM_OP_REVERSE, "reverse");
	kscm__mk_proc(kscm, KSCM_OP_APPEND, "append");
	kscm__mk_proc(kscm, KSCM_OP_PUT, "put");
	kscm__mk_proc(kscm, KSCM_OP_GET, "get");
	kscm__mk_proc(kscm, KSCM_OP_GC, "gc");
	kscm__mk_proc(kscm, KSCM_OP_GCVERB, "gc-verbose");
	kscm__mk_proc(kscm, KSCM_OP_NEWSEGMENT, "new-segment");
	kscm__mk_proc(kscm, KSCM_OP_LIST_LENGTH, "list-length");	/* a.k */
	kscm__mk_proc(kscm, KSCM_OP_ASSQ, "assq");	/* a.k */
	kscm__mk_proc(kscm, KSCM_OP_PRINT_WIDTH, "print-width");	/* a.k */
	kscm__mk_proc(kscm, KSCM_OP_GET_CLOSURE, "get-closure-code");	/* a.k */
	kscm__mk_proc(kscm, KSCM_OP_CLOSUREP, "closure?");	/* a.k */
#ifdef KSCM_CONFIG_USE_MACRO
	kscm__mk_proc(kscm, KSCM_OP_MACROP, "macro?");	/* a.k */
#endif
	kscm__mk_proc(kscm, KSCM_OP_STRCAT, "string-cat");
	kscm__mk_proc(kscm, KSCM_OP_STRLEN, "string-length");
	kscm__mk_proc(kscm, KSCM_OP_STRGET, "string-get");
#ifdef KSCM_CONFIG_USE_PERSIST
	kscm__mk_proc(kscm, KSCM_OP_SAVE_STATE, "save-state");
	kscm__mk_proc(kscm, KSCM_OP_RESUME_STATE, "resume-state");
#endif
#ifdef KSCM_CONFIG_USE_STRUCTS
	kscm__mk_proc(kscm, KSCM_OP_BUFFER, "buffer?");
	kscm__mk_proc(kscm, KSCM_OP_BUFFER_NEW, "buffer-new");
	kscm__mk_proc(kscm, KSCM_OP_BUFFER_LEN, "buffer-length");
	kscm__mk_proc(kscm, KSCM_OP_BUFFER_GET, "buffer-get");
	kscm__mk_proc(kscm, KSCM_OP_BUFFER_SET, "buffer-set!");
	kscm__mk_proc(kscm, KSCM_OP_ABSTRACTION, "abstraction?");
	kscm__mk_proc(kscm, KSCM_OP_ABSTRACTION_NEW, "abstraction-new");
	kscm__mk_proc(kscm, KSCM_OP_ABSTRACTION_TYPE, "abstraction-type");
	kscm__mk_proc(kscm, KSCM_OP_ABSTRACTION_VALUE, "abstraction-value");
	kscm__mk_proc(kscm, KSCM_OP_BUFFER_LOAD, "buffer-load");
	kscm__mk_proc(kscm, KSCM_OP_BUFFER_SAVE, "buffer-save");
#endif
#ifdef KSCM_CONFIG_USE_OBJECTS
	kscm__mk_proc(kscm, KSCM_OP_OBJECT, "object?");
	kscm__mk_proc(kscm, KSCM_OP_OBJECT_NEW, "object-new");
	kscm__mk_proc(kscm, KSCM_OP_OBJECT_LEN, "object-length");
	kscm__mk_proc(kscm, KSCM_OP_OBJECT_GET, "object-get");
	kscm__mk_proc(kscm, KSCM_OP_OBJECT_SET, "object-set!");
	kscm__mk_proc(kscm, KSCM_OP_OBJECT_RETYPE, "object-retype!");
	/* NOTE: There is no object-type function, the abstraction-type function handles all custom-typed values. */
#endif
	kscm__mk_proc(kscm, KSCM_OP_SYMBOL_TO_STRING, "symbol->string");
	kscm__mk_proc(kscm, KSCM_OP_QUIT, "quit");
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
	kscm__resetinput(kscm);
	longjmp(kscm->error_jmp, KSCM_OP_T0LVL);
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
	short   op = (short)KSCM_OP_LOAD;

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