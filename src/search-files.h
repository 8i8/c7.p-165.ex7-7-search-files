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

#define MAXFILES	1000
#define MAXLEN		1000			/* Max length of input line */
#define MAXLINES	5000			/* Lines to be sorted */
#define ALLOCSIZE	5000			/* Size of available space */
#define MAXTOKEN	6			/* Maximum number of tokens */
#define TOKENSIZE	24			/* Size of space for input tokens */

typedef short bool;
enum boolean { false, true };
enum function { simple, alpha, fold, nosort };

/* Function pointers */
typedef int (*comp)(const void *, const void *);
extern comp strsimp;
extern comp stnsort;
extern comp strfold;

typedef short int bool;

/* Global flags */
struct State {
	unsigned int func;			/* Define which function to use */
	unsigned int numeric	: 1;		/* use numeric sort in qsort */
	unsigned int reverse	: 1;		/* reverse search order */
	unsigned int remempty	: 1;
	unsigned int directory	: 1;
	unsigned int rsort	: 1;
	unsigned int indx	: 1;
	unsigned int linenum	: 1;
};

struct F_name {
	unsigned char *name;
};

/* Global data struct, to store each file */
struct Line {
	unsigned char *line;
	struct F_name *name;
	struct Line *next;
	size_t len;
	size_t num;
	size_t id;
	unsigned int isTrue : 1;
};

struct File {
	struct Line *lines;
	struct F_name f_name;
	unsigned char *str;
	unsigned int flag : 1;
	size_t f_lines;			/* Character count for entire file */
	size_t f_len;
};

struct Folio {
	struct File *files;
	unsigned char *memory;		/* Pointer to the text block */
	struct Line *linesArray;	/* Pointer to the Linres block */
	size_t t_files;
	size_t t_lines;
	size_t t_len;
	size_t ntab;
};

extern struct State state;
extern struct Folio folio;
extern struct Line *lineptr[];
extern size_t pt;

/* Main */
void settings(int argc, char*argv[]);
void resetglobals(void);

/* i/o */
void getflags(int argc, char *argv[]);
void getinput(struct Folio *folio, int argc, char *argv[]);
void loadfolio(struct Folio *folio);
size_t readlines(unsigned char *lineptr[], size_t maxlines);
void printhash(struct Line lineptr[], size_t len);
size_t deleteline(unsigned char *lineptr[], int line, size_t nlines);
void settabs(char n[]);
size_t insertline(unsigned char *lineptr[], unsigned char *line, size_t maxlines, size_t index, size_t nlines);
void printfolio(struct Folio folio);
void freeall(struct Folio *folio);

/* Hash table */
void hashtable(struct Folio *folio);

/* Sort */
void sortsection(void *lines, int nel, int width, int func);
void _qsort(void *base, size_t nel, size_t width, comp fn);
size_t sortdivide(unsigned char *lineptr[], int func, size_t nlines, size_t ntab);
size_t addspacer(unsigned char *lineptr[], size_t maxlines, size_t nlines, size_t ntab);

