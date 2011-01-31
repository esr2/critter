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

void beginningOfFile(char* filename);
void endOfFile(YYLTYPE location);
void beginningOfProgram();
void endOfProgram(YYLTYPE location);

void ifHasBraces(enum tree_code statementValue, YYLTYPE location);
void isFunctionTooLong(YYLTYPE location);
void tooManyParameters(YYLTYPE location);
void CPlusPlusComments(YYLTYPE location);
void registerComment(char* text, YYLTYPE location, int progress);

#endif