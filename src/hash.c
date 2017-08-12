/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Hash table
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "search-files.h"
#include <string.h>

#define P1 		317		/* prime smaller than s^16/200 */
#define P2 		65521		/* prime smaller than 2^16 */
#define P3		281539415969051	/* prime smaller than 2^64/65521 */

static Line *hashtab[P2];

/*
 * hash:	form hash value for string s
 */
static unsigned long hash(char *s)
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
static Line *lookup(char *s)
{
	Line *ln;

	for (ln = hashtab[hash(s)]; ln != NULL; ln = ln->h_next)
		if (strcmp(s, ln->line) == 0)
			return ln;
	return NULL;
}

/*
 * install:	link line structs to hash bucket.
 */
Folio hashtable(Folio folio)
{
	size_t i, j;
	Line *ln;
	unsigned hashval;

	for (i = 0; i < folio.count; i++) {
		for (j = 0; j < folio.files[i].count; j++)
		{
			if ((ln = lookup(folio.files[i].lines[j].line)) != NULL)
			{
				ln->h_next = &folio.files[i].lines[j];
				if (folio.files[i].lines[j].line[0] != '\0') {
					if (!ln->isTrue)
						lineptr[pt++] = ln;
					lineptr[pt++] = &folio.files[i].lines[j];
					ln->isTrue = 1, folio.files[i].lines[j].isTrue = 1;
				}
			} else {
				hashval = hash(folio.files[i].lines[j].line);
				/* Move existing struct link, if value present, to the current struct */
				folio.files[i].lines[j].h_next = hashtab[hashval];
				/* Put new struct into hash bucket first position */
				hashtab[hashval] = &folio.files[i].lines[j];
			}
		}
	}

	return folio;
}

