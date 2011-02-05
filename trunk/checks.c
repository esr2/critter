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
void hasBraces(YYLTYPE location, enum tree_code statementValue) {
	if (statementValue != COMPOUND_STATEMENT) {
		lyyerror(location, "Please use braces after all if, for and while statements");
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
void tooManyParameters(YYLTYPE location) {
	int MAX_NUM_PARAMETERS = 7;
	
	static int numParameters = -1;
	static YYLTYPE prevLoc;
	
	if (location.first_line != prevLoc.first_line || numParameters == -1) {
		if (numParameters != -1 && numParameters >= MAX_NUM_PARAMETERS) {
			char string[200];
			sprintf(string,
					"Please use less than %d function parameters, you used %d",
					MAX_NUM_PARAMETERS, numParameters);
			lyyerror(prevLoc, string);
		}
		
		numParameters = 1;
	}
	
	numParameters++;
	prevLoc = location;
}

/**
 * Throws an error on C++ style single line comments.
 */
void CPlusPlusComments(YYLTYPE location) {
	lyyerror(location, "Don't use C++ style comments");
}

/**
 * Compare two locations - meant to be used by Dynarray_search. Returns 0 if
 * equal and 1 if not.
 */
int compareLocations(const void *element1, const void *element2) {
	// taking advantage of how dynarray compares elements
	YYLTYPE *commentLocation = (YYLTYPE*)element1;
	YYLTYPE *functionLocation = (YYLTYPE*)element2;
		
	assert(commentLocation != NULL);
	assert(functionLocation != NULL);
	
	assert(commentLocation->filename != NULL);
	assert(functionLocation->filename != NULL);
	
	int COMPARE_DISTANCE = 5;
	
	if (strcmp(commentLocation->filename, functionLocation->filename) == 0) {
		// Comment before function call
		int distance = functionLocation->first_line - commentLocation->last_line;
		if (distance <= COMPARE_DISTANCE && distance > 0) {
			return 0;
		}
		
		// Comment inside function body
		distance = commentLocation->first_line - functionLocation->first_line;
		if (distance <= COMPARE_DISTANCE && distance > 0) {
			return 0;
		}
	}
	
	return 1;
}

/**
 * Checks for comments before functions.
 */
void checkForComment(YYLTYPE location) {
	int index = DynArray_search(commentLocations, &location, compareLocations);
	if (index == -1) {
		// comment not found
		lyyerror(location, "Please include a descriptive comment above each function");
	} else {
		//printf("comment is %s\n", (char*)DynArray_get(commentTexts, index));
	}
}

/**
 * Checks if each switch statment has a default case.
 */
void switchHasDefault(YYLTYPE location, int progress) {
	enum PROGRESS {
		END = 1,
		MIDDLE = 0,
		BEGIN = -1
	};
	
	static int started = 0;
	static int found = 0;
	
	switch (progress) {
		case BEGIN:
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