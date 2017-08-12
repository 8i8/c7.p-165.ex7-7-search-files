/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    init.c ~ initiate variables
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "search-files.h"

/* main:	state for program functions */
State state = {
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	alpha
};

/* main:	portfolio of file input */
Folio folio = { { {
		NULL,
		NULL,
		NULL,
		0,
		0,
		0
	} },
	NULL,
	0,
	0,
};

Line lines = {
	NULL,
	NULL,
	0,
	0,
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
