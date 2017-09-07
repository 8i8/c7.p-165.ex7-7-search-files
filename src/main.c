/*
 * Exercise 7-7. Modify the pattern finding program of Chapter 5 to take its
 * input from a set of named files or, if no files are named as arguments, from
 * the standard input. Should the file name be printed when a matching line is
 * found?
 */
#include "search-files.h"

int main(int argc, char *argv[])
{
	if (argc > 1)
		settings(argc, argv);

	if (folio.t_files)
		loadfolio(&folio);
	else
		printf("usage:	%s <file1> <file2>\n", *argv);
	
	hashtable(&folio);

	/* Sort input */
	//sortsection((void**)lineptr, 0, pt-1, state.func, 0);
	
	/* If required, add line spacers. */
	//if (state.directory)
	//	pt = addspacer(lineptr, folio.t_files, pt, 0);

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

	printfolio(folio);
	//printhash(lineptr, pt);

	freeall(&folio);

	return 0;
}

/*
 * settings:	If the argument is preceded by '-' it is a flag, program state
 * is set accordingly else it is either a string or a file, in which case it is
 * sent to getinput().
 */
void settings(int argc, char*argv[])
{
	size_t i;

	resetglobals();

	for (i = 1; i < (unsigned)argc; i++)
		if (*argv[i] == '-')
			getflags(i, argv);

	getinput(&folio, argc, argv);
}

