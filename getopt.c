/* A quick and dirty getopt implementation for Visual Studio.
*
* Released under terms of the MIT License:
*
* The MIT License (MIT)
* Copyright (c) 2016, Ron Parker
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/
#include <stdio.h>
#include <string.h>
#include "unistd.h"

char *optarg = NULL;

int optind = 0; /* Next argument to process. Zero restarts. */
int opterr = 1; /* Print a message if there's an error. */
int optopt;

static int nextchar = 0; /* The next character to process in argv[optind]. */

#define DONE -1
#define UNRECOGNIZED_OPTION '?'
#define MISSING_ARGUMENT ':'

// If arg begins with '-' and is not exactly "-" or "--", it is an option argument.
#define isopt(arg) ((arg)[0] == '-' && (arg)[1] != '\0' && \
				    !((arg)[1] == '-' && (arg)[2] == '\0'))

int getopt(int argc, char * const argv[], const char *optstring)
{
	int			has_arg, is_optional;
	const char *match;

	/* Check for a reset. */
	if (optind == 0)
	{
		optind = 1;
		nextchar = 0;
	}

	for (;;)
	{
		if (!argv[optind] || !isopt(argv[optind]))
			return DONE;

		if (argv[optind][nextchar] != '\0')
			break;

		optind++;
		nextchar = 0;
	}

	if (nextchar == 0)
		nextchar = 1;
	
	optopt = argv[optind][nextchar];
	match = strchr(optstring, optopt);

	/* Advance to the next character or argument. */
	nextchar++;
	if (argv[optind][nextchar] == '\0')
	{
		optind++;
		nextchar = 0;
	}

	if (!match)
	{
		if (opterr)
			fprintf(stderr, "Option '%c' is not recognized.", optopt);

		return UNRECOGNIZED_OPTION;
	}

	/* Should the option have an argument? */
	has_arg = match[1] == ':';
	if (has_arg)
	{
		/* Options in optstring followed by two colons (::) are optional. */
		is_optional = has_arg && match[2] == ':';

		/* If we've depleted this argument, optind has already been advanced,
			* and nextarg is zero, so this is safe even at the end of all
			* arguments where argv[optind] is NULL.
			*/
		optarg = argv[optind] + nextchar;

		/* Advance the pointers in preparation for the next option. */
		optind++;
		nextchar = 0;

		/* I'm interpreting the man page as only returning ':' if optstring
		 * begins with ':' and the argument is optional.  Otherwise, there is no
		 * reason for using "::" in optstring to indicate optionality.
		 */
		if (optarg == NULL)
			return (optstring[0] == ':' && is_optional) ? MISSING_ARGUMENT : UNRECOGNIZED_OPTION;
	}

	return optopt;
}