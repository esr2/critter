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
	yyin = fopen("test.c", "r");
	if (yyin == NULL) {
		printf("FILE FAIL\n");
		return;
	} else {
		printf("=================\n");
	}
	
	if (!yyparse()) {
		printf("\n\nWIN\n");
		return 0;
	} else {
		return 1;
	}
}



