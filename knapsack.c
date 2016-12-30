/* knapsack.c -- Knapsack solver.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Thu Dec 29 11:20:10 EET 2016
 * Disclaimer: (C) 2016 Luis Colorado <luiscoloradourcola@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <errno.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_TARGET	(100)
#define MAX				(64)
#define FLAG_VERBOSE	(1 << 0)
#define FLAG_FIRST      (1 << 1)

#define F(x) "%s:%d:%s: " x, __FILE__, __LINE__, __func__

struct node {
	size_t	number;
	int		value;
};

int flags = 0;
int target = DEFAULT_TARGET;
int numbers[MAX];
size_t n_numbers = 0;

struct node nodes[MAX];
size_t n_nodes = 0;
int factors[MAX];
int sols = 0;

char *fmt_bw[] = {
	" %d ",
	"[%d]",
};

char *fmt_color[] = {
	" %d",
	" \033[1;36m%d\033[m",
};

char **fmt = fmt_bw;

static void add(int n);
static int comp_int(const void *_a, const void *_b);
static ssize_t print(int n);
static void search(int sum, int n);
static void do_usage(void);

int main(int argc, char **argv)
{
	int opt;
	struct node *p = NULL;
	int i;

	while ((opt = getopt(argc, argv, "t:vch?1")) != EOF) {
		switch(opt) {
		case 't': target = atoi(optarg); break;
		case 'v': flags |= FLAG_VERBOSE; break;
        case '1': flags |= FLAG_FIRST; break;
		case 'c': fmt = fmt_color; break;
		case 'h': case '?':
			do_usage();
			exit(EXIT_SUCCESS);
		} /* switch */
	} /* while */

	argc -= optind;
	argv += optind;

	if (argc) {
		int i;
		for (i = 0; i < argc; i++)
			add(atoi(argv[i]));
	} else {
		/* read from stdin */
		int n;
		while (scanf("%d", &n) == 1)
			add(n);
	} /* if */

	qsort(numbers, n_numbers, sizeof(numbers[0]), comp_int);

	for (i = 0; i < n_numbers; i++) {
		int n = numbers[i];
		if (!p || p->value != n) {
			p = nodes + n_nodes++;
			p->value = n;
			p->number = 0;
		}
		p->number++;
	} /* for */

	search(0, 0);

    printf(F("%d solutions\n"), sols);

	exit(EXIT_SUCCESS);
} /* main */

static void search(int sum, int n)
{
	int i;
	if (sum == target) print(n);
	if (sum >= target) return;
	if (n > n_nodes) return;
	/* sum < target, n < n_nodes */
	for (i = 0; i <= nodes[n].number; i++) {
		factors[n] = i;
		search(sum, n+1);
		sum += nodes[n].value;
	}
} /* search */

static void add(int n)
{
	if (n_numbers >= MAX) {
		fprintf(stderr,
			F("MAX(%d) overflow\n"),
			MAX);
		exit(EXIT_FAILURE);
	} /* if */
	numbers[n_numbers++] = n;
} /* add */

static int comp_int(const void *_a, const void *_b)
{
	const struct node *a = _a, *b = _b;
	return b->number - a->number;
} /* comp_int */

static ssize_t print(int n)
{
	ssize_t res = 0;
	int i, j;
	if (!sols++ || (flags & FLAG_VERBOSE)) {
		res += printf(F("SOL(%d): %d = "), sols, target);
		for (i = 0; i < n_nodes; i++) {
			for (j = 0; j < nodes[i].number; j++) {
				res += printf(fmt[(i < n && j < factors[i]) & 1],
							  nodes[i].value);
			}
		} /* for */
		res += printf("\n");
	} /* if */
    if (flags & FLAG_FIRST)
        exit(EXIT_SUCCESS);
	return res;
} /* print */

static void do_usage(void)
{
	printf(
		"Usage: knapsack [ options ... ] [ number ... ]\n"
		"Options:\n"
		"  -v  Verbose: print all solutions, and not only the first one.\n"
		"  -c  Color output: print selected numbers in bright cyan color.\n"
		"  -h, -? Print this help screen.\n"
        "  -1  print only the first solution without searching for more\n"
		"  -t number: target number to sum.  Print solutions adding up to\n"
		"	  number.\n"
		"  number: list of numbers.  If empty, the list of numbers is read\n"
		"	  from stdin.\n");
} /* do_usage */
