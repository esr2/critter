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

#include "c.tab.h"

void isFileTooLong(YYLTYPE location);
void hasBraces(YYLTYPE location, int progress);
void isFunctionTooLong(YYLTYPE location);
void tooManyParameters(YYLTYPE location, int progress);
void CPlusPlusComments(YYLTYPE location);
void checkForComment(YYLTYPE location, char* construct);
void switchHasDefault(YYLTYPE location, int progress);
void switchCasesHaveBreaks(YYLTYPE location, int progress, int isCase);
void tooDeeplyNested(YYLTYPE location, int progress);
void useEnumNotConstOrDefine(YYLTYPE location, int progress);
void neverUseGotos(YYLTYPE location);
void isVariableNameTooShort(YYLTYPE location, char* identifier);
void isMagicNumber(YYLTYPE location, int progress, char* constant);
void globalHasComment(YYLTYPE location, int progress);

enum {
	END = 1,
	MIDDLE = 0,
	BEGINNING = -1
};

#endif