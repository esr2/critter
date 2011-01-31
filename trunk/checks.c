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
/**
 * Called at the beginning of each file before parsing begins.
 */
void beginningOfFile(char* filename) {
	
}

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

}

/**
 * Called at the end of the program execution.
 */
void endOfProgram(YYLTYPE location) {

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
	enum PROGRESS {
		END = 1,
		MIDDLE = 0,
		BEGIN = -1
	};
#define MAX_COMMENT_LENGTH 2000
	
	static int hasEnded = 0;
	static YYLTYPE prevLoc;
	static char fullComment[MAX_COMMENT_LENGTH];
	
	if (!text) {
		// We've encountered an error that should never happen, for now just return
		return;
	}
	if (!prevLoc.first_line) {prevLoc = location;}
	
	if (progress == END) {
		prevLoc.last_line = location.last_line;
		prevLoc.last_column = location.last_column;
		
	} else if (progress == MIDDLE) {
		size_t size = MAX_COMMENT_LENGTH - strlen(fullComment);
		strncat(fullComment, text, size);
		
	} else if (progress == BEGIN) {
		int i;
		for (i = 0; i < MAX_COMMENT_LENGTH; i++) {
			fullComment[i] = '\0';
		}
		
		prevLoc.first_line = location.first_line;
		prevLoc.first_column = location.first_column;
		prevLoc.filename = location.filename;
	} else {
		// PROFESSOR ERROR!!!
	}
	
}