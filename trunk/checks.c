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
#include "locations.h"

/**
 * Check if the file is above a maximum length
 */
void isFileTooLong(YYLTYPE location) {
	int MAX_FILE_LENGTH = 200;
	
	if (location.first_line > MAX_FILE_LENGTH) {
		flyyerror(ERROR_NORMAL,
				  location, 
				  "File is too long, should be less than %d lines",
				  MAX_FILE_LENGTH);
	}
}

/**
 * Checks if there are braces surrounding an if statement.
 */
void hasBraces(YYLTYPE location, char* construct) {
	if (lastCalled_get() != endCompoundStatement) {
		if (construct == "else" && (lastCalled_get() == endElse ||
									lastCalled_get() == endIf)) {
			/* at the end of an else if [else] */
			return;
		}
		
		flyyerror(ERROR_NORMAL,
				  location, 
				  "Please use braces after all %s statements",
				  construct);
	}
	
}

/**
 * Checks if a function is too long by it's line count
 */
void isFunctionTooLongByLines(YYLTYPE location) {
	int MAX_FUNCTION_LENGTH = 55;
	
	if (location.last_line - location.first_line + 1 >= MAX_FUNCTION_LENGTH) {
		flyyerror(ERROR_NORMAL,
				  location, 
				  "Function is too long by line count, should be less than %d lines",
				  MAX_FUNCTION_LENGTH);
	}
}

/**
 * Checks if a function is too long by it's statement count
 */
void isFunctionTooLongByStatements(YYLTYPE location, int progress) {
	int MAX_FUNCTION_LENGTH = 50;
	static int statementCount = 0;
	
	switch (progress) {
		case BEGINNING:
			statementCount = 0;
			break;
		case MIDDLE:
			statementCount++;
			break;
		case END:
			if (statementCount >= MAX_FUNCTION_LENGTH) {
				flyyerror(ERROR_NORMAL,
						  location, 
						  "Function is too long by statement count, should be less than %d statements",
						  MAX_FUNCTION_LENGTH);
			}
			break;
		default:
			break;
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
				flyyerror(ERROR_HIGH,
						  location,
						  "Please use less than %d function parameters, you used %d",
						  MAX_NUM_PARAMETERS, numParameters);
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
	lyyerror(ERROR_NORMAL, location, "Do not use C++ style comments");
}

/**
 * Checks for comments before some construct.
 */
void checkForComment(YYLTYPE location, char* construct) {
	char* text = comment_getCommentAbove(location, 1);

	if (text == NULL) {
		// comment not found
		flyyerror(ERROR_HIGH,
				  location,
				  "Please include a descriptive comment above each %s",
				  construct);
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
				lyyerror(ERROR_HIGH, location, "Always include a default in switch statements");
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
				flyyerror(ERROR_HIGH,
						  location, 
						  "Each case/default in a switch statement should have a break statement, you're missing %d",
						  missing);
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
	int MAX_NESTING_LEVEL = 3; /* Acceptable is function, loop, if.
								  Anything after is too much */
								
	/* Statement inside just a function is at level 0 */
	static int nestedLevel = -1;
	
	switch (progress) {
		case BEGINNING:
			nestedLevel++;
			break;
		case END:
			/* complain only at the highest point which is too deep -- i.e. 
			 * avoid complaining on each further offense/level past the max */
			if (nestedLevel == MAX_NESTING_LEVEL) {
				lyyerror(ERROR_HIGH, location, "This area is too deeply nested, consider refactoring");
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
				//lyyerror(ERROR_NORMAL, location, "It would be better to use enum to define integral constants");
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
	lyyerror(ERROR_HIGH, location, "Never use GOTO statements");
}

/**
 * Error if a variable name is less than a certain minimum length. 
 */
void isVariableNameTooShort(YYLTYPE location, int progress, char* identifier) {
	int MINIMUM_VARIABLE_NAME_LENGTH = 3;
	static int inDeclaration = 0;
	char *acceptableVariables[3] = {"i", "j", "c"};
	int numAcceptable = 3;
	
	switch (progress) {
		case BEGINNING:
			inDeclaration++;
			break;
		case MIDDLE:
			if (inDeclaration) {
				if (strlen(identifier) < MINIMUM_VARIABLE_NAME_LENGTH) {
					int i;
					
					/* see if number is within the acceptableVariables array */
					for (i = 0; i < numAcceptable; i++) {
						if (strcmp(acceptableVariables[i], identifier) == 0) {
							return;
						}
					}
					
					flyyerror(ERROR_NORMAL,
							  location, 
							  "Variable/function name '%s' is too short",
							  identifier);
				}
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
	int acceptableNumbers[3] = {0, 1, 2};
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
				
				lyyerror(ERROR_HIGH, location, "Do not use magic numbers");
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
	int MAX_LOOP_LENGTH = 35;
	
	if (location.last_line - location.first_line + 1 >= MAX_LOOP_LENGTH) {
		flyyerror(ERROR_NORMAL,
				  location, 
				  "Loop is too long, should be less than %d lines; consider pulling out the code into its own function",
				  MAX_LOOP_LENGTH);
	}
}

/**
 * Check if compound statement is empty
 */
void isCompoundStatementEmpty(YYLTYPE location, int progress) {
	static void (*context)(YYLTYPE);
	
	switch (progress) {
		case BEGINNING:
			context = lastCalled_get();
			break;
		case END:
			if (lastCalled_get() == beginCompoundStatement) {
				/* create a good error message */
				char *parent = NULL;
				
				if (context == beginIf) { parent = "if statements"; }
				else if (context == beginElse) { parent = "else statements"; }
				else if (context == beginFor) { parent = "for loops"; }
				else if (context == beginWhile) { parent = "while loops"; }
				else if (context == beginDoWhile) { parent = "doWhile loops"; }
				
				if (parent) {
					flyyerror(ERROR_HIGH, location, "Do not use empty %s", parent);
				} else {
					lyyerror(ERROR_HIGH, location, "Do not use empty block statements");
				}
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
				flyyerror(ERROR_LOW,
						  location, 
						  "There are too many functions in this file. Please limit yourself to %d",
						  MAX_FUNCTIONS_PER_FILE);
			}
			numFunctions = 0;
			break;
		default:
			break;
	}
}

/**
 * Throw error based on the placement of the else relative to the if statement. 
 *  - if an if statement is bracketed, it should appear on multiple lines
 *  - if an if statement is bracketed, the else statement should be on the same line as
 *    as the closing brace
 *  - if an if statement is unbracketed, the else statement should appear on the next line
 */
void checkIfElsePlacement(YYLTYPE location, int progress) {
	static int ifIsBracketed = 0;
	static int ifLastLine;
	static int hadError = 0;
	
	switch (progress) {
		case BEGINNING:
			ifIsBracketed = (lastCalled_get() == endCompoundStatement);
			ifLastLine = location.last_line;
			/* check that, if bracketed, appears on multiple lines */
			if (ifIsBracketed && ((location.last_line - location.first_line) <= 0)) {
				lyyerror(ERROR_LOW, location, "When using braces with an if statement, use multiple lines");
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
					lyyerror(ERROR_LOW,
							 location, 
							 "Please put the else on the same line as the closing if brace");
				}
			} else {
				/* else should be on line after the if statement finishes */
				if ((location.first_line - ifLastLine) <= 0) {
					lyyerror(ERROR_LOW, location, "Please put the else on the line after the if");
				}
			}
			
			hadError = 0;
			break;
		default:
			break;
	}
}

/**
 * Helper function to free text.
 */
static void freeText(void* element, void* extra) {
	free(element);
}

/**
 * Make sure function comments have the appropriate contents. Specifically check
 * that the comment mentions each parameter (by name) and what the function returns.
 */
void validateComment(YYLTYPE location, enum commandType command, char* text) {
	static YYLTYPE *beginFunctionLocation = NULL;
	static DynArray_T parameters = NULL;
	static int didReturnSomething;
	static int inParameterList;
	static int inFunction;
	static int isFirstBracket;
	static int bracketLine;
	static int bracketColumn;
	
	char* commentText = NULL;
	
	switch (command) {
		case BEGIN_FUNCTION:
			inFunction = 1;
			beginFunctionLocation = allocateLocation(location);
			parameters = DynArray_new(0);
			didReturnSomething = 0;
			inParameterList = 0;
			isFirstBracket = 1;
			break;
		case BEGIN_PARAM_LIST:
			inParameterList++;
			break;
		case FOUND_IDENTIFIER:
			if (inFunction && inParameterList == 1) {
				/* ASSUMPTION: all identifier found in a paramter list are the names
				   of the parameters. */
				DynArray_add(parameters, strdup(text));
			}
			break;
		case END_PARAM_LIST:
			inParameterList--;
			break;
		case BEGIN_FUNCTION_BODY:
			if (inFunction && isFirstBracket) {
				bracketLine = location.first_line;
				bracketColumn = location.first_column;
			}
			isFirstBracket = 0;
			break;
		case RETURNING:
			didReturnSomething = 1;
			break;
		case END_FUNCTION:
			/* find comment text */
			commentText = comment_getCommentAbove(*beginFunctionLocation, 1);
			if (commentText == NULL) {
				YYLTYPE * between = allocateLocation(*beginFunctionLocation);
				between->first_line = beginFunctionLocation->last_line;
				between->first_column = beginFunctionLocation->last_column;
				between->last_line = bracketLine;
				between->last_column = bracketColumn;
				
				commentText = comment_getCommentWithin(*between);
				freeLocations(between, NULL);
				
				if (commentText == NULL) {
					lyyerror(ERROR_HIGH,
							 location,
							 "Please include a descriptive comment above each function");
					return;
				}
			}
			
			/* look for each parameter name */
			int i;
			int numParameters = DynArray_getLength(parameters);
			int numParametersInComment = 0;
			for (i = 0; i < numParameters; i++) {
				char *position = strstr(commentText, (char*)DynArray_get(parameters, i));
				
				if (position != NULL) {
					numParametersInComment++;
				}
			}
			
			if (numParametersInComment < numParameters) {
				lyyerror(ERROR_HIGH, 
						 location, 
						 "A function's comment should refer to each parameter by name");
			}
			
			/* if didReturnSomething, look for the word 'return' */
			if (didReturnSomething) {
				char *position = strstr(commentText, "return");
				
				if (position == NULL) {
					lyyerror(ERROR_HIGH,
							 location, 
							 "A function's comment should explictly state what the function returns");
				}
			}
			
			/* clean up */
			inFunction = 0;
			freeLocations(beginFunctionLocation, NULL);
			DynArray_map(parameters, freeText, NULL);
			DynArray_free(parameters);
			break;
		default:
			break;
	}

}