/*
 * Exercise 7-7. Modify the pattern finding program of Chapter 5 to take its
 * input from a set of named files or, if no files are named as arguments, from
 * the standard input. Should the file name be printed when a matching line is
 * found?
 */

/* Redefine getline */
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>

#define MAXFILES	FOPEN_MAX
#define MAXLEN		1000			/* Max length of input line */
#define MAXLINES	5000			/* Lines to be sorted */
#define ALLOCSIZE	5000			/* Size of available space */
#define MAXTOKEN	6			/* Maximum number of tokens */
#define TOKENSIZE	24			/* Size of space for input tokens */

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
	struct line *h_next;
	char *line;
	size_t len;
	short isTrue;
};

typedef struct line Line;

struct file {
	char *str;
	Line *lines;
	char *name;
	size_t count;
	size_t len;
	short file;
};

typedef struct file File;

typedef struct {
	File files[MAXFILES];
	char *memory;
	size_t count;
	size_t len;
} Folio;

extern State state;
extern Folio folio;
extern Line *lineptr[];
extern size_t pt;

/* Main */
size_t settings(int argc, char*argv[]);
void inputargs(int argc, char*argv[]);
void sortsection(char *lineptr[], int left, int right, int func, int ntab);
void resetglobals(void);

/* i/o */
void getflags(int argc, char*argv[]);
void getinput(char* const argument, size_t const file);
Folio loadfolio(Folio folio);
size_t readlines(char *lineptr[], size_t maxlines);
void writelines(Line **lines, size_t lp);
size_t deleteline(char *lineptr[], int line, size_t nlines);
void settabs(char n[]);
size_t insertline(char *lineptr[], char* line, size_t maxlines, size_t index, size_t nlines);
void printtest(Folio folio);

/* Hash table */
Folio hashtable(Folio folio);

/* Sort */
void _qsort(void *lineptr[], int left, int right, compar fn, int ntab);
size_t sortdivide(char *lineptr[], int func, size_t nlines, int ntab);
size_t addspacer(char *lineptr[], size_t maxlines, size_t nlines, int ntab);

