/*
 * $Id: shlib.c,v 1.7 1994/02/09 02:29:31 rgrimes Exp $
 */

#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <a.out.h>

#include "ld.h"

#ifdef SUNOS4
char	*strsep();
#endif

/*
 * Standard directories to search for files specified by -l.
 */
#ifndef STANDARD_SEARCH_DIRS
#define	STANDARD_SEARCH_DIRS	"/usr/lib"
#endif

char *standard_search_dirs[] = {
	STANDARD_SEARCH_DIRS
};

int n_search_dirs;

void
add_search_dir(name)
	char	*name;
{
	n_search_dirs++;
	search_dirs = (char **)xrealloc(search_dirs,
				n_search_dirs * sizeof(char *));
	search_dirs[n_search_dirs - 1] = strdup(name);
}

void
std_search_dirs(paths)
char	*paths;
{
	char	*cp;
	int	i, n;

	if (paths != NULL)
		/* Add search directories from `paths' */
		while ((cp = strsep(&paths, ":")) != NULL) {
			add_search_dir(cp);
			if (paths)
				*(paths-1) = ':';
		}

	/* Append standard search directories */
	n = sizeof standard_search_dirs / sizeof standard_search_dirs[0];
	for (i = 0; i < n; i++)
		add_search_dir(standard_search_dirs[i]);
}

/*
 * Return true if CP points to a valid dewey number.
 * Decode and leave the result in the array DEWEY.
 * Return the number of decoded entries in DEWEY.
 */

int
getdewey(dewey, cp)
int	dewey[];
char	*cp;
{
	int	i, n;

	for (n = 0, i = 0; i < MAXDEWEY; i++) {
		if (*cp == '\0')
			break;

		if (*cp == '.') cp++;
		if (!isdigit(*cp))
			return 0;

		dewey[n++] = strtol(cp, &cp, 10);
	}

	return n;
}

/*
 * Compare two dewey arrays.
 * Return -1 if `d1' represents a smaller value than `d2'.
 * Return  1 if `d1' represents a greater value than `d2'.
 * Return  0 if equal.
 */
int
cmpndewey(d1, n1, d2, n2)
int	d1[], d2[];
int	n1, n2;
{
	int	i;

	for (i = 0; i < n1 && i < n2; i++) {
		if (d1[i] < d2[i])
			return -1;
		if (d1[i] > d2[i])
			return 1;
	}

	if (n1 == n2)
		return 0;

	if (i == n1)
		return -1;

	if (i == n2)
		return 1;
}

/*
 * Search directories for a shared library matching the given
 * major and minor version numbers.
 *
 * MAJOR == -1 && MINOR == -1	--> find highest version
 * MAJOR != -1 && MINOR == -1   --> find highest minor version
 * MAJOR == -1 && MINOR != -1   --> invalid
 * MAJOR != -1 && MINOR != -1   --> find highest micro version
 */

/* Not interested in devices right now... */
#undef major
#undef minor

char *
findshlib(name, majorp, minorp, do_dot_a)
char	*name;
int	*majorp, *minorp;
int	do_dot_a;
{
	int		dewey[MAXDEWEY];
	int		ndewey;
	int		tmp[MAXDEWEY];
	int		i;
	int		len;
	char		*lname, *path = NULL;
	int		major = *majorp, minor = *minorp;

	len = strlen(name);
	lname = (char *)alloca(len + sizeof("lib"));
	sprintf(lname, "lib%s", name);
	len += 3;

	ndewey = 0;

	for (i = 0; i < n_search_dirs; i++) {
		DIR		*dd = opendir(search_dirs[i]);
		struct dirent	*dp;
		int		found_dot_a = 0;

		if (dd == NULL)
			continue;

		while ((dp = readdir(dd)) != NULL) {
			int	n, j, might_take_it = 0;

			if (do_dot_a && path == NULL &&
					dp->d_namlen == len + 2 &&
					strncmp(dp->d_name, lname, len) == 0 &&
					(dp->d_name+len)[0] == '.' &&
					(dp->d_name+len)[1] == 'a') {

				path = concat(search_dirs[i], "/", dp->d_name);
				found_dot_a = 1;
			}

			if (dp->d_namlen < len + 4)
				continue;
			if (strncmp(dp->d_name, lname, len) != 0)
				continue;
			if (strncmp(dp->d_name+len, ".so.", 4) != 0)
				continue;

			if ((n = getdewey(tmp, dp->d_name+len+4)) == 0)
				continue;

			if (major != -1 && found_dot_a) { /* XXX */
				free(path);
				path = NULL;
				found_dot_a = 0;
			}

			if (major == -1 && minor == -1) {
				might_take_it = 1;
			} else if (major != -1 && minor == -1) {
				if (tmp[0] == major)
					might_take_it = 1;
			} else if (major != -1 && minor != -1) {
				if (tmp[0] == major)
					if (n == 1 || tmp[1] >= minor)
						might_take_it = 1;
			}

			if (!might_take_it)
				continue;

			if (cmpndewey(tmp, n, dewey, ndewey) <= 0)
				continue;

			/* We have a better version */
			if (path)
				free(path);
			path = concat(search_dirs[i], "/", dp->d_name);
			found_dot_a = 0;
			bcopy(tmp, dewey, sizeof(dewey));
			ndewey = n;
			*majorp = dewey[0];
			*minorp = dewey[1];
		}
		closedir(dd);

		if (found_dot_a)
			/*
			 * There's a .a archive here.
			 */
			return path;
	}

	return path;
}
