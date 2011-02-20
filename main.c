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
	if (argc != 2) {
		fprintf(stderr, "Usage %s <file> \n", argv[0]);
		return 1;
	}
	
	if(newfile(argv[1])) {
		h_beginProgram();
		if (!yyparse()) {
			/* Success */
			return 0;
		} else {
			/* Failure */
			return 1;
		}
	}
	
	/* should never get here */
	return 1;
}



