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

void hasBraces(YYLTYPE location, enum tree_code statementValue);
void isFunctionTooLong(YYLTYPE location);
void tooManyParameters(YYLTYPE location);
void CPlusPlusComments(YYLTYPE location);
//void registerComment(YYLTYPE location, char* text, int progress);
void checkForComment(YYLTYPE location);
void switchHasDefault(YYLTYPE location, int progress);

#endif