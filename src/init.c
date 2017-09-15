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
 * initFolio:	portfolio of file input.
 */
struct Folio *initFolio(struct Folio *folio)
{
	folio->files = NULL;
	folio->memory = NULL;
	folio->linesArray = NULL;
	folio->t_files = 0;
	folio->t_lines = 0;
	folio->t_len = 0;
	folio->ntab = 0;
	return folio;
}

/**
 * init_file:	Set default values for File struct.
 */
struct File init_file(struct File *file, struct Folio *root)
{
	file->lines = NULL;
	file->f_name.name = NULL;
	file->root = root;
	file->f_lines = 0;
	file->f_len = 0;
	file->flag = 0;
	return *file;
}

/**
 * init_line:	set default values for Line struct.
 */
struct Line init_line(struct Line *line, size_t id)
{
	line->line = NULL;
	line->name = NULL;
	line->file = NULL;
	line->next = NULL;
	line->len = 0;
	line->num = 0;
	line->id = id;
	line->isTrue = 0;
	return *line;
}

