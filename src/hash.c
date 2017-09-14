/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Hash table
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "search-files.h"
#include <string.h>

#define P1 		317		/* prime smaller than s^16/200 */
#define P2 		65521		/* prime smaller than 2^16 */
#define P3		281539415969051	/* prime smaller than 2^64/65521 */

static struct Line *hashtab[P2];
struct Line *lineptr[100];

/*
 * hash:	form hash value for string s
 */
static unsigned long hash(unsigned char *s)
{
	unsigned long hashval;

	for (hashval = 0; *s != '\0'; s++) {
		hashval = *s + P1 * hashval;
		/* A prime is used for a modulus to avoid wrapping, although
		 * this is not a problem for unsigned int, it is I think more
		 * likely to form a patten if it is not prime. */
		hashval %= P3;
	}
	return hashval % P2;
}

/*
 * lookup:	Look for s in hashtab
 */
static struct Line *lookup(unsigned char *s)
{
	struct Line *ln;
	ln = NULL;

	for (ln = hashtab[hash(s)]; ln != NULL; ln = ln->next)
		if (strcmp((char*)s, (char*)ln->line) == 0)
			return ln;

	return NULL;
}

/*
 * makenode:	Check if the lines hash halready has a node, if it does link to
 * it, if not then start a new branch.
 */
static void makenode(struct Folio *folio, const size_t i, const size_t j)
{
	struct Line *ln;
	unsigned hashval;

	if ((ln = lookup(folio->files[i].lines[j].line)) != NULL)
	{
		ln->isTrue = folio->files[i].lines[j].isTrue = true;
		if (ln->next != NULL)
			folio->files[i].lines[j].next = ln->next;
		ln->next = &folio->files[i].lines[j];
		lineptr[pt++] = ln;
	} else {
		hashval = hash(folio->files[i].lines[j].line);
		/* Move NULL trminator to the `next` struct */
		folio->files[i].lines[j].next = hashtab[hashval];
		/* Put new struct into a hash bucket. */
		hashtab[hashval] = &folio->files[i].lines[j];
	}
}

/*
 * install:	link line structs to hash bucket.
 */
void hashtable(struct Folio *folio)
{
	size_t i, j;

	for (i = 0; i < folio->t_files; i++)
		for (j = 0; j < folio->files[i].f_lines; j++)
			if (*folio->files[i].lines[j].line != '\0')
				makenode(folio, i, j);
}

