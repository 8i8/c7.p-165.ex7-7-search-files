/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    i/o
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "search-files.h"
#include <string.h>
#include <stdint.h>

unsigned char *lineptr[MAXLINES];		/* Pointer to text lines */
static unsigned char allocbuf[ALLOCSIZE];	/* Storage for alloc */
static unsigned char *allocp = allocbuf;		/* Next free position */

static size_t _getline(unsigned char *, size_t);
static unsigned char *alloc(size_t);

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

	while ((c = *++argv[argc])) {
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
	rewind(fp);

	return len;
}

/*
 * getinput:	Input files from supplied arguments and count the length of the
 * total input for later use in memory assignment.
 */
void getinput(char* const argument, size_t const i)
{
	FILE *fp;
	size_t count = i+1;

	/* Input files if address given */
	if ((fp = fopen(argument, "r")) != NULL) {
		folio.files[i].name = (unsigned char*)argument, folio.files[i].file = 1,
			folio.count = count;
		folio.len += folio.files[i].len = filesize(fp);
		fclose(fp);
	} else {
		folio.files[i].str = (unsigned char*)argument, folio.files[i].name = (unsigned char*)"$string",
			folio.count = count;
		folio.len += folio.files[i].len = strlen((char*)folio.files[i].str)+1;
	}
}

/*
 * readfile:	Transfer input form file to memory, count end of line
 * characters whilst doing so.
 */
static unsigned char* readfile(Folio *folio, unsigned char* mem, size_t i)
{
	FILE *fp;
	int c;
	fp = fopen((char*)folio->files[i].name, "r");

	while ((c = getc(fp)) != EOF) {
		if (c == '\n' && *(mem-1) != '\\')
			(folio->files[i].count)++;
		*mem++ = (unsigned char)c;
	}
	*mem++ = '\0';
	fclose(fp);

	return mem;
}

static unsigned char* readstring(Folio *folio, unsigned char* mem, const size_t i)
{
	size_t j;
	unsigned char c;

	for (j = 0; (c = *(folio->files[i].str+j)) != '\0'; j++) {
		if (c == '\n' && *(mem-1) != '\\')
			(folio->files[i].count)++;
		*mem++ = (char)c;
	}

	*mem++ = '\0';

	return mem;
}

/*
 * defline:	Exchange all instances of \n for \0.
 */
static void defline(Folio *folio, unsigned char* mem, const size_t i, size_t *j, size_t *k)
{
	folio->files[i].lines[*j].line = mem+1;
	folio->files[i].lines[*j].isTrue = 0;
	folio->files[i].lines[(*j)++].len = *k;
	*k = 0;
	*mem = '\0';
}

/*
 * loadfolio:	For given folio struct create memory for and read in the file
 * contents, or the string, into an array of pointers; Use one string for each
 * line.
 */
Folio loadfolio(Folio folio)
{
	size_t i, j, k;
	char c;
	unsigned char *mem, *mark;

	/* Request required memory */
	if ((folio.memory = malloc(folio.len*sizeof(char))) == NULL)
		printf("error:	malloc failed to assign memory in loadfolio(), memory\n");

	/* set pointer to start of memory block */
	mem = folio.memory;

	/* Copy each file onto allocated memory, set each entry point and count
	 * new line char */
	for (i = 0; i < folio.count; i++)
	{
		/* Count \n's and transcribe form source to memory. */
		if (folio.files[i].file)
			mem = readfile(&folio, mem, i);
		else
			mem = readstring(&folio, mem, i);

		/* If there is no new line char in the previous place, the line
		 * has not yet been counted; Count it. */
		if(*(mem-2) != '\n')
			(folio.files[i].count)++;

		/* Mark the end of the current input and then reset to start. */
		mark = mem;
		mem = folio.memory;

		/* allocate an array of pointers to structs, one for each line
		 * of text; Iterate through the file replacing each end of line
		 * marker with the NUL terminator */
		if ((folio.files[i].lines = malloc(folio.files[i].count*(sizeof(Line*)+sizeof(Line)))) == NULL)
			printf("error:	malloc failed to assign memory in loadfolio(), lines\n");

		/* Set the first structs string to current memory position. */
		folio.files[i].lines[0].line = mem;

		/* Exchange all instances of '\n' for '\0' */
		for (j = 0, k = 0; (c = *mem) != '\0'; mem++, k++)
			if (c == '\n' && *(mem-1) != '\\')
				defline(&folio, mem, i, &j, &k);

		/* Set all markers to start of next memory block */
		mem = folio.memory = mark;
	}

	return folio;
}

/*
 * readlines:	Read input lines, check available space for new line, return
 * line count. Copy the new line into the allocated space and fill lineptr
 * array with pointers to the new lines gathered.
 */
size_t readlines(unsigned char *lineptr[], size_t maxlines)
{
	size_t len, nlines;
	unsigned char *p, line[MAXLEN];

	nlines = 0;
	while ((len = _getline(line, MAXLEN)) > 0)
		if (nlines >= maxlines || (p = alloc(len)) == NULL)
			return 0;
		else {
			if (state.remempty && len == 1)
				continue;
			line[len-1] = '\0'; /* delete newline char*/
			strcpy((char*)p, (char*)line);
			lineptr[nlines++] = p;
		}

	return nlines;
}

/*
 * getline:	Input from stdin line by line.
 */
static size_t _getline(unsigned char *s, size_t lim)
{
	unsigned char *s_in;
	int c;
	s_in = s;

	while (--lim > 0 && (c = getchar()) != EOF && c != '\n')
		*s++ = (unsigned char)c;
	if (c == '\n')
		*s++ = c;
	*s = '\0';

	return s - s_in;
}

/*
 * insertline:	Add new line to char* array, after given index value.
 */
size_t insertline(unsigned char *lineptr[], unsigned char* line, size_t maxlines, size_t index, size_t nlines)
{
	unsigned char *p;
	size_t i = 0;

	/* If there is room in alloc buffer ... */
	if (nlines >= maxlines || (p = alloc(strlen((char*)line)+1)) == NULL) {
		printf("Error alloc: insufficient place in allocbuf[]\n");
		return 0;
	}

	/* Copy the above line to p */
	strcpy((char*)p, (char*)line);
	nlines++;

	/* Add a new index space shunting all pointers forwards one place. */
	i = nlines;
	while (--i > index)
		lineptr[i] = lineptr[i-1];
 
	 /* Add p to the newly created index space. */
	lineptr[index] = p; 

	return nlines;
}

/*
 * alloc:	Count memory use for assigning input, first check that the
 * requested length is available, if it is return a pointer to the array place,
 * if not return 0.
 */
static unsigned char *alloc(size_t n)	/* return pointer to  characters */
{
	if (allocbuf + ALLOCSIZE - allocp >= (unsigned)n) { /* if 'n' fits */
		allocp += n;
		return allocp - n;	/* old p */
	} else		/* not enough room */
		return 0;
}

/*
 * freealloc:	Remove line from allocbuf.
 */
static unsigned char freealloc(unsigned char *allocbuf)
{
	int len;
	unsigned char* line;
	line = allocbuf;

	len = strlen((char*)line)+1;

	if (len) {
		allocp -= len;
		while (line < allocp) {
			*line = *(line+len);
			line++;
		}
		return len;
	} else
		printf("Error freealloc: no string provided\n");

	return 1;
}

/*
 * deleteline:	Delete line.
 */
size_t deleteline(unsigned char *lineptr[], int line, size_t nlines)
{
	size_t i = 0;
	int len;
	unsigned char *m;

	m = lineptr[line];

	if ((len = freealloc(lineptr[line]))) {
		/*
		 * Displace all pointers after the removed by one place.
		 */
		while (line < (int)nlines) {
			lineptr[line] = lineptr[line+1];
			line++;
		}
		/*
		 * Diminish any address that is greater than the address of
		 * the removed line by the length of the remove line.
		 */
		while (i < nlines) {
			if (lineptr[i] > m)
				lineptr[i] -= len;
			i++;
		}
		return --nlines;
	} else
		printf("Error deleteline: freealloc failed\n");

	return nlines;
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
 * writelines:	Write output.
 */
void writelines(unsigned char **lines, size_t lp)
{
	size_t i;

	for (i = 0; i < lp; i++)
		if (state.linenum)
			printf("%3lu: %s\n", i, lines[i]);
		else
			printf("%s\n", lines[i]);
}

/*
 * writelines:	Write output.
 */
void writelinesstruct(Line **lines, size_t lp)
{
	size_t i;

	for (i = 0; i < lp; i++)
		if (state.linenum)
			printf("%3lu: %s\n", i, lines[i]->line);
		else
			printf("%s\n", lines[i]->line);
}

/*
 * printtest:	Basic test of folio struct data.
 */
void printtest(Folio folio)
{
	size_t i, j;

	for (i = 0; i < folio.count; i++)
		for (j = 0; j < folio.files[i].count; j++)
			//if (folio.files[i].lines[j].isTrue)
				printf("%s:%3lu: %s\n", folio.files[i].name, j+1,
						folio.files[i].lines[j].line);
}

