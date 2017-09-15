/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    init.c ~ initiate variables
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "search-files.h"

struct Line *lineptr[100];

/* main:	state for program functions */
struct State state = {
	alpha,
	//simple,
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

/**
 * resetglobals:	Reset all global values.
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

/**
 * initfile:	Set default values for File struct.
 */
struct File initfile(struct File file)
{
	file.lines = NULL;
	file.f_name.name = NULL;
	file.f_lines = 0;
	file.f_len = 0;
	file.flag = 0;
	return file;
}

/**
 * initline:	set default values for Line struct.
 */
struct Line initline(struct Line line, size_t id)
{
	line.line = NULL;
	line.name = NULL;
	line.next = NULL;
	line.len = 0;
	line.num = 0;
	line.id = id;
	line.isTrue = 0;
	return line;
}

