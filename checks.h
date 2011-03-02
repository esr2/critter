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

enum {
	BEGINNING = -1,
	MIDDLE = 0,
	END = 1,
};

enum commandType {
	BEGIN_FUNCTION,
	BEGIN_PARAM_LIST,
	FOUND_IDENTIFIER,
	END_PARAM_LIST,
	RETURNING,
	END_FUNCTION
};

void isFileTooLong(YYLTYPE location);
void hasBraces(YYLTYPE location);
void isFunctionTooLongByLines(YYLTYPE location);
void isFunctionTooLongByStatements(YYLTYPE location, int progress);
void tooManyParameters(YYLTYPE location, int progress);
void CPlusPlusComments(YYLTYPE location);
void checkForComment(YYLTYPE location, char* construct);
void switchHasDefault(YYLTYPE location, int progress);
void switchCasesHaveBreaks(YYLTYPE location, int progress, int isCase);
void tooDeeplyNested(YYLTYPE location, int progress);
void useEnumNotConstOrDefine(YYLTYPE location, int progress);
void neverUseGotos(YYLTYPE location);
void isVariableNameTooShort(YYLTYPE location, int progress, char* identifier);
void isMagicNumber(YYLTYPE location, int progress, char* constant);
void globalHasComment(YYLTYPE location, int progress);
void isLoopTooLong(YYLTYPE location);
void isLoopEmpty(YYLTYPE location, void (*beginLoop)(YYLTYPE), char* loopType);
void isCompoundStatementEmpty(YYLTYPE location, int progress);
void tooManyFunctionsInFile(YYLTYPE location, int progress);
void checkIfElsePlacement(YYLTYPE location, int progress);
void validateComment(YYLTYPE location, enum commandType command, char* text);

#endif