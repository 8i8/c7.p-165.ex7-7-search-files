/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Sort.
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "search-files.h"
#include <ctype.h>
#include <string.h>

/* _qsort */
//static int nsort(unsigned char *left, unsigned char *right, comp fn);
//static int firstcmp(unsigned char *s1, unsigned char *s2, size_t ntab);
//static int tabcmp(unsigned char *s1, unsigned char *s2, size_t ntab);

/* Sort functions */
static void swap(void *string, size_t i, size_t j, size_t width);
static unsigned char* jumptochar(unsigned char *c);
static unsigned char* jumptotab(unsigned char *c, size_t ntab);
static int sortfolded(const void *s1, const void *s2);
static int numcmp(const void *l1, const void *l2);
static int strtcmp(const void *s, const void *t);
static int sortalpha(const void *s1, const void *s2);
static int strtcmp(const void *s, const void *t);

/* Function pointers */
comp strsimp = (int (*)(const void*, const void*)) strtcmp;
comp stnsort = (int (*)(const void*, const void*)) sortalpha;
comp strfold = (int (*)(const void*, const void*)) sortfolded;

/*
 * sortsection:	Switch, selects the sort function for qsort, see program states.
 */
void sortsection(void *lines, int nel, size_t width, int func)
{
	switch (func) {
		case simple:
			_qsort((void*)lines, nel, width, strsimp);
			break;
		case alpha:
			_qsort((void*)lines, nel, width, stnsort);
			break;
		case fold:
			_qsort((void*)lines, nel, width, strfold);
			break;
		case nosort:
			break;
		default:
			break;
	}
}

/*
 * qsort:	generic qsort function.
 */
void _qsort(void *base, size_t nel, size_t width, int (*fn)(const void*, const void*))
{
	unsigned char *b = (unsigned char*)base;
	size_t i, left, last;

	left = 0;
	if (nel)
		nel--;

	if (left >= nel)
		return;

	swap(b, left, nel/2, width);

	last = left;
	for (i = left+1; i <= nel; i++)
		if ((fn)(&b[i*width], &b[left*width]) < 0)
			swap(b, ++last, i, width);

	swap(b, left, last, width);

	_qsort(b+(left*width), last-left, width, fn);
	_qsort(b+((last+1)*width), nel-last, width, fn);
}

/*
 * compfileds:	group together identical lines and make a list from their
 * corresponding numbers.
 */
//static size_t compfields(struct Line *lineptr[], size_t left, size_t right, size_t nlines, int ntab)
//{
//	unsigned char comp[MAXLEN];
//	unsigned char line[MAXLEN];
//	unsigned char *c;
//	size_t mark;
//        mark = left;
//	int i; //temp to quieten the compiler
//
//	/*
//	 * Copy the first line as a base for concatenation, and again as a
//	 * temporary comparator, delete the original.
//	 */
//	strcpy((char*)comp, (char*)lineptr[left]);
//	strcpy((char*)line, (char*)lineptr[left]);
//	//nlines = deleteline(lineptr, left++, nlines);
//
//	while (left <= right)
//	{
//		if (!tabcmp(comp, lineptr[mark], lineptr[mark]->file->root->ntab-1)) { 
//			c = lineptr[mark];
//
//			if ((c = jumptotab(c, ntab)) == NULL) {
//				mark++, left++;
//				continue;
//			}
//
//			c = jumptochar(c);
//			strcat((char*)line, ", ");
//			strcat((char*)line, (char*)c);
//
//			//nlines = deleteline(lineptr, mark, nlines);
//		} else
//			mark++;
//		left++;
//	}
//
//	//nlines = insertline(lineptr, line, MAXLINES, orig, nlines);
//
//	return nlines;
//}

/*
 * firstcmp:	Compare the first char of each line, return 0 if there is an
 * alphabetical match and 1 if there is not. Essentially, to select the input
 * width for qsort, left and right. Also used when adding empty spacer lines.
 */
static int firstcmp(unsigned char *s1, unsigned char *s2, size_t ntab)
{
	bool p1, p2;
	unsigned char *s1_pt, *s2_pt;
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
//static int tabcmp(unsigned char *s1, unsigned char *s2, size_t ntab)
//{
//	bool p1, p2;
//	unsigned char *s1_pt, *s2_pt;
//	int res;
//	p1 = p2 = false;
//	s1_pt = s1, s2_pt = s2;
//	res = 0;
//
//	/*
//	 * Jump to specified tab if it exists in both strings, else return 0;
//	 */
//	if (ntab) {
//		if ((s1 = jumptotab(s1, ntab)) == NULL)
//			s1 = s1_pt, p1 = true;
//		if ((s2 = jumptotab(s2, ntab)) == NULL)
//			s2 = s2_pt, p2 = true;
//		/*
//		 * If either pointers return null the tab does not exist,
//		 * return 1, values are not the same.
//		 */
//		if (p1 == true || p2 == true)
//			return 1;
//	}
//
//	/*
//	 * Jump to the first relevant character.
//	 */
//	s1 = jumptochar(s1);
//	s2 = jumptochar(s2);
//
//	/*
//	 * If the strings are identical up until the tab, return 0.
//	 */
//	res = strtcmp(s1, s2);
//
//	if (!res)
//		return 0;
//
//	return 1;
//}

/*
 * sortdivide:	Search over the array looking for lines that are grouped
 * together alphabetically or as blocks of numbers, select the start and end
 * index of each group and then sort by the next argv input using the given tab
 * field.
 */
//size_t sortdivide(unsigned char *lineptr[], int func, size_t nlines, size_t ntab)
//{
//	size_t i, j;
//	i = j = 0;
//
//	while (++i < nlines)
//		/*
//		 * If the first char of both lines differ, start the count.
//		 */
//		if (!firstcmp(lineptr[i-1], lineptr[i], ntab) || i == 1) {
//			/*
//			 * Whilst the first char of the prior tab stop are the
//			 * same; Keep on counting.
//			 */
//			j = i-1;
//			while (i < nlines && !strtcmp(lineptr[i-1], lineptr[i]))
//				i++;
//			/*
//			 * Perform sort between this current change of letter
//			 * and the last stored index j; then store i as j.
//			 * If directory mode is set, call compfields()
//			 * function.
//			 */
//			if (state.indx) {
//				sortsection((void**)lineptr, j, i-1, func);
//				nlines = compfields(lineptr, j, i-1, nlines, ntab);
//			} else
//				sortsection((void**)lineptr, j, i-1, func);
//		}
//
//	return nlines;
//}

/*
 * addspacer:	Add empty 'spacer' line.
 */
size_t addspacer(unsigned char *lineptr[], size_t maxlines, size_t nlines, size_t ntab)
{
	size_t i = 0;

	while (++i < nlines)
		if (!firstcmp(lineptr[i-1], lineptr[i], ntab) && 
				(!isdigit(*lineptr[i-1]) || !isdigit(*lineptr[i]))) {
			 i=maxlines;//nlines = insertline(lineptr, (int)'\0', maxlines, i++, nlines);
		}

	return nlines;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Sort functions and char maps.
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*
 * Swap for 'width' places, 'i' to 'j' on the given string.
 */
void swap(void *string, size_t i, size_t j, size_t width)
{
	if (i == j)
		return; /* return directly if swap is not required */

	unsigned char t;
	unsigned char *s = string;

	i = i*width; 
	j = j*width;

	size_t k;
	for (k = 0; k < width; k++) {
		t        = *(s+k+i);
		*(s+k+i) = *(s+k+j);
		*(s+k+j) = t;
	}
}

/*
 * jumptochar:	Skip over all spaces and non alphanumeric char from the start
 * of the string.
 */
static unsigned char* jumptochar(unsigned char *c)
{
	while (!isalnum(*c) && *c != '\0' && *c != '\t')
		c++;
	return c;
}

/*
 * jumptotab:	Skip to the n'th tab.
 * getline
 */
static unsigned char* jumptotab(unsigned char *c, size_t ntab)
{
	while (*c != '\0')
		if (*c++ == '\t' && --ntab == 0)
			return c;

	return NULL;
}

/*
 * sortascii:	Conversion used for sortfolded.
 */
static int sortascii(char *c, bool fold)
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
static int sortalpha(const void *l1, const void *l2)
{
	struct Line *s1, *s2;
	unsigned char c1, c2;
	s1 = (struct Line*)l1, s2 = (struct Line*)l2;

	c1 = sortascii((char*)&s1->line[0], false);
	c2 = sortascii((char*)&s2->line[0], false);
	return c1 - c2;
}

/*
 * sortfolded:	Sort string with Upper case folded in.
 */
static int sortfolded(const void *l1, const void *l2)
{
	const struct Line *s1, *s2;
	s1 = l1, s2 = l2;
	unsigned char c1, c2;

	c1 = sortascii((char*)&s1->line[0], true);
	c2 = sortascii((char*)&s2->line[0], true);
	return c1 - c2;
}

/*
 * Compare s1 and s2 numerically.
 */
static int numcmp(const void *l1, const void *l2)
{
	const struct Line *s1, *s2;
	s1 = (struct Line*)l1, s2 = (struct Line*)l2;

	double v1, v2;
	v1 = atof((char*)s1->line);
	v2 = atof((char*)s2->line);
	if (v1 < v2)
		return -1;
	else if (v1 > v2)
		return 1;
	return 0;
}

/*
 * strtcmp:	String compare that will stop at a tabstop.
 */
static int strtcmp(const void *s, const void *t)
{
	size_t i;
	const struct Line *u, *v;
	u = s, v = t;
        for (i = 0 ; *u->line == *v->line; i++)
                if (*(u->line+i) == '\0' || *(v->line+i) == '\t')
                        return 0;
        return *u->line - *v->line;
}

///*
// * _qsort:	Sort v[left]...v[right] into increasing order.
// */
//void _qsort(void *line[], int left, int right, compar fn, int ntab)
//{
//	size_t i, last;
//
//	if (left >= right)		/* do nothing if array contains */
//		return;			/* fewer than two elements */
//
//	swap(line, left, (left + right)/2);
//	last = left;
//
//	/*
//	 * Perform sort in either the direct or the reverse order.
//	 */
//	if (!state.reverse) {
//		for (i = left+1; (int)i <= right; i++)
//			if (nsort(line[i], line[left], fn, ntab) < 0)
//				swap(line, ++last, i);
//	} else
//		for (i = left+1; (int)i <= right; i++)
//			if (nsort(line[i], line[left], fn, ntab) > 0)
//				swap(line, ++last, i);
//
//	swap(line, left, last);
//	_qsort(line, left, last-1, fn, ntab);
//	_qsort(line, last+1, right, fn, ntab);
//}
//
///*
// * nsort:	Prepare string for sort function, filter numbers and letters,
// * recursive call to sort function; Separating this section of the function
// * from the body of qsort, has enabled shorter reverse '-r' code in qsort.
// */
//static int nsort(unsigned char *left, unsigned char *right, compar fn, int ntab)
//{
//	unsigned char *l_pt, *r_pt;
//	int res = 0;
//	bool b1, b2, p1, p2;
//	b1 = b2 = p1 = p2 = false;
//	l_pt = left, r_pt = right;
//
//	/*
//	 * Move to desired tab.
//	 */
//	if (ntab) {
//		if ((left = jumptotab(left, ntab)) == NULL)
//			left = l_pt, p1 = true;
//		if ((right = jumptotab(right, ntab)) == NULL)
//			right = r_pt, p2 = true;
//		/*
//		 * If either pointers returns null, return 0, the values are
//		 * not to be swapped.
//		 */
//		if (p1 == true || p2 == true)
//			return 0;
//	}
//
//	/*
//	 * Remove redundant char.
//	 */
//	left = jumptochar(left);
//	right = jumptochar(right);
//
//	if (state.numeric) {
//		if (isdigit(*left))
//			b1 = true;
//		if (isdigit(*right))
//			b2 = true;
//	}
//
//	/*
//	 * Return either alphabetical or numerical order.
//	 */
//	if (b1 && b2) {
//		res = numcmp(left, right);
//		if (!res && (*left != '\0' || *left != '\t'))
//			res = nsort(++left, ++right, fn, ntab);
//	} else {
//		res = (*fn)(left, right);
//		if (!res && (*left != '\0' || *left != '\t'))
//			res = nsort(++left, ++right, fn, ntab);
//	}
//
//        return res;
//}
