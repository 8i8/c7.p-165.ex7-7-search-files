/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    init.c ~ initiate variables
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "search-files.h"

/* main:	state for program functions */
struct State state = {
	alpha,
	false,
	false,
	false,
	false,
	false,
	false,
	false
};

/* main:	portfolio of file input */
struct Folio folio = {
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
	0
};

/*
 * Values that are reset between argv strings.
 */
void resetglobals(void)
{
	state.numeric = 	false;
	state.reverse = 	false;
	state.remempty =	false;
	state.directory =	false;
	state.rsort = 		false;
	state.indx =		false;
}

size_t pt = 0;

struct Line initline(struct Line ln, size_t id)
{
	ln.line = NULL;
	ln.name = NULL;
	ln.next = NULL;
	ln.len = 0;
	ln.num = 0;
	ln.id = id;
	ln.isTrue = 0;

	return ln;
}

