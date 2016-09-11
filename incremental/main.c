
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

#include "dlist.h"
#include "macros.h"
#include "output.h"
#include "input.h"

#include "search_zone.h"

static FILE * F_summary_file = NULL;
static int nobj, npoint;

// {{{ Functions
void help (char * cmd) {
	fprintf(stderr,
"Usage: %s [options] <instance file>\n"
"\n"
"Computes the hypervolume of the provided instance.\n"
"\n"
#ifdef MAXIMIZE
"Note: MAXIMIZATION version.\n"
#else
"Note: MINIMIZATION version.\n"
#endif
"\n"
"Options:\n"
"  -S <file> output summary to <file>\n"
"  -a append output to existing file\n"
"  -n <n> number of runs (results averaged)\n"
#ifdef MAXIMIZE
"  -O use the null vector as reference point\n"
#else
"  -O use the all-ones vector as reference point\n"
#endif
, cmd);
/*\t-D <file> output detailed summary to <file>\n\*/
/*\t-s <n> output detailed summary every n points\n\*/
	return;
}

int vcomp (const void * v1, const void * v2, void * j) {
	const cost_t * iv1 = * ((const cost_t **) v1), * iv2 = *((const cost_t **) v2);
	int * ij = (int*) j;
	if (better_pt (iv1[*ij], iv2[*ij], iv1, iv2))
		return -1;
	else if (iv1[*ij] == iv2[*ij])
		return 0;
	else
		return 1;
}

/*  TODO : F_detailed_file */
static void hdl (int sig, siginfo_t *siginfo, void *context) {
	/*if (sig == SIGTERM || sig == SIGINT) {*/
		if (F_summary_file != NULL) {
			fprintf(F_summary_file, "NA NA %d %d NA\n", npoint, nobj);
			fclose (F_summary_file);
		}
	/*}*/
	exit (EXIT_FAILURE);
}
// }}}

int main (int argc, char ** argv) {
	int c;
	struct stat     statbuf;

	cost_t ** points = NULL;

	char * summary_file            = NULL;
	bool append = false;
	bool summary_file_exists = false;
	int nruns = 1;
	bool null_one_bounds = false;

	struct sigaction act;
	memset (&act, '\0', sizeof(act));
	// }}}


	/*{{{ parse arguments*/
	opterr = 0;
	char * options = "hS:n:aO";

	while ((c = getopt (argc, argv, options)) != -1)
		switch (c) {
			case 'h':
				help (argv[0]);
				exit (EXIT_SUCCESS);
			case 'S':
				summary_file = optarg;
				break;
			case 'a':
				append = true;
				break;
			/*case 'D':
				detailed_file = optarg;
				break;*/
			/*case 's':
				output_every = atoi (optarg);
				break;*/
			case 'n':
				nruns = atoi (optarg);
				break;
			case 'O':
				null_one_bounds = true;
				break;
			case '?':
				if (optopt == 's' || optopt == 'S' || optopt == 'D')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				exit (EXIT_FAILURE);
			default:
				abort ();
		}

	if (optind == argc) {
		help (argv[0]);
		exit (EXIT_FAILURE);
	}
	/*}}} parse arguments*/
	
	/* Read the instance file */
	points = csv2int (argv[argc-1], & npoint, & nobj);
	assert (points != NULL);

	act.sa_sigaction = &hdl;
	act.sa_flags = SA_SIGINFO;

	if (summary_file != NULL) {
		if (append) {
			summary_file_exists = stat(summary_file, &statbuf) == 0 ? true : false;
			F_summary_file = fopen (summary_file, "a");
		}
		else {
			F_summary_file = fopen (summary_file, "w");
		}
		assert (F_summary_file != NULL);
		if (!append || !summary_file_exists) {
			fprintf (F_summary_file, "\"total_time\" \"total_mem\" \"n\" \"d\" \"hypervolume\"\n");
		}
	}

	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGKILL, &act, NULL);
	sigaction(SIGXCPU, &act, NULL);

	cost_t * lb;
	cost_t * ub; 

	if (null_one_bounds) {
		lb = malloc (nobj * sizeof (cost_t));
		ub = malloc (nobj * sizeof (cost_t));
		for (int j = 0; j<nobj; j++) {
			lb[j] = unit_cube_lb;
			ub[j] = unit_cube_ub;
		}
		for (int i = 1; i < npoint; i++) {
			for (int j = 0; j<nobj; j++) {
				if (worse (points[i][j], ub[j]) || better (points[i][j], lb[j])) {
					printf("%f %f %f\n", points[i][j], ub[j], lb[j]);
					fprintf(stderr, "Error: With '-O' option, all points should have their coordinates in [0,1].\n");
					exit (EXIT_FAILURE);
				}
			}
		}
	}
	else {
		lb = memcpy_or_null (points[0], nobj * sizeof (cost_t));
		ub = memcpy_or_null (points[0], nobj * sizeof (cost_t));
		for (int i = 1; i < npoint; i++) {
			for (int j = 0; j<nobj; j++) {
				if (worse (points[i][j], ub[j]))
					ub[j] = points[i][j];
				if (better (points[i][j], lb[j]))
					lb[j] = points[i][j];
			}
		}
	}

	printf("Info: Number of points: %d\n"
			   "Info: Dimension: %d\n", npoint, nobj);

#ifdef MAXIMIZE
	printf("Info: Assuming MAXIMIZATION.\n");
#else
	printf("Info: Assuming MINIMIZATION.\n");
#endif

	printf("Info: Using initial upper bound ");
	println_cost_tab (ub, nobj);
	printf("Info: Using initial lower bound ");
	println_cost_tab (lb, nobj);

	if (nruns > 1)
		printf("Info: Performing %d runs.\n", nruns);

	sz_init (nobj, lb, ub);
	dlist_t * sz_list = sz_list_new ();
	
	int i, k;
	double volume = 0;
	struct timeval tv1, tv2;
	struct rusage ru_before, ru_after;
	double totaltime;
	getrusage (RUSAGE_SELF, &ru_before);
	for (k = 0; k < nruns; k++) {
		if (nruns > 1) {
			printf("\rInfo: Run #%d", k+1);
			fflush (stdout);
		}
		for (i=0; i<npoint; i++) {
			if (k == 0)
				volume += sz_update (&sz_list, points[i]);
			else
				(void) sz_update (&sz_list, points[i]);
		}
		if (nruns > 1 && k < nruns-1) {
			sz_list_free_and_null (&sz_list);
			sz_list = sz_list_new ();
		}
	}
	getrusage (RUSAGE_SELF, &ru_after);
	tv1 = ru_before.ru_utime;
	tv2 = ru_after.ru_utime;
	totaltime = tv2.tv_sec + tv2.tv_usec * 1e-6 - tv1.tv_sec - tv1.tv_usec * 1e-6;

	// Summary...
	int nlub = dlist_length (sz_list);
	printf("\nNumber of remaining local upper bounds: %d\n", nlub);
	if (nruns > 1)
		printf("\n");
	printf("Elapsed time: %f seconds (average over %d runs: %f).\n", totaltime, nruns, totaltime/(double)nruns);
	double maxmem;
	maxmem = ru_after.ru_maxrss/(double) 1000; /* MB */
	printf("Total memory used (getrusage): %.2fMB\n", maxmem);
  printf("Hypervolume: %e\n", volume);

	if (summary_file != NULL) {
		fprintf (F_summary_file, "%e %e %d %d %e\n", 
				totaltime/(double)nruns,
				maxmem,
				npoint, 
				nobj, 
				volume
				);
		(void) fclose (F_summary_file);
	}

	exit (EXIT_SUCCESS);
}

// vim:foldmethod=marker:
