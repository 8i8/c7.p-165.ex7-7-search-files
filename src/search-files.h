/*
 * Exercise 7-7. Modify the pattern finding program of Chapter 5 to take its
 * input from a set of named files or, if no files are named as arguments, from
 * the standard input. Should the file name be printed when a matching line is
 * found?
 */

#include <stdio.h>
#include <stdlib.h>

/* Redefine getline */
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L

//#define MAXFILES	FOPEN_MAX
#define MAXFILES	10000
#define MAXLEN		1000			/* Max length of input line */
#define MAXLINES	5000			/* Lines to be sorted */
#define ALLOCSIZE	5000			/* Size of available space */
#define MAXTOKEN	6			/* Maximum number of tokens */
#define TOKENSIZE	24			/* Size of space for input tokens */

typedef short bool;
enum boolean { false, true };
enum function { simple, alpha, fold, nosort };

/* Function pointers */
typedef int (*compar)(void *, void *);		/* Sort functions for qsort */
extern compar strsimp;
extern compar stnsort;
extern compar strfold;

typedef short int bool;

/* Global flags */
typedef struct {
	bool numeric;				/* use numeric sort in qsort */
	bool reverse;				/* reverse search order */
	bool remempty;
	bool directory;
	bool rsort;
	bool indx;
	bool linenum;
	int  func;				/* Define which function to use */
} State;

/* Global data struct, to store each file */
struct line {
	struct line *next;
	unsigned char *line;
	size_t len;
	short isTrue;
};

typedef struct line Line;

struct file {
	unsigned char *str;
	Line **lines;
	unsigned char *name;
	size_t count;
	size_t len;
	short file;
};

typedef struct file File;

typedef struct {
	File files[MAXFILES];
	unsigned char *memory;
	size_t file_t;
	size_t line_t;
	size_t len_t;
} Folio;

extern State state;
extern Folio folio;
extern unsigned char *lineptr[];
extern size_t pt;

/* Main */
size_t settings(int argc, char*argv[]);
void inputargs(int argc, char*argv[]);
void sortsection(void *lineptr[], int left, int right, int func, int ntab);
void resetglobals(void);

/* i/o */
void getflags(int argc, char*argv[]);
void getinput(char* const argument, size_t const file);
Folio loadfolio(Folio folio);
size_t readlines(unsigned char *lineptr[], size_t maxlines);
void printhash(unsigned char **lines, size_t lp);
size_t deleteline(unsigned char *lineptr[], int line, size_t nlines);
void settabs(char n[]);
size_t insertline(unsigned char *lineptr[], unsigned char *line, size_t maxlines, size_t index, size_t nlines);
void printfolio(Folio folio);

/* Hash table */
Folio hashtable(Folio folio);

/* Sort */
void _qsort(void *lineptr[], int left, int right, compar fn, int ntab);
size_t sortdivide(unsigned char *lineptr[], int func, size_t nlines, int ntab);
size_t addspacer(unsigned char *lineptr[], size_t maxlines, size_t nlines, int ntab);

