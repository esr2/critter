/*
 *  checks.h
 *  
 *
 *  Created by Erin Rosenbaum on 11/22/10.
 *  Copyright 2010 CritTer. All rights reserved.
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
	REGISTER_PARAM,
	END_PARAM_LIST,
	BEGIN_FUNCTION_BODY,
	FOUND_IDENTIFIER,
	FOUND_POINTER,
	END_STATEMENT,
	RETURNING,
	END_FUNCTION
};

void isFileTooLong(YYLTYPE location);
void hasBraces(YYLTYPE location, char* construct);
void isFunctionTooLongByLines(YYLTYPE location);
void isFunctionTooLongByStatements(YYLTYPE location, int progress);
void tooManyParameters(YYLTYPE location, int progress);
void neverUseCPlusPlusComments(YYLTYPE location);
void fileHasComment(YYLTYPE location);
void hasComment(YYLTYPE location, char* construct);
void switchHasDefault(YYLTYPE location, int progress);
void switchCasesHaveBreaks(YYLTYPE location, int progress, int isCase);
void isTooDeeplyNested(YYLTYPE location, int progress);
void useEnumNotDefine(YYLTYPE location);
void neverUseGotos(YYLTYPE location);
void isVariableNameTooShort(YYLTYPE location, int progress, char* identifier);
void isMagicNumber(YYLTYPE location, int progress, char* constant);
void globalHasComment(YYLTYPE location, int progress);
void isLoopTooLong(YYLTYPE location);
void isCompoundStatementEmpty(YYLTYPE location, int progress);
void tooManyFunctionsInFile(YYLTYPE location, int progress);
void isIfElsePlacementValid(YYLTYPE location, int progress);
void isFunctionCommentValid(YYLTYPE location, enum commandType command, char* text);
void arePointerParametersValidated(YYLTYPE location, enum commandType command, char* identifier);
void doFunctionsHaveCommonPrefix(YYLTYPE location, int progress, char* identifier);
void functionHasEnoughLocalComments(YYLTYPE location, int progress, int isComment);
void structFieldsHaveComments(YYLTYPE location, int progress);

#endif