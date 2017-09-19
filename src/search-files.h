/*
 * Exercise 7-7. Modify the pattern finding program of Chapter 5 to take its
 * input from a set of named files or, if no files are named as arguments, from
 * the standard input. Should the file name be printed when a matching line is
 * found?
 */

#include <stdio.h>
#include <stdlib.h>
#include "structs.c"

/* Redefine getline */
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L

#define MAXLEN		1000			/* Max length of input line */
#define MAXLINES	5000			/* Lines to be sorted */

typedef short bool;

enum boolean { false, true };
enum function { simple, alpha, fold, nosort };
enum filetype { type_string, type_file };

/* Function pointers */
typedef int (*comp)(const void *, const void *);
extern comp strsimp;
extern comp stnsort;
extern comp strfold;

typedef short int bool;

extern struct State state;

/* i/o */
void getflags(int argc, char *argv[]);
void getinput(struct Folio *folio, int argc, char *argv[]);
void loadfolio(struct Folio *folio);
void printhash(struct Sort *s);
void settabs(char n[]);
size_t insertline(unsigned char *lineptr[], unsigned char *line, size_t maxlines, size_t index, size_t nlines);
void printfolio(struct Folio *folio);
void free_folio(struct Folio *fx1);
void free_sort(struct Sort *s);

/* Hash table */
struct Sort *hashtable(struct Folio *folio, struct Sort *s);

/* Sort */
void sortsection(void *lines, int nel, size_t width, int func);
void _qsort(void *base, size_t nel, size_t width, int (*fn)(const void*, const void*));
size_t sortdivide(unsigned char *lineptr[], int func, size_t nlines, size_t ntab);
size_t addspacer(unsigned char *lineptr[], size_t maxlines, size_t nlines, size_t ntab);

/* init */
void resetglobals(void);
struct Folio *init_folio(struct Folio *folio);
struct Sort *init_sort(struct Sort *sort);
struct File init_file(struct File *file, struct Folio *root);
struct Line init_line(struct Line *line, size_t id);
