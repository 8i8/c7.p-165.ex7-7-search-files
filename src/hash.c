/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Hash table
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "search-files.h"
#include <string.h>

#include <ctype.h>
#include <assert.h>

#define P1 		317		/* prime smaller than s^16/200 */
#define P2 		65521		/* prime smaller than 2^16 */
#define P3		281539415969051	/* prime smaller than 2^64/65521 */

static Line *hashtab[P2];

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
static Line *lookup(unsigned char *s)
{
	Line *ln;

	for (ln = hashtab[hash(s)]; ln != NULL; ln = ln->next)
		if (strcmp((char*)s, (char*)ln->line) == 0) {
			assert(isprint(ln->line[0]));
			return ln;
		}

	return NULL;
}

/*
 * makenode:	Check if the lines hash halready has a node, if it does link to
 * it, if not then start a new branch.
 */
static void makenode(Folio *folio, const size_t i, const size_t j)
{
	Line *ln;
	unsigned hashval;

	if ((ln = lookup(folio->files[i].lines[j][0].line)) != NULL)
	{
		ln->next = &folio->files[i].lines[j][0];
		if (!ln->isTrue)
			lineptr[pt++] = ln->line;
		lineptr[pt++] = folio->files[i].lines[j][0].line;
		ln->isTrue = folio->files[i].lines[j][0].isTrue = true;
	} else {
		hashval = hash(folio->files[i].lines[j][0].line);
		/* Move existing struct link, if value present, to the current struct */
		folio->files[i].lines[j][0].next = hashtab[hashval];
		/* Put new struct into hash bucket first position */
		hashtab[hashval] = &folio->files[i].lines[j][0];
	}
}

/*
 * install:	link line structs to hash bucket.
 */
void hashtable(Folio *folio)
{
	size_t i, j;

	for (i = 0; i < folio->file_t; i++) {
		for (j = 0; j < folio->files[i].count; j++)
		{
			if (folio->files[i].lines[j][0].line[0] != '\0') {
				makenode(folio, i, j);
			}
		}
	}
}

