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
#include "dynarray.h"
#include <assert.h>
#include <stdlib.h>

static DynArray_T commentLocations;
static DynArray_T commentTexts;

/**
 * Called at the beginning of each file before parsing begins.
 */
void beginningOfFile(char* filename) {
	
}

void freeLocations(void* element, void* extra) { 
	assert(element != NULL);
	YYLTYPE *location = (YYLTYPE *)element;
	if (location->filename != NULL) { free(location->filename); }
	free(element); 
}
void freeComments(void* element, void* extra) { free(element); }

/**
 * Called at the end of each file. Location.first_line = last_line.
 */
void endOfFile(YYLTYPE location) {
	/* Make one last call to tooManyParameters to make sure that if the last
	 * function has an error, the error actually gets displayed. */
	location.first_line++;
	location.last_line++;
	tooManyParameters(location);
	
}

/**
 * Called at the beginning of the program execution before parsing begins.
 */
void beginningOfProgram(char* filename) {
	commentTexts = DynArray_new(0);
	commentLocations = DynArray_new(0);
}

/**
 * Called at the end of the program execution.
 */
void endOfProgram(YYLTYPE location) {
	assert(commentTexts != NULL);
	assert(commentLocations != NULL);
	// Free the comment arrays
	DynArray_map(commentLocations, freeLocations, NULL);
	DynArray_free(commentLocations);
	DynArray_map(commentTexts, freeComments, NULL);
	DynArray_free(commentTexts);
}

/**
 * Checks if there are braces surrounding an if statement.
 */
void ifHasBraces(enum tree_code statementValue, YYLTYPE location) {
	if (statementValue != COMPOUND_STATEMENT) {
		lyyerror(location, "Please use braces after all if statements");
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
 * Collects the last comment. Progress should equal 1 if true, 0 if in the 
 * middle of a comment, and -1 if starting a comment.
 */
void registerComment(char* text, YYLTYPE location, int progress) {
	//lyyerror(location, "registering comment");
	enum PROGRESS {
		END = 1,
		MIDDLE = 0,
		BEGIN = -1
	};
	
#define MAX_COMMENT_LENGTH 2000
	static char lastCommentText[MAX_COMMENT_LENGTH];
	static YYLTYPE lastCommentLocation;

	static int hasEnded = 0;
	
	if (!text) {
		// We've encountered an error that should never happen, for now just return
		return;
	}
	
	if (progress == END) {
		lastCommentLocation.last_line = location.last_line;
		lastCommentLocation.last_column = location.last_column;
		
		// add the latest comment to the arrays
		char *text = malloc(sizeof(char) * strlen(lastCommentText));
		strcpy(text, lastCommentText);
		YYLTYPE *loc = malloc(sizeof(YYLTYPE));
		loc->filename = malloc(sizeof(char) * strlen(lastCommentLocation.filename));
		strcpy(loc->filename, lastCommentLocation.filename);
		loc->first_line = lastCommentLocation.first_line;
		loc->first_column = lastCommentLocation.first_column;
		loc->last_line = lastCommentLocation.last_line;
		loc->last_column = lastCommentLocation.last_column;
		
		DynArray_add(commentTexts, text);
		DynArray_add(commentLocations, loc);
	} else if (progress == MIDDLE) {
		size_t size = MAX_COMMENT_LENGTH - strlen(lastCommentText);
		strncat(lastCommentText, text, size);
		
	} else if (progress == BEGIN) {
		int i;
		for (i = 0; i < MAX_COMMENT_LENGTH; i++) {
			lastCommentText[i] = '\0';
		}
		
		lastCommentLocation = location;
	} else {
		// PROFESSOR ERROR!!!
	}
	
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
		int distance = abs(commentLocation->last_line - functionLocation->first_line);
		if (distance <= COMPARE_DISTANCE) {
			return 0;
		}
		// TRY checking function last line to comment first line to find comments after
		// but very likely that you'll find some inferior comment
	}
	
	return 1;
}

/**
 * Checks for comments before functions.
 */
void checkForComment(YYLTYPE location) {
	int index = DynArray_backwardsSearch(commentLocations, &location, compareLocations);
	if (index == -1) {
		// comment not found
	} else {
		printf("comment is %s\n", (char*)DynArray_get(commentTexts, index));
	}
}