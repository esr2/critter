/*
 *  checks.c
 *  
 *
 *  Created by Erin Rosenbaum on 11/22/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "checks.h"
#include <stdio.h>
#include <strings.h>
#include <assert.h>
#include <stdlib.h>
#include "dynarray.h"
#include "comments.h"

/**
 * Check if the file is above a maximum length
 */
void isFileTooLong(YYLTYPE location) {
	int MAX_FILE_LENGTH = 100;
	
	if (location.first_line > MAX_FILE_LENGTH) {
		lyyerror(location, "File is too long");
	}
}

/**
 * Checks if there are braces surrounding an if statement.
 */
void hasBraces(YYLTYPE location, int progress) {
	static int foundCompound = 0;
	static int started = 0;
	
	switch (progress) {
		case BEGINNING:
			foundCompound = 0;
			started = 1;
			break;
		case MIDDLE:
			foundCompound = 1;
			break;
		case END:
			if (!foundCompound && started) {
				lyyerror(location, "Please use braces after all if, for and while statements");
			}
			started = 0;
			break;
		default:
			break;
	}
	
}

/**
 * Checks if a function is too long
 */
void isFunctionTooLong(YYLTYPE location) {
	int MAX_FUNCTION_LENGTH = 100;
	
	if (location.last_line - location.first_line + 1 >= MAX_FUNCTION_LENGTH) {
		lyyerror(location, "Function is too long");
	}
}

/**
 * Checks if there are too many parameters in the function declaration.
 */
void tooManyParameters(YYLTYPE location, int progress) {
	int MAX_NUM_PARAMETERS = 7;
	
	static int numParameters = -1;
	static int nestedListLevel = -1; /* should = 0 normally */
	
	switch (progress) {
		case BEGINNING:
			nestedListLevel++;
			if (nestedListLevel == 0) { 
				numParameters = 0; 
			}
			break;
		case MIDDLE: 
			if (nestedListLevel == 0) {
				numParameters++;
			}
			break;
		case END:
			if (numParameters >= MAX_NUM_PARAMETERS && nestedListLevel == 0) {
				char string[200];
				sprintf(string,
						"Please use less than %d function parameters, you used %d",
						MAX_NUM_PARAMETERS, numParameters);
				lyyerror(location, string);
			}
			nestedListLevel--;
			break;
		default:
			break;
	}
}

/**
 * Throws an error on C++ style single line comments.
 */
void CPlusPlusComments(YYLTYPE location) {
	lyyerror(location, "Don't use C++ style comments");
}

/**
 * Checks for comments before functions.
 */
void checkForComment(YYLTYPE location) {
	char* text = comment_getCommentCloseTo(location, 5);

	if (text == NULL) {
		// comment not found
		lyyerror(location, "Please include a descriptive comment above each function");
	} else {
		//printf("comment is %s\n", text);
	}
}

/**
 * Checks if each switch statment has a default case.
 */
void switchHasDefault(YYLTYPE location, int progress) {
	static int started = 0;
	static int found = 0;
	
	switch (progress) {
		case BEGINNING:
			started = 1;
			found = 0;
			break;
		case MIDDLE:
			found = 1;
			break;
		case END:
			if (!found && started) {
				lyyerror(location, "Always include a default in switch statements");
			}
			started = 0;
			break;
		default: break;
	}
}

/**
 * Checks if each switch case statment has a break statement.
 */
void switchCasesHaveBreaks(YYLTYPE location, int progress, int isCase) {
	static int numCases = 0;
	static int numBreaks = 0;

	
	switch (progress) {
		case BEGINNING:
			numCases = 0;
			numBreaks = 0;
			break;
		case MIDDLE:
			switch (isCase) {
				case 0: /* break */
					numBreaks++;
					break;
				case 1: /* case */
					numCases++;
				default:
					break;
			}
			break;
		case END:
			if (numCases != numBreaks) {
				int missing = numCases - numBreaks;
				char errorMsg[200];
				sprintf(errorMsg, "Each case in a switch statement should have a break statement, you're missing %d",
						missing);
				lyyerror(location, errorMsg);
			}
			break;
		default:
			break;
	}
}

/**
 * Checks whether a region of code (i.e. a compound statement) nests too deeply.
 */
void tooDeeplyNested(YYLTYPE location, int progress) {
	int MAX_NESTING_LEVEL = 2;
	static int nestedLevel = -1;
	static int lastLevel = -1;
	
	lastLevel = nestedLevel;
	
	switch (progress) {
		case BEGINNING:
			nestedLevel++;
			break;
		case END:
			/* complain only at the highest point which is too deep -- i.e. 
			 * avoid complaining on each further offense/level past the max */
			if (nestedLevel == MAX_NESTING_LEVEL) {
				lyyerror(location, "This area is too deeply nested, consider refactoring");
			}
			nestedLevel--;
			break;
		default:
			break;
	}

}

/**
 * Advices using enum instead of const for declarations. Actually just checks
 * that 'const' doesn't appear inside a parameter list before outputting the error.
 */
void useEnumNotConstOrDefine(YYLTYPE location, int progress) {
	static int inParameterList = 0;
	
	switch (progress) {
		case BEGINNING:
			inParameterList = 1;
			break;
		case MIDDLE:
			if (!inParameterList) {
				//lyyerror(location, "It would be better to use enum to define integral constants");
			}
			break;
		case END:
			inParameterList = 0;
		default:
			break;
	}
}

/**
 * Error on any use of a GOTO
 */
void neverUseGotos(YYLTYPE location) {
	lyyerror(location, "Never use GOTO statements");
}