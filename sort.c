/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Sort.
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "search-files.h"
#include <ctype.h>
#include <string.h>

/* _qsort */
static int nsort(char *left, char *right, compar fn, int ntab);
static int firstcmp(char *s1, char *s2, int ntab);
static int tabcmp(char *s1, char *s2, int ntab);

/* Sort functions */
static void swap(void *v[], size_t i, size_t j);
static char* jumptochar(char *c);
static char* jumptotab(char *c, int ntab);
static int sortalpha(char *s1, char *s2);
static int sortfolded(char *s1, char *s2);
static int numcmp(char *s1, char *s2);
static int strtcmp(char *s, char *t);

/* Function pointers */
compar strsimp = (int (*)(void*, void*)) strcmp;
compar stnsort = (int (*)(void*, void*)) sortalpha;
compar strfold = (int (*)(void*, void*)) sortfolded;

/*
 * _qsort:	Sort v[left]...v[right] into increasing order.
 */
void _qsort(void *v[], int left, int right, compar fn, int ntab)
{
	size_t i, last;

	if (left >= right)	/* do nothing if array contains */
		return;		/* fewer than two elements */

	swap(v, left, (left + right)/2);
	last = left;

	/*
	 * Perform sort in either the direct or the reverse order.
	 */
	if (!state.reverse) {
		for (i = left+1; (int)i <= right; i++)
			if (nsort(v[i], v[left], fn, ntab) < 0)
				swap(v, ++last, i);
	} else
		for (i = left+1; (int)i <= right; i++)
			if (nsort(v[i], v[left], fn, ntab) > 0)
				swap(v, ++last, i);

	swap(v, left, last);
	_qsort(v, left, last-1, fn, ntab);
	_qsort(v, last+1, right, fn, ntab);
}

/*
 * nsort:	Prepare string for sort function, filter numbers and letters,
 * recursive call to sort function; Separating this section of the function
 * from the body of qsort, has enabled shorter reverse '-r' code in qsort.
 */
static int nsort(char *left, char *right, compar fn, int ntab)
{
	char *l_pt, *r_pt;
	int res = 0;
	bool b1, b2, p1, p2;
	b1 = b2 = p1 = p2 = false;
	l_pt = left, r_pt = right;

	/*
	 * Move to desired tab.
	 */
	if (ntab) {
		if ((left = jumptotab(left, ntab)) == NULL)
			left = l_pt, p1 = true;
		if ((right = jumptotab(right, ntab)) == NULL)
			right = r_pt, p2 = true;
		/*
		 * If either pointers returns null, return 0, the values are
		 * not to be swapped.
		 */
		if (p1 == true || p2 == true)
			return 0;
	}

	/*
	 * Remove redundant char.
	 */
	left = jumptochar(left);
	right = jumptochar(right);

	if (state.numeric) {
		if (isdigit(*left))
			b1 = true;
		if (isdigit(*right))
			b2 = true;
	}

	/*
	 * Return either alphabetical or numerical order.
	 */
	if (b1 && b2) {
		res = numcmp(left, right);
		if (!res && (*left != '\0' || *left != '\t'))
			res = nsort(++left, ++right, fn, ntab);
	} else {
		res = (*fn)(left, right);
		if (!res && (*left != '\0' || *left != '\t'))
			res = nsort(++left, ++right, fn, ntab);
	}

        return res;
}

/*
 * compfileds:	group together identical lines and make a list from their
 * corresponding numbers.
 */
static size_t compfields(char *lineptr[], size_t left, size_t right, size_t nlines, int ntab)
{
	char comp[MAXLEN];
	char line[MAXLEN];
	char *c;
	size_t orig, mark;
        orig = mark = left;

	/*
	 * Copy the first line as a base for concatenation, and again as a
	 * temporary comparator, delete the original.
	 */
	strcpy(comp, lineptr[left]);
	strcpy(line, lineptr[left]);
	nlines = deleteline(lineptr, left++, nlines);

	while (left <= right)
	{
		if (!tabcmp(comp, lineptr[mark], ntab-1)) { 
			c = lineptr[mark];

			if ((c = jumptotab(c, ntab)) == NULL) {
				mark++, left++;
				continue;
			}

			c = jumptochar(c);
			strcat(line, ", ");
			strcat(line, c);

			nlines = deleteline(lineptr, mark, nlines);
		} else
			mark++;
		left++;
	}

	nlines = insertline(lineptr, line, MAXLINES, orig, nlines);

	return nlines;
}

/*
 * firstcmp:	Compare the first char of each line, return 0 if there is an
 * alphabetical match and 1 if there is not. Essentially, to select the input
 * width for qsort, left and right. Also used when adding empty spacer lines.
 */
static int firstcmp(char *s1, char *s2, int ntab)
{
	bool p1, p2;
	char *s1_pt, *s2_pt;
	p1 = p2 = false;
	s1_pt = s1, s2_pt = s2;

	/*
	 * Jump to specified tab if it exists in both strings, else return 0;
	 */
	if (ntab) {
		if ((s1 = jumptotab(s1, ntab)) == NULL)
			s1 = s1_pt, p1 = true;
		if ((s2 = jumptotab(s2, ntab)) == NULL)
			s2 = s2_pt, p2 = true;
		/*
		 * If either pointers return null the tab does not exist,
		 * return 1, values are not the same.
		 */
		if (p1 == true || p2 == true)
			return 1;
	}

	/*
	 * Jump to the first relevant character.
	 */
	s1 = jumptochar(s1);
	s2 = jumptochar(s2);

	/*
	 * If the char differ return 0, else return 1 should they be the same.
	 */
	if (sortfolded(s1, s2) && (isalpha(*s1) || isalpha(*s2)))
		return 0;
	else if (isdigit(*s1) && isdigit(*s2))
		if (*s1 != *s2)
			return 0;

	return 1;
}

/*
 * Test if the contents of the given tab fields are identical.
 */
static int tabcmp(char *s1, char *s2, int ntab)
{
	bool p1, p2;
	char *s1_pt, *s2_pt;
	int res;
	p1 = p2 = false;
	s1_pt = s1, s2_pt = s2;
	res = 0;

	/*
	 * Jump to specified tab if it exists in both strings, else return 0;
	 */
	if (ntab) {
		if ((s1 = jumptotab(s1, ntab)) == NULL)
			s1 = s1_pt, p1 = true;
		if ((s2 = jumptotab(s2, ntab)) == NULL)
			s2 = s2_pt, p2 = true;
		/*
		 * If either pointers return null the tab does not exist,
		 * return 1, values are not the same.
		 */
		if (p1 == true || p2 == true)
			return 1;
	}

	/*
	 * Jump to the first relevant character.
	 */
	s1 = jumptochar(s1);
	s2 = jumptochar(s2);

	/*
	 * If the strings are identical up until the tab, return 0.
	 */
	res = strtcmp(s1, s2);

	if (!res)
		return 0;

	return 1;
}

/*
 * sortdivide:	Search over the array looking for lines that are grouped
 * together alphabetically or as blocks of numbers, select the start and end
 * index of each group and then sort by the next argv input using the given tab
 * field.
 */
size_t sortdivide(char *lineptr[], int func, size_t nlines, int ntab)
{
	size_t i, j;
	i = j = 0;

	while (++i < nlines)
		/*
		 * If the first char of both lines differ, start the count.
		 */
		if (!firstcmp(lineptr[i-1], lineptr[i], ntab) || i == 1) {
			/*
			 * Whilst the first char of the prior tab stop are the
			 * same; Keep on counting.
			 */
			j = i-1;
			while (i < nlines && !strtcmp(lineptr[i-1], lineptr[i]))
				i++;

			/*
			 * Perform sort between this current change of letter
			 * and the last stored index j; then store i as j.
			 * If directory mode is set, call compfields()
			 * function.
			 */
			if (state.indx) {
				sortsection(lineptr, j, i-1, func, ntab);
				nlines = compfields(lineptr, j, i-1, nlines, ntab);
			} else
				sortsection(lineptr, j, i-1, func, ntab);
		}

	return nlines;
}

/*
 * addspacer:	Add empty 'spacer' line.
 */
size_t addspacer(char *lineptr[], size_t maxlines, size_t nlines, int ntab)
{
	size_t i = 0;

	while (++i < nlines)
		if (!firstcmp(lineptr[i-1], lineptr[i], ntab) && 
				(!isdigit(*lineptr[i-1]) || !isdigit(*lineptr[i])))
			nlines = insertline(lineptr, "\0", maxlines, i++, nlines);

	return nlines;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Sort functions and char maps.
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*
 * swap:	Interchange v[i] and v[j]
 */
static void swap(void *v[], size_t i, size_t j)
{
	void *temp;

	temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}

/*
 * jumptochar:	Skip over all spaces and non alphanumeric char from the start
 * of the string.
 */
static char* jumptochar(char *c)
{
	while (!isalnum(*c) && *c != '\0' && *c != '\t')
		c++;
	return c;
}

/*
 * jumptotab:	Skip to the n'th tab.
 */
static char* jumptotab(char *c, int ntab)
{
	while (*c != '\0')
		if (*c++ == '\t' && --ntab == 0)
			return c;

	return NULL;
}

/*
 * sortascii:	Conversion used for sortfolded.
 */
static int sortascii(int *c, bool fold)
{
	if (isupper(*c))
		if (fold)
			return *c = tolower(*c);
		else
			return *c += 57;
	else if (islower(*c))
		return *c;
	else if (isdigit(*c))
		return *c += 118;
	return 0;
}

/*
 * sortalpha:	Sorting character maps.
 */
static int sortalpha(char *s1, char *s2)
{
	int c1, c2;
	c1 = *s1, c2 = *s2;
	c1 = sortascii(&c1, false);
	c2 = sortascii(&c2, false);
	return c1 - c2;
}

/*
 * sortfolded:	Sort string with Upper case folded in.
 */
static int sortfolded(char *s1, char *s2)
{
	int c1, c2;
	c1 = *s1, c2 = *s2;
	c1 = sortascii(&c1, true);
	c2 = sortascii(&c2, true);
	return c1 - c2;
}

/*
 * Compare s1 and s2 numerically.
 */
static int numcmp(char *s1, char *s2)
{
	double v1, v2;
	v1 = atof(s1);
	v2 = atof(s2);
	if (v1 < v2)
		return -1;
	else if (v1 > v2)
		return 1;
	return 0;
}

/*
 * strtcmp:	String compare that will stop at a tabstop.
 */
static int strtcmp(char *s, char *t)
{
        for ( ; *s == *t; s++, t++)
                if (*s == '\0' || *s == '\t')
                        return 0;
        return *s - *t;
}

