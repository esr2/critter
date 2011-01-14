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

int main(void) {
	if(newfile("test.c")) {
		beginningOfProgram();
		if (!yyparse()) {
			/* Success */
			return 0;
		} else {
			/* Failure */
			return 1;
		}
	}
}



