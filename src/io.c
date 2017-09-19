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

/**
 * definefile:	Record input details in file struct, either file or argv
 * string.
 */
struct File definefile(struct File file, unsigned char* name, int type, size_t len)
{
	if (type == type_file)
		file.f_name.name = name;
	else
		file.f_name.name = (unsigned char*)"argv", file.f_name.str = name;
	file.flag = type;
	file.root->t_len += file.f_len = len;
	return file;
}

/*
 * getinput:	Input files and strings from supplied arguments, counting the
 * total length for use in total memory assignment.
 */
void getinput(struct Folio *fx1, int argc, char *argv[])
{
	FILE *fp;
	size_t i, j;

	/* Count and files or strings, allocate file structs */
	for (i = 1, j = 0; i < (unsigned)argc; i++)
		if (*argv[i] != '-')
			j++;

	fx1->t_files = j;
	fx1->files = malloc(j*sizeof(struct File));

	for (i = 0; i < j; i++)
		fx1->files[i] = init_file(&fx1->files[i], fx1);

	/* Input argv if not a flag */
	i = j = 0;
	while (++i < (unsigned int)argc)
		if (*argv[i] != '-') {
			if ((fp = fopen(argv[i], "r")) != NULL) {
				fx1->files[j] = definefile(
						fx1->files[j],
						(unsigned char*)argv[i],
						type_file,
						filesize(fp));
				fclose(fp);
			} else
				fx1->files[j] = definefile(
						fx1->files[j],
						(unsigned char*)argv[i],
						type_string,
						1+strlen(argv[i]));
			j++;
		}
}

/*
 * readfile:	Transfer input form files to memory, count end of line
 * characters to obtain the line count, convert newline char to nul
 * terminators.
 */
static unsigned char *readfile(struct File *file, unsigned char* mem)
{
	FILE *fp;
	size_t i;	/* Check that this is not the first char of a file */
	int c;

	fp = fopen((char*)file->f_name.name, "r");

	for (i = 0; (c = getc(fp)) != EOF; *mem++ = (unsigned char)c, i++)
		if (c == '\n') {
			if (i == 0)
				file->f_lines++, file->root->t_lines++, c = '\0';
			else if (*(mem-1) != '\\')
				file->f_lines++, file->root->t_lines++, c = '\0';
		}

	/* If there is no nul terminator, count line */
	if(i > 0 && *(mem-1) != '\0')
		file->f_lines++, file->root->t_lines++, *mem++ = '\0';

	fclose(fp);

	return mem;
}

/*
 * readstring:	Copy argument string into folio memory block.
 */
static unsigned char *readstring(struct File *file, unsigned char* mem)
{
	size_t i;	/* k checks that it is not the first char of a line */
	int c;

	for (i = 0; (c = file->f_name.str[i]) != '\0'; *mem++ = (unsigned char)c, i++)
		if (c == '\n') {
			if (i == 0)
				file->f_lines++, file->root->t_lines++, c = '\0';
			else if (*(mem-1) != '\\')
				file->f_lines++, file->root->t_lines++, c = '\0';
		}

	*mem++ = '\0';
	file->f_lines++, file->root->t_lines++;

	return mem;
}

/*
 * defineline:	Exchange instances of \n for \0, record line details.
 */
static void defineline(struct Folio *fx1, const size_t i, size_t *j, size_t *k)
{
	fx1->files[i].lines[*j].name = &(fx1->files[i].f_name);
	fx1->files[i].lines[*j].next = NULL;
	fx1->files[i].lines[*j].file = &fx1->files[i];
	fx1->files[i].lines[*j].len = *k;
	fx1->files[i].lines[*j].num = (*j)+1;
	fx1->files[i].lines[*j].isTrue = 0;
}

/*
 * alloclines:	 Memory for Line pointer array and structs.
 */
static void alloclines(struct Folio *fx1)
{
	size_t i;

	if ((fx1->linesArray = malloc(fx1->t_lines*(sizeof(struct Line)))) == NULL)
		printf("error:	malloc failed to assign memory in alloclines(), Line\n");

	for (i = 0; i < fx1->t_lines; i++)
		fx1->linesArray[i] = init_line(&fx1->linesArray[i], i);
}

/*
 * assignlines:	 Define addresses of Line pointer array of structs.
 */
static void assignlines(struct Folio *fx1)
{
	size_t i;
	struct Line *l_ptr;
	l_ptr = fx1->linesArray;

	for (i = 0; i < fx1->t_files; i++) {
		fx1->files[i].lines = l_ptr;
		l_ptr += fx1->files[i].f_lines;
	}
}

/*
 * loadfolio:	For given folio struct create required memory and store all
 * eliments addressed by argv.
 */
void loadfolio(struct Folio *fx1)
{
	size_t i, j, k;
	unsigned char *mem;

	if ((fx1->memory = malloc(fx1->t_len*sizeof(char)) ) == NULL)
		printf("error:	malloc failed to assign memory in loadfx1(), memory\n");

	mem = fx1->memory;

	/* Copy each string into allocated memory. */
	for (i = 0; i < fx1->t_files; i++)
		if (fx1->files[i].flag)
			mem = readfile(&fx1->files[i], mem);
		else
			mem = readstring(&fx1->files[i], mem);

	/* Allocate memory to structs and set them to point to string addresses. */
	mem = fx1->memory;
	alloclines(fx1);
	assignlines(fx1);

	for (i = 0; i < fx1->t_files; i++)
		/* k+1 (+1) to skip over the nul terminator. */
		for (j = 0, k = 0; j < fx1->files[i].f_lines; mem += k+1, j++) {
			fx1->files[i].lines[j].line = mem;
			k = strlen((char*)mem);
			defineline(fx1, i, &j, &k);
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
 * printhash:	Print structs from hash array.
 */
void printhash(struct Sort *s)
{
	size_t i;

	if (state.reverse)
		for (i = s->pt; i > 0; i--)
			printline(s->results[i-1]);
	else
		for (i = 0; i < s->pt; i++)
			printline(s->results[i]);
}

/*
 * printfolio:	Print entire folio array.
 */
void printfolio(struct Folio *fx1)
{
	size_t i, j;

	if (state.reverse)
		for (i = fx1->t_files; i > 0; i--)
			for (j = fx1->files[i-1].f_lines; j > 0; j--)
				printline(&fx1->files[i-1].lines[j-1]);
	else
		for (i = 0; i < fx1->t_files; i++)
			for (j = 0; j < fx1->files[i].f_lines; j++)
				printline(&fx1->files[i].lines[j]);
}

/*
 * free_folio:	Free allocated memory for folio structure and content.
 */
void free_folio(struct Folio *fx1)
{
	free(fx1->memory);
	free(fx1->linesArray);
	free(fx1->files);
	free(fx1);
}

/*
 * free_sort:	Free allocated memory for Sort struct and array.
 */
void free_sort(struct Sort *s)
{
	free(s->results);
	free(s);
}
