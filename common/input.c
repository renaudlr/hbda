/*
 * =====================================================================================
 *
 *       Filename:  input.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  14/09/2013 00:04:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "input.h"

/*#define DEBUG_INPUT*/

#define LINE_LENGTH 1000
#define SEP " ;\t\n\v\f\r"

cost_t ** csv2int (char * file, int * nrow, int * ncol) {
	int i = 0, j, n = 0;
	char * token;
	cost_t * tmp1 = NULL;
	cost_t ** tab = NULL; /* Attention au NULL !!! */
	char line[LINE_LENGTH];
	FILE * f = fopen(file, "rt");

	if (f==NULL) {
		fprintf(stderr, "Error opening file %s\n", file);
		exit (EXIT_FAILURE);
	}

	while (fgets(line, LINE_LENGTH, f) != NULL) {
		// Skip comment lines
		/*printf("%s\n", line);*/
		if (!(isdigit(line[0]) || line[0] == '-'))
		/*if (line[0] == '#')*/
			continue;
		tab = realloc (tab, (i+1)*sizeof (cost_t*));
		if (tab == NULL) {
			fprintf (stderr, "Insufficient memory.\n");
			(void) fclose (f);
			exit (EXIT_FAILURE);
		}
		tmp1 = NULL;
		j = 0;
		token = strtok (line, SEP);
		while (token != NULL) {
			tmp1 = realloc (tmp1, (j+1)*sizeof (cost_t));
			if (tmp1 == NULL) {
				fprintf (stderr, "Insufficient memory.\n");
				(void) fclose(f);
				exit (EXIT_FAILURE);
			}
			tmp1[j] = (cost_t) atof(token);
			j++;
			token = strtok (NULL, SEP);
		}
		tab[i] = tmp1;
		if (i==0)
			n = j;
		i++;
	}
	*nrow = i;
	*ncol = n;

#ifdef DEBUG_INPUT
	printf("file: %s, nrow: %d, ncol: %d\n", file, *nrow, *ncol);
#endif
	
	(void) fclose(f);
	return tab; 
}
