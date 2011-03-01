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
#include "sax.h"

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
void hasBraces(YYLTYPE location) {
	if (lastCalledFunction != endCompoundStatement) {
		lyyerror(location, "Please use braces after all if, else, for and while statements");
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
	static int nestedListLevel = -1; /* should = 0 inside a non-nested parameter list */
	
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
	lyyerror(location, "Do not use C++ style comments");
}

/**
 * Checks for comments before some construct.
 */
void checkForComment(YYLTYPE location, char* construct) {
	char* text = comment_getCommentCloseTo(location, 1);

	if (text == NULL) {
		// comment not found
		char error[500];
		sprintf(error,
				"Please include a descriptive comment above each %s",
				construct);
		lyyerror(location, error);
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
 * Error on any use of a GOTO.
 */
void neverUseGotos(YYLTYPE location) {
	lyyerror(location, "Never use GOTO statements");
}

/**
 * Error if a variable name is less than a certain minimum length. 
 */
void isVariableNameTooShort(YYLTYPE location, int progress, char* identifier) {
	int MINIMUM_VARIABLE_NAME_LENGTH = 1;
	static int inDeclaration = 0;
	
	switch (progress) {
		case BEGINNING:
			inDeclaration++;
			break;
		case MIDDLE:
			if (inDeclaration && strlen(identifier) < MINIMUM_VARIABLE_NAME_LENGTH) {
				char error[500];
				sprintf(error, "Variable/function name '%s' is too short", identifier);
				lyyerror(location, error);
			}
			break;
		case END:
			inDeclaration--;
			break;
		default:
			break;
	}
}

/**
 * Error on encountering a magic number outside of a declaration (presumably, inside a
 * declaration a variable will be initialized to a magic number and then used throughout
 * the rest of the code).
 */
void isMagicNumber(YYLTYPE location, int progress, char* constant) {
	int acceptableNumbers[2] = {0, 1};
	int numAcceptable = sizeof(acceptableNumbers)/sizeof(int);
	
	static int inDeclaration = 0;
	
	switch (progress) {
		case BEGINNING:
			inDeclaration++;
			break;
		case MIDDLE:
			if (inDeclaration == 0) {
				int number = (int)strtol(constant, (char**)NULL, 0);
				int i;

				/* see if number is within the acceptableNumbers array */
				for (i = 0; i < numAcceptable; i++) {
					if (number == acceptableNumbers[i]) {
						return;
					}
				}
				
				lyyerror(location, "Do not use magic numbers");
			}
			break;
		case END:
			inDeclaration--;
			break;
		default:
			break;
	}
}

/**
 * Check that each global variable has a comment.
 */
void globalHasComment(YYLTYPE location, int progress) {
	static int inFunction = 0;
	
	switch (progress) {
		case BEGINNING:
			inFunction = 1;
			break;
		case MIDDLE:
			if (!inFunction) {
				checkForComment(location, "global variable");
			}
			break;
		case END:
			inFunction = 0;
			break;
		default:
			break;
	}
}

/**
 * Check that the loop length is less than a maximum.
 */
void isLoopTooLong(YYLTYPE location) {
	int MAX_LOOP_LENGTH = 20;
	
	if (location.last_line - location.first_line + 1 >= MAX_LOOP_LENGTH) {
		lyyerror(location, "Loop is too long, consider pulling out code into its own function");
	}
}

/**
 * Check that loops are not empty.
 */
void isLoopEmpty(YYLTYPE location, void (*beginLoop)(YYLTYPE), char* loopType) {
	if (lastCalledFunction == beginLoop) {
		char error[500];
		sprintf(error, "Do not use empty %s loops", loopType);
		lyyerror(location, error);
	}
}


/**
 * Check if compound statement is empty
 */
void isCompoundStatementEmpty(YYLTYPE location, int progress) {
	static void (*context)(YYLTYPE);
	
	switch (progress) {
		case BEGINNING:
			context = lastCalledFunction;
			break;
		case END:
			if (lastCalledFunction == beginCompoundStatement) {
				/* create a good error message */
				char *parent = NULL;
				
				if (context == beginIf) { parent = "if statements"; }
				else if (context == beginElse) { parent = "else statements"; }
				else if (context == beginFor) { parent = "for loops"; }
				else if (context == beginWhile) { parent = "while loops"; }
				else if (context == beginDoWhile) { parent = "doWhile loops"; }
				
				char error[500];
				
				if (parent) {
					sprintf(error, "Do not use empty %s", parent);
				} else {
					sprintf(error, "Do not use empty block statements");
				}
				
				lyyerror(location, error);
			}
			break;
		default:
			break;
	}
}

/**
 * Check if there are too many functions in a file.
 */
void tooManyFunctionsInFile(YYLTYPE location, int progress) {
	int MAX_FUNCTIONS_PER_FILE = 5;
	static int numFunctions = 0;
	
	switch (progress) {
		case MIDDLE:
			numFunctions++;
			break;
		case END:
			if (numFunctions > MAX_FUNCTIONS_PER_FILE) {
				lyyerror(location, "There are too many functions in this file");
			}
			numFunctions = 0;
			break;
		default:
			break;
	}
}

void checkIfElsePlacement(YYLTYPE location, int progress) {
	static int ifIsBracketed = 0;
	static int ifLastLine;
	static int hadError = 0;
	
	switch (progress) {
		case BEGINNING:
			ifIsBracketed = (lastCalledFunction == endCompoundStatement);
			ifLastLine = location.last_line;
			/* check that, if bracketed, appears on multiple lines */
			if (ifIsBracketed && ((location.last_line - location.first_line) <= 0)) {
				lyyerror(location, "When using braces with an if statement, use multiple lines");
				hadError = 1;
			}
			break;
		case END:
			/* Don't need to check that the else is adjacent because else statements
			   must come after an if statement */
			if (hadError) {
				hadError = 0;
				return;
			}
			
			if (ifIsBracketed) {
				/* else should be on same line as '}' */
				if (location.first_line != ifLastLine) {
					lyyerror(location, 
							 "Please put the else on the same line as the closing if brace");
				}
			} else {
				/* else should be on line after the if statement finishes */
				if ((location.first_line - ifLastLine) <= 0) {
					lyyerror(location, "Please put the else on the line after the if");
				}
			}
			
			hadError = 0;
			break;
		default:
			break;
	}
}