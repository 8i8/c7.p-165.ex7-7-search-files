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
 *	-p, no sort
 *	-r, reverse sort
 *	-s, basic string compare
 */
void getflags(int argc, char*argv[])
{
	int c;
	size_t i = 0;

	while ((c = *(argv[argc]+(++i)))) {
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

/*
 * getinput:	Input files from supplied arguments, counting the total length
 * for use in total memory assignment.
 */
void getinput(struct Folio *folio, int argc, char *argv[])
{
	FILE *fp;
	size_t i, j;

	for (i = 1, j = 0; i < (unsigned)argc; i++)
		if (*argv[i] != '-')
			j++;

	folio->files = malloc(j*sizeof(struct File));
	folio->t_files = j;

	for (i = 0; i < j; i++) {
		folio->files[i].lines = NULL;
		folio->files[i].f_name.name = NULL;
		folio->files[i].str = NULL;
		folio->files[i].f_lines = 0;
		folio->files[i].f_len = 0;
		folio->files[i].flag = 0;
	}

	/* Input files if address given */
	//i = (unsigned)argc, j = 0;
	i = j = 0;
	while (++i < (unsigned int)argc)
		if (*argv[i] != '-') {
			if ((fp = fopen(argv[i], "r")) != NULL) {
				folio->files[j].f_name.name = (unsigned char*)argv[i];
				folio->files[j].flag = 1;
				folio->t_len += folio->files[j].f_len = filesize(fp);
				fclose(fp);
			} else {
				folio->files[j].str = (unsigned char*)argv[i];
				folio->files[j].f_name.name = (unsigned char*)"$string";
				folio->t_len += folio->files[j].f_len = strlen(argv[i]);
			}
			j++;
		}
}

/*
 * readfile:	Transfer input form file to memory, count end of line
 * characters whilst doing so.
 */
static unsigned char *readfile(struct Folio *folio, unsigned char* mem, size_t i)
{
	FILE *fp;
	size_t j;	/* Simply to check that it is not the first char of a file */
	int c;
	j = 0;
	fp = fopen((char*)folio->files[i].f_name.name, "r");

	while ((c = getc(fp)) != EOF) {
		if (c == '\n' && (j > 0 && (*(mem-1) != '\\')))
			folio->files[i].f_lines++, folio->t_lines++, c = '\0';
		*mem++ = (unsigned char)c, j++;
	}
	/* If there is no '\0' char in the previous place; The line has not yet
	 * been counted; Count it and then add the nul trminator. */
	if(j > 0 && *(mem-1) != '\0')
		folio->files[i].f_lines++, folio->t_lines++, *mem++ = '\0';

	/* This line solves an out by one error introduced by the offset
	 * between the count and the index in the Line structs */
	//folio->t_lines++;

	fclose(fp);

	return mem;
}

/*
 * readstring:	Copy argument string into folio memory block.
 */
static unsigned char* readstring(struct Folio *folio, unsigned char* mem, const size_t i)
{
	size_t j, k;	/* k checks that it is not the first char of a line */
	unsigned char c;

	for (j = 0, k = 0; (c = *(folio->files[i].str+j)) != '\0'; j++) {
		if (c == '\n' && (k > 0 && (*(mem-1) != '\\')))
			folio->files[i].f_lines++, folio->t_lines++, c = '\0';
		*mem++ = (char)c, k++;
	}
	/* If there is not a nul trerminator in the previous place; The line
	 * has not yet been counted; Count it. If there is, then there are two,
	 * remove one of them */
	mem--;
	if(k > 0 && *(mem-1) != '\0')
		folio->files[i].f_lines++, folio->t_lines++, mem++;

	return mem;
}

/*
 * defline:	Exchange instances of \n for \0, record line details.
 */
static void defline(struct Folio *folio, const size_t i, size_t *j, size_t *k)
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

	for (i = 0; i < folio->t_lines; i++) {
		folio->linesArray[i].line = NULL;
		folio->linesArray[i].name = NULL;
		folio->linesArray[i].next = NULL;
		folio->linesArray[i].len = 0;
		folio->linesArray[i].num = 0;
		folio->linesArray[i].id = i;
		folio->linesArray[i].isTrue = 0;
	}
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

	/* Request required memory for all strings*/
	if ((folio->memory = malloc(folio->t_len*sizeof(char)) ) == NULL)
		printf("error:	malloc failed to assign memory in loadfolio(), memory\n");

	/* set pointer to start of memory block */
	mem = folio->memory;

	/* For each file copy onto allocated memory, set each entry point and count
	 * new line char for struct creation that follows */
	for (i = 0; i < folio->t_files; i++)
	{
		/* Count \n's and transcribe form source to memory. */
		if (folio->files[i].flag)
			mem = readfile(folio, mem, i);
		else
			mem = readstring(folio, mem, i);
	}

	/* Reset to start address. */
	mem = folio->memory;

	/* Memory for pointer array and structs */
	alloclines(folio);
	assignlines(folio);

	/* Assign each files starting address to the array */
	for (i = 0; i < folio->t_files; i++)
	{
		/* Set the start of each line to the respective structr. */
		for (j = 0, k = 0; j < folio->files[i].f_lines; mem++) {
			folio->files[i].lines[j].line = mem;
			k = strlen((char*)mem);
			defline(folio, i, &j, &k);
			j++, mem += k;
		}
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

	for (i = 0; i < len; i++)
		printline(lineptr[i]);
}

/*
 * printfolio:	Basic test of folio struct data.
 */
void printfolio(struct Folio folio)
{
	size_t i, j;

	for (i = 0; i < folio.t_files; i++)
		for (j = 0; j < folio.files[i].f_lines; j++)
			if (folio.files[i].lines[j].isTrue)
				printf("%s:%3lu: %s\n", folio.files[i].f_name.name, j+1,
						folio.files[i].lines[j].line);
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

