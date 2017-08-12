/*
 * Exercise 7-7. Modify the pattern finding program of Chapter 5 to take its
 * input from a set of named files or, if no files are named as arguments, from
 * the standard input. Should the file name be printed when a matching line is
 * found?
 */
#include "search-files.h"

/*
 * Sort input lines.
 */
int main(int argc, char *argv[])
{
	//size_t i;

	if (argc > 1)
		settings(argc, argv);

	/* If no files are supplied, fill lineptr array from stdin. */
	if (folio.count)
		folio = loadfolio(folio);
	else
		printf("usage:	%s <file1> <file2>\n", *argv);
	
	folio = hashtable(folio);
	//printtest(folio);

	/* Sort input */
	//sortsection(lineptr, 0, pt, state.func, 0);
	
	///* If required add line spacers. */
	//if (state.directory)
	//	pt = addspacer(lineptr, MAXLINES, pt, 0);

	//i = 1;
	///* Sort using argv */
	//while (++i < (unsigned int)argc) {
	//	settings(i, argv);

	//	/* Sort input */
	//	if (state.rsort)
	//		sortsection(lineptr, 0, pt-1, state.func, i-1);

	//	pt = sortdivide(lineptr, state.func, pt, i-1);

	//	/*
	//	 * If directory setting is used, add a blank line break
	//	 * after each new starting letter.
	//	 */
	//	if (state.directory)
	//		pt = addspacer(lineptr, MAXLINES, pt, i-1);
	//}

	/*
	 * Output to terminal.
	 */
	writelines(lineptr, pt);

	return 0;
}

/*
 * settings:	If the argument is preceded by '-' it is a flag else it is a
 * string, in which case it is sent to getinput() to be oped or recorded.
 */
size_t settings(int argc, char*argv[])
{
	size_t file, i;
	file = i = 0;

	resetglobals();

	for (i = 1; i < (unsigned)argc; i++)
		if (*argv[i] == '-')
			getflags(i, argv);
		else
			getinput(argv[i], file++);
	return file;
}

/*
 * sortsection:	Switch, selects the sort function for qsort.
 */
void sortsection(char *lineptr[], int left, int right, int func, int ntab)
{
	switch (func) {
		case simple:
			_qsort((void **) lineptr, left, right, strsimp, ntab);
			break;
		case alpha:
			_qsort((void **) lineptr, left, right, stnsort, ntab);
			break;
		case fold:
			_qsort((void **) lineptr, left, right, strfold, ntab);
			break;
		case nosort:
			break;
		default:
			break;
	}
}

