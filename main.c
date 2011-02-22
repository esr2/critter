/*
 *  main.c
 *  
 *
 *  Created by Erin Rosenbaum on 11/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "lex.yy.c"
#include <stdio.h>

int main(int argc, char* argv[]) {
	int failures = 0;
	int i;
	
	if (argc < 2) {
		fprintf(stderr, "Usage %s <file> \n", argv[0]);
		return 1;
	}
	
	/* go backwards because will be popped off in LIFO and we want to mimic
	 * the order that the files were passed in in. */
	h_beginProgram();
	
	for (i = argc-1; i > 0; i--) {
		if(!newfile(argv[i])) {
			failures++;
		}
	}
	
	if (failures != (argc - 1)) {
		if (!yyparse()) {
			/* Success */
			return 0;
		} else {
			/* Failure */
			return 1;
		}
	}
	
	/* Only get here if we didn't parse -- ie the number of failed files
	 * was equal to the number of files passed in. */
	YYLTYPE loc = {0, 0, 0, 0, NULL};
	h_endProgram(loc);
	return 1;
}



