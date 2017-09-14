/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    i/o
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "search-files.h"
#include <string.h>

/*
 * getflags:	Get flag arguments for program settings.
 *
 * input arguments:
 *
 * 	-d, directory sort
 * 	-e, remove blank lines
 *	-f, fold lower case to upper case characters
 *	-i, compile for index
 *	-n, numerical sort
 *	-N, print line numbers
 *	-p, no sort
 *	-r, reverse sort
 *	-s, basic string compare
 */
void getflags(int argc, char*argv[])
{
	int c;
	size_t i = 0;

	while ((c = *(argv[argc]+(++i))))
		switch (c) {
			case 'a':
				state.func = alpha;
				break;
			case 'd':
				state.indx = true;
				state.directory = true;
				break;
			case 'e':
				state.remempty = true;
				break;
			case 'f':
				state.func = fold;
				break;
			case 'i':
				state.indx = true;
				break;
			case 'n':
				state.indx = true;
				state.numeric = true;
				break;
			case 'N':
				state.linenum = true;
				break;
			case 'p':
				state.func = nosort;
				break;
			case 'r':
				state.reverse = true;
				break;
			case 's':
				state.func = simple;
				break;
			case 'R':
				state.rsort = true;
			default:
				break;
		}
}

/*
 * filesize:	returns the files size
 */
static size_t filesize(FILE *fp)
{
	size_t len;

	fseek(fp, 0L, SEEK_END);
	len = 1 + (unsigned)ftell(fp);

	return len;
}

struct File definefile(struct File file, unsigned char* name, short int type, size_t len)
{
	file.f_name.name = name;
	file.flag = type;
	folio.t_len += file.f_len = len;
	return file;
}

/*
 * getinput:	Input files and strings from supplied arguments, counting the
 * total length for use in total memory assignment.
 */
void getinput(struct Folio *folio, int argc, char *argv[])
{
	FILE *fp;
	size_t i, j;

	/* Count and files or strings, allocate file structs */
	for (i = 1, j = 0; i < (unsigned)argc; i++)
		if (*argv[i] != '-')
			j++;

	folio->files = malloc(j*sizeof(struct File));
	folio->t_files = j;

	for (i = 0; i < j; i++)
		folio->files[i] = initfile(folio->files[i]);

	/* Input argv if not a flag */
	i = j = 0;
	while (++i < (unsigned int)argc)
		if (*argv[i] != '-') {
			if ((fp = fopen(argv[i], "r")) != NULL) {
				folio->files[j] = definefile(
						folio->files[j],
						(unsigned char*)argv[i],
						file,
						filesize(fp));
				fclose(fp);
			} else
				folio->files[j] = definefile(
						folio->files[j],
						(unsigned char*)argv[i],
						file,
						strlen(argv[i]));
			j++;
		}
}

/*
 * readfile:	Transfer input form files to memory, count end of line
 * characters to obtain the line count whilst doing so, convert new line to nul
 * terminators.
 */
static unsigned char *readfile(struct Folio *folio, unsigned char* mem, size_t i)
{
	FILE *fp;
	size_t j;	/* Check that this is not the first char of a file */
	int c;

	fp = fopen((char*)folio->files[i].f_name.name, "r");

	for (j = 0; (c = getc(fp)) != EOF; *mem++ = (unsigned char)c, j++)
		if (c == '\n' && (j > 0 && (*(mem-1) != '\\')))
			folio->files[i].f_lines++, folio->t_lines++, c = '\0';

	/* If there is no nul terminator, count line */
	if(j > 0 && *(mem-1) != '\0')
		folio->files[i].f_lines++, folio->t_lines++, *mem++ = '\0';

	fclose(fp);

	return mem;
}

/*
 * readstring:	Copy argument string into folio memory block.
 */
static unsigned char* readstring(struct Folio *folio, unsigned char* mem, const size_t i)
{
	size_t j, k;	/* k checks that it is not the first char of a line */
	int c;

	for (j = 0, k = 0; (c = *(folio->files[i].f_name.name+j)) != '\0'; *mem++ = (unsigned char)c, j++, k++)
		if (c == '\n' && (k > 0 && (*(mem-1) != '\\')))
			folio->files[i].f_lines++, folio->t_lines++, c = '\0';

	/* If there is no nul terminator, count line */
	if(k > 0 && *(--mem-1) != '\0')
		folio->files[i].f_lines++, folio->t_lines++, mem++;

	return mem;
}

/*
 * defineline:	Exchange instances of \n for \0, record line details.
 */
static void defineline(struct Folio *folio, const size_t i, size_t *j, size_t *k)
{
	folio->files[i].lines[*j].name = &(folio->files[i].f_name);
	folio->files[i].lines[*j].next = NULL;
	folio->files[i].lines[*j].len = *k;
	folio->files[i].lines[*j].num = (*j)+1;
	folio->files[i].lines[*j].isTrue = 0;
}

/*
 * alloclines:	 Memory for Line pointer array and structs.
 */
static void alloclines(struct Folio *folio)
{
	size_t i;

	if ((folio->linesArray = malloc(folio->t_lines*(sizeof(struct Line)))) == NULL)
		printf("error:	malloc failed to assign memory in alloclines(), Line\n");

	for (i = 0; i < folio->t_lines; i++)
		folio->linesArray[i] = initline(folio->linesArray[i], i);
}

/*
 * assignlines:	 Define addresses of Line pointer array of structs.
 */
static void assignlines(struct Folio *folio)
{
	size_t i;
	struct Line *l_ptr;
	l_ptr = folio->linesArray;

	for (i = 0; i < folio->t_files; i++) {
		folio->files[i].lines = l_ptr;
		l_ptr += folio->files[i].f_lines;
	}
}

/*
 * loadfolio:	For given folio struct create required memory, read in the file
 * contents, or the string, into an array of pointers; Use one string for each
 * line.
 */
void loadfolio(struct Folio *folio)
{
	size_t i, j, k;
	unsigned char *mem;

	if ((folio->memory = malloc(folio->t_len*sizeof(char)) ) == NULL)
		printf("error:	malloc failed to assign memory in loadfolio(), memory\n");

	mem = folio->memory;

	/* Copy each string into allocated memory. */
	for (i = 0; i < folio->t_files; i++)
		if (folio->files[i].flag)
			mem = readfile(folio, mem, i);
		else
			mem = readstring(folio, mem, i);

	/* Allocate memory to structs and join strings. */
	mem = folio->memory;
	alloclines(folio);
	assignlines(folio);

	for (i = 0; i < folio->t_files; i++)
		for (j = 0, k = 0; j < folio->files[i].f_lines; mem += k+1, j++) {
			folio->files[i].lines[j].line = mem;
			k = strlen((char*)mem);
			defineline(folio, i, &j, &k);
		}
}

/*
 * settabs:	Set system tab width.
 */
void settabs(char n[])
{
	char tabs[10] = { "tabs " };
	strcat(tabs, n);
	system(tabs);
}

/*
 * printline:	Print given node and recursivly any that are linked.
 */
void printline(struct Line *line)
{
	if (state.linenum)
		printf("%s:%lu:%s\n", line->name->name, line->num, line->line);
	else
		printf("%s:%s\n", line->name->name, line->line);
	if (line->next != NULL)
		printline(line->next);
}

/*
 * printhash:	Write output.
 */
void printhash(struct Line *lineptr[], size_t len)
{
	size_t i;

	if (state.reverse)
		for (i = len; i > 0; i--)
			printline(lineptr[i-1]);
	else
		for (i = 0; i < len; i++)
			printline(lineptr[i]);
	
}

/*
 * printfolio:	Basic test of folio struct data.
 */
void printfolio(struct Folio folio)
{
	size_t i, j;

	if (state.reverse)
		for (i = folio.t_files; i > 0; i--)
			for (j = 0; j < folio.files[i-1].f_lines; j++)
				printline(&folio.files[i-1].lines[j]);
	else
		for (i = 0; i < folio.t_files; i++)
			for (j = 0; j < folio.files[i].f_lines; j++)
				printline(&folio.files[i].lines[j]);
}

/*
 * freeall:	Free all allocated memory.
 */
void freeall(struct Folio *folio)
{
	free(folio->memory);
	free(folio->linesArray);
	free(folio->files);
}

