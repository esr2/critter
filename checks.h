/*
 *  checks.h
 *  
 *
 *  Created by Erin Rosenbaum on 11/22/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CHECKS_INCLUDED
#define CHECKS_INCLUDED

#include "tree.h"
#include "c.tab.h"

void hasBraces(YYLTYPE location, int progress);
void hasBracesO(YYLTYPE location, enum tree_code statementValue);
void isFunctionTooLong(YYLTYPE location);
void tooManyParameters(YYLTYPE location);
void CPlusPlusComments(YYLTYPE location);
void checkForComment(YYLTYPE location);
void switchHasDefault(YYLTYPE location, int progress);

enum {
	END = 1,
	MIDDLE = 0,
	BEGINNING = -1
};

#endif