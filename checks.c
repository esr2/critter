/*
 *  checks.c
 *
 *
 *  Created by Erin Rosenbaum on 11/22/10.
 *  Copyright 2010 CritTer. All rights reserved.
 *
 */

#include "checks.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "dynarray.h"
#include "comments.h"
#include "sax.h"
#include "locations.h"

/**
 * Check if the file exceeds a maximum length.
 *
 * COS217 MAX_FILE_LENGTH = 500
 */
void isFileTooLong(YYLTYPE location) {
	int MAX_FILE_LENGTH = 500;

	if (location.first_line > MAX_FILE_LENGTH) {
		lyyerrorf(ERROR_NORMAL, location,
              "File is too long, should be less than %d lines",
              MAX_FILE_LENGTH);
	}
}

/**
 * Check if the statement within an if statement, else clause, for statement,
 * while statement, and do while statement is a compound statement.
 */
void hasBraces(YYLTYPE location, char* construct) {
	if (lastCalled_get() != endCompoundStatement) {
		if ((strcmp(construct,"else")== 0) && (lastCalled_get() == endElse ||
																					 lastCalled_get() == endIf)) {
			/* at the end of an else if [else] */
			return;
		}

		/* Dondero lyyerrorf(ERROR_NORMAL, location,
              "Please use braces after all %s statements", construct); */
	}

}

/**
 * Check if a function exceeds a maximum line count.
 *
 * COS217 MAX_FUNCTION_LENGTH = 140
 */
void isFunctionTooLongByLines(YYLTYPE location) {
	int MAX_FUNCTION_LENGTH = 140;

	if (location.last_line - location.first_line + 1 >= MAX_FUNCTION_LENGTH) {
		lyyerrorf(ERROR_NORMAL, location, "Function is too long by line count, "
              "should be less than %d lines", MAX_FUNCTION_LENGTH);
	}
}

/**
 * Check if a function exceeds a maximum statement count.
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
				lyyerrorf(ERROR_NORMAL, location,
                  "Function is too long by statement count, "
                  "should be less than %d statements", MAX_FUNCTION_LENGTH);
			}
			break;
		default:
			break;
	}
}

/**
 * Checks if there are too many parameters in the function declaration.
 *
 * COS217 MAX_NUM_PARAMETERS = 7
 */
void tooManyParameters(YYLTYPE location, int progress) {
	int MAX_NUM_PARAMETERS = 7;

	static int numParameters = -1;
	static int nestedListLevel = -1; /* should = 0 inside a non-nested
																			parameter list */

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
				lyyerrorf(ERROR_HIGH, location,
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
 * Warn against using C++ style single line comments.
 *
 * COS217
 */
void neverUseCPlusPlusComments(YYLTYPE location) {
	lyyerror(ERROR_NORMAL, location, "Do not use C++ style comments");
}

/**
 * Check for a comment at the beginning of a file.
 *
 * COS217
 */
void fileHasComment(YYLTYPE location) {
	char* text = comment_getCommentOnSameLine(location, NULL);

	if (text == NULL) {
		// comment not found
		lyyerrorf(ERROR_HIGH, location,
							"Please include a descriptive comment at the start of each file");
	}
}

/**
 * Check for comments before some construct.
 *
 * COS217 - global variables
 */
void hasComment(YYLTYPE location, char* construct) {
	char* text = comment_getCommentAbove(location, /* Dondero 1 */ 2, NULL);

	if (text == NULL) {
		// comment not found
		lyyerrorf(ERROR_HIGH, location,
							"Please include a descriptive comment above each %s", construct);
	}
}

/**
 * Check that each switch statement has a default case.
 *
 * COS217
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
				lyyerror(ERROR_HIGH, location,
								 "Always include a default in switch statements");
			}
			started = 0;
			break;
		default: break;
	}
}

/**
 * Check that each switch case has a break or return statement.
 *
 * COS217
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
			if (numCases > numBreaks) {
				int numMissing = numCases - numBreaks;
				lyyerrorf(ERROR_HIGH, location,
									"Each case/default in a switch statement should "
									"have a break statement, you're missing %d", numMissing);
			}
			break;
		default:
			break;
	}
}

/**
 * Check whether a region of code (i.e. a compound statement) nests too deeply.
 *
 * COS217 MAX_NESTING_LEVEL = 3: permits function, loop, if. Further nesting
 * is deemed too much.
 */
void isTooDeeplyNested(YYLTYPE location, int progress) {
	int MAX_NESTING_LEVEL = 3;

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
				lyyerror(ERROR_HIGH, location,
                 "This area is too deeply nested, consider refactoring");
			}
			nestedLevel--;
			break;
		default:
			break;
	}

}

/**
 * Warn against using #define instead of enum for declarations.
 *
 * COS217
 */
void useEnumNotDefine(YYLTYPE location) {
	lyyerror(ERROR_NORMAL, location,
					 "It would be better to use enum to define integral constants");
}

/**
 * Warn against using GOTO statements.
 *
 * COS217
 */
void neverUseGotos(YYLTYPE location) {
	lyyerror(ERROR_HIGH, location, "Never use GOTO statements");
}

/**
 * Check if a variable's name exceeds a minimum length.
 *
 * COS217 MINIMUM_VARIABLE_NAME_LENGTH = 3
 * accepted short names: "i", "j", "k", "c", "n", "fp", "fd", "pc", "ul"
 */
void isVariableNameTooShort(YYLTYPE location, int progress, char* identifier) {
	int MINIMUM_VARIABLE_NAME_LENGTH = 3;
	static int inDeclaration = 0;
   /* Dondero
	char *acceptableVariables[8] = {"i", "j", "k", "c", "n", "fp", "fd",
																	"pc"};
   */
	char *acceptableVariables[] = {
		"c", "pc", "c1", "c2", "uc", "ac",
		"s", "ps", "s1", "s2", "us", "as",
		"i", "pi", "i1", "i2", "ui", "ai",
		"l", "pl", "l1", "l2", "ul", "al",
		"f", "pf", "f1", "f2", "af",
		"d", "pd", "d1", "d2", "ad",
		"pv",
		"o", "po", "ao",
		"j", "k", "n", "m"
	};
	int numAcceptable = sizeof(acceptableVariables)/sizeof(char*);

	switch (progress) {
		case BEGINNING:
			inDeclaration++;
			break;
		case MIDDLE:
			if (inDeclaration) {
				if (strlen(identifier) < MINIMUM_VARIABLE_NAME_LENGTH) {
					int i;

					/* see if variable name is within the acceptableVariables array */
					for (i = 0; i < numAcceptable; i++) {
						if (strcmp(acceptableVariables[i], identifier) == 0) {
							return;
						}
					}

					lyyerrorf(ERROR_NORMAL, location,
										"Variable/function name '%s' is too short", identifier);
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
 * Warn against using magic numbers outside of a declaration. (Presumably,
 * inside a declaration, a variable will be initialized to a magic
 * number and then used throughout the rest of the code).
 *
 * COS217 acceptable numbers = 0, 1, 2 but never in case statements
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
			if (lastCalled_get() == registerCase) {
        /* never use numbers in case statements */
				lyyerrorf(ERROR_HIGH, location, "Do not use magic numbers (%s)",
                  constant);
			} else if (inDeclaration == 0) {
				int number = (int)strtol(constant, (char**)NULL, 0);
				int i;

				/* see if number is within the acceptableNumbers array */
				for (i = 0; i < numAcceptable; i++) {
					if (number == acceptableNumbers[i]) {
						return;
					}
				}

				lyyerrorf(ERROR_HIGH, location, "Do not use magic numbers (%s)",
									constant);
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
 * Check if each global variable has a comment.
 *
 * COS217
 */
void globalHasComment(YYLTYPE location, int progress) {
	static int inFunction = 0;

	switch (progress) {
		case BEGINNING:
			inFunction = 1;
			break;
		case MIDDLE:
			if (!inFunction) {
				hasComment(location, "global variable");
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
 * Check if the loop length exceeds a maximum length.
 *
 * COS217 MAX_LOOP_LENGTH = 35
 */
void isLoopTooLong(YYLTYPE location) {
	int MAX_LOOP_LENGTH = 35;

	if (location.last_line - location.first_line + 1 >= MAX_LOOP_LENGTH) {
		lyyerrorf(ERROR_NORMAL, location,
							"Loop is too long, should be less than %d lines; "
							"consider pulling out the code into its own function",
							MAX_LOOP_LENGTH);
	}
}

/**
 * Check if the compound statement is empty.
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
					lyyerrorf(ERROR_HIGH, location, "Do not use empty %s", parent);
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
 *
 * COS217 MAX_FUNCTIONS_PER_FILE = 15
 */
void tooManyFunctionsInFile(YYLTYPE location, int progress) {
	int MAX_FUNCTIONS_PER_FILE = 15;
	static int numFunctions = 0;

	switch (progress) {
		case MIDDLE:
			numFunctions++;
			break;
		case END:
			if (numFunctions > MAX_FUNCTIONS_PER_FILE) {
				lyyerrorf(ERROR_LOW, location,
									"There are too many functions in this file. "
									"Please limit yourself to %d", MAX_FUNCTIONS_PER_FILE);
			}
			numFunctions = 0;
			break;
		default:
			break;
	}
}

/**
 * Warn against poor if/else placement as defined by the Google style guide.
 *  - if an if statement is bracketed, it should appear on multiple lines
 *  - if an if statement is bracketed, the else statement should be on the same
 *    line as the closing brace
 *  - if an if statement is unbracketed, the else statement should appear on
 *    the next line
 */
void isIfElsePlacementValid(YYLTYPE location, int progress) {
	static int ifIsBracketed = 0;
	static int ifLastLine;
	static int hadError = 0;

	switch (progress) {
		case BEGINNING:
			ifIsBracketed = (lastCalled_get() == endCompoundStatement);
			ifLastLine = location.last_line;
			/* check that, if bracketed, appears on multiple lines */
			if (ifIsBracketed && ((location.last_line - location.first_line) <= 0)) {
				lyyerror(ERROR_LOW, location,
                "When using braces with an if statement, use multiple lines");
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
					/* Dondero lyyerror(ERROR_LOW, location,
                   "Please put the else on the same line as the closing if brace"); */
				}
			} else {
				/* else should be on line after the if statement finishes */
				if ((location.first_line - ifLastLine) <= 0) {
					lyyerror(ERROR_LOW, location,
                   "Please put the else on the line after the if");
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
 * Check if function comments have the appropriate contents. Specifically check
 * that the comment mentions each parameter (by name) and what the function
 * returns.
 *
 * COS217
 */
void isFunctionCommentValid(YYLTYPE location, enum commandType command,
														char* text) {
	static YYLTYPE *beginFunctionLocation = NULL;
	static DynArray_T parameters = NULL;
	static int didReturnSomething;
	static int inParameterList;
	static int inFunction;
	static int isFirstBracket;
	static int bracketLine;
	static int bracketColumn;

	char* commentText = NULL;
	char* commentTextAbove = NULL;
	char* commentTextBetween = NULL;
	YYLTYPE commentLocationAbove;
	YYLTYPE commentLocationBetween;
	YYLTYPE* commentLocation = NULL;

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
				/* ASSUMPTION: all identifiers found in a paramter list are the names
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
			commentTextAbove = comment_getCommentAbove(*beginFunctionLocation, 2,
																								 &commentLocationAbove);

			YYLTYPE * between = allocateLocation(*beginFunctionLocation);
			between->first_line = beginFunctionLocation->last_line;
			between->first_column = beginFunctionLocation->last_column;
			between->last_line = bracketLine;
			between->last_column = bracketColumn;

			commentTextBetween = comment_getCommentWithin(*between,
																										&commentLocationBetween);
			freeLocations(between, NULL);

			if (commentTextBetween != NULL) {
				commentText = commentTextBetween;
				commentLocation = &commentLocationBetween;
			} else {
				commentText = commentTextAbove;
				commentLocation = &commentLocationAbove;
			}

			int commentHasContent = comment_isContentful(commentText);


			if (commentText == NULL || (commentHasContent != 1)) {
				lyyerror(ERROR_HIGH, location,
                 "Please include a descriptive comment above each function");

				/* clean up */
				inFunction = 0;
				freeLocations(beginFunctionLocation, NULL);
				DynArray_map(parameters, freeText, NULL);
				DynArray_free(parameters);

				return;
			}

			/* look for each parameter name */
			int i;
      char missingParameters[500];
      memset(missingParameters, '\0', 500);
			int numParameters = DynArray_getLength(parameters);
			int numParametersInComment = 0;
			for (i = 0; i < numParameters; i++) {
				if (comment_contains(commentText, (char*)DynArray_get(parameters, i), 0)) {
					numParametersInComment++;
				} else {
          strcat(missingParameters, (char*)DynArray_get(parameters, i));
          strcat(missingParameters, " ");
        }
			}

			if (numParametersInComment < numParameters) {
				lyyerrorf(ERROR_HIGH, location,
								  "A function's comment should refer to each parameter "
                  "by name: %s", missingParameters);
			}

			/* if didReturnSomething, look for the word 'return' */
			if (didReturnSomething) {
				if (!comment_contains(commentText, "return", 1)) {
					lyyerror(ERROR_HIGH, location,
									 "A function's comment should explicitly state what "
									 "the function returns");
				}
			}

			/* check that the comment is not on the same line as the function
				 declaration */
			if (commentLocation->first_line == beginFunctionLocation->first_line) {
				lyyerror(ERROR_NORMAL, location,
								 "Please put function comments above the function declaration");
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

/**
 * Check if each pointer type parameter into a function is mentioned within
 * an assert() before being used. This check will most likely produce a couple
 * of false warnings (ie throw an error on a proper use of the pointer) because
 * it is only checking for asserts.
 *
 * COS217
 */
void arePointerParametersValidated(YYLTYPE location, enum commandType command,
																	 char* identifier) {
	static DynArray_T parameterNames = NULL;
	static int lastIdentifierWasAssert = 0;
	static int inParameterList;
	static int parameterIsAPointer;
	static int inFunction = 0;
  static char * parameterName = NULL;

	switch (command) {
		case BEGIN_FUNCTION:
			inFunction = 1;
			parameterNames = DynArray_new(0);
			lastIdentifierWasAssert = 0;
			inParameterList = 0;
			parameterIsAPointer = 0;
			break;
		case BEGIN_PARAM_LIST:
			inParameterList++;
      parameterName = NULL;
			break;
		case REGISTER_PARAM:
			/* called after all elements in a parameter */
      /* determine if parameter was a relevant pointer and store it */
      if (parameterIsAPointer && (parameterName != NULL) &&
          strcmp("argv", parameterName) != 0) {
        DynArray_add(parameterNames, strdup(parameterName));
      }

      if (parameterName != NULL) { free(parameterName); }
			parameterIsAPointer = 0;
			break;
		case END_PARAM_LIST:
			inParameterList--;
			break;
		case FOUND_IDENTIFIER:
			if (!inFunction) {
				return;
			}

			if (inParameterList == 1) {
				/* if identifier is a parameter that isn't argv, store the name
           temporarily. */
        parameterName = strdup(identifier);
				return;
			}

			if (strcmp("assert", identifier) == 0) {
				lastIdentifierWasAssert = 1;
			} else {
				int numParameters = DynArray_getLength(parameterNames);
				int i;
				for (i = 0; i < numParameters; i++) {
					if (strcmp(identifier, DynArray_get(parameterNames, i)) == 0) {
						/* identifier matches a parameter name */
						if (!lastIdentifierWasAssert) {
							lyyerrorf(ERROR_NORMAL, location,
												"Do you want to validate %s through an assert?",
												identifier);
						}
						/* remove the parameter from the list */
						char* name = (char*)DynArray_removeAt(parameterNames, i);
						free(name);
						break;
					}
				}
			}
			break;
		case FOUND_POINTER:
			if (inFunction && inParameterList == 1) {
				parameterIsAPointer = 1;
			}
			break;
		case END_STATEMENT:
			lastIdentifierWasAssert = 0;
			break;
		case END_FUNCTION:
			/* free each parameter name and the list */
			DynArray_map(parameterNames, freeText, NULL);
			DynArray_free(parameterNames);
			inFunction = 0;
			break;
		default:
			break;
	}

}

/**
 * Check that function names contain a common prefix. Prefix matching stops
 * upon seeing an underscore.
 *
 * COS217
 */
void doFunctionsHaveCommonPrefix(YYLTYPE location, int progress,
																 char* identifier) {
	static DynArray_T functionNames;
	static DynArray_T functionLocations;
	static int functionNameIsFound;
	static int inFunction;

	/* ASSUMPTION: the first identifier to appear after a begin function call
	 is the name of the function */

	switch (progress) {
		case BEGINNING:
			functionNames = DynArray_new(0);
			functionLocations = DynArray_new(0);
			functionNameIsFound = 0;
			inFunction = 0;
			break;
		case MIDDLE:
			if (identifier == NULL) {
				/* beginning of the function, reset if name has been found */
				functionNameIsFound = 0;
				inFunction = 1;
			} else if (inFunction && !functionNameIsFound) {
				functionNameIsFound = 1;
				DynArray_add(functionNames, strdup(identifier));
				DynArray_add(functionLocations, allocateLocation(location));
			}
			break;
		case END:
			if (identifier && strcmp(identifier, "file") == 0) {
				/* end of file */

				/* go through array and find all entries where the location
				   matches the current file name. Check these entries for a
				   common prefix and then remove them from the arrays */

				/* grab all the relevant names */
				DynArray_T functionsInFile = DynArray_new(0);
				int i, j;
				for (i = 0; i < DynArray_getLength(functionNames); ) {
					YYLTYPE * loc = DynArray_get(functionLocations, i);
					if (strcmp(loc->filename, location.filename) == 0) {
						/* function is in file, move it to the local array */
						char* name = DynArray_removeAt(functionNames, i);
						if (strcmp(name, "main") != 0) {
							DynArray_add(functionsInFile, name);
						} else {
              /* if the file has a main function */
              DynArray_free(functionsInFile);
              return;
            }
						loc = DynArray_removeAt(functionLocations, i);
						freeLocations(loc, NULL);
					} else {
						i++;
					}
				}

				/* throw error on first mismatch and stop checking at
					 first underscore */
				int numNames = DynArray_getLength(functionsInFile);
				if (numNames == 0) {
					DynArray_free(functionsInFile);
					return;
				}
				char * names[numNames];
				DynArray_toArray(functionsInFile, (void**)&names);
				size_t length = strlen(names[0]);

				int foundMismatch = 0;
				for (i = 0; i < length; i++) {
					char c = names[0][i];
					if (c == '_' && i >=3) {
						/* assume we've found the end of the prefix and stop
						   checking the function names */
						break;
					}

					for (j = 1; j < numNames; j++) {
						if (names[j][i] != c) {
							foundMismatch = 1;
							break;
						}
					}
					if (foundMismatch) {
						break;
					}
				}

				if (foundMismatch) {
					lyyerror(ERROR_NORMAL, location,
									 "Please prefix all function names with a reasonable "
									 "module name");
				} else {
					char prefix[i+1];
					strncpy(prefix, names[0], i);
					prefix[i] = '\0';

					if (strstrInsensitive(location.filename, prefix) == NULL) {
						lyyerrorf(ERROR_NORMAL, location,
											"Please match function prefixes to the module name, "
											"%s and %s do not match", prefix, location.filename);
					}
				}

				DynArray_map(functionsInFile, freeText, NULL);
				DynArray_free(functionsInFile);

			} else if (identifier && strcmp(identifier, "function") == 0) {
				inFunction = 0;
			} else {
				/* end of program */
				DynArray_map(functionLocations, freeLocations, NULL);
				DynArray_free(functionLocations);
				DynArray_map(functionNames, freeText, NULL);
				DynArray_free(functionNames);
			}
			break;
		default:
			break;
	}

}

/**
 * Check that there are enough local comments in the function relative to the
 * number of control/selection statements.
 *
 * COS217 maxDiscrepancy = 4
 */
void functionHasEnoughLocalComments(YYLTYPE location, int progress, int isComment) {
	/* count up the number of elements that really should have comments
	   associated to them in some fashion: if, for, while, do while, switch */
	static int numElements;

	/* count up the number of comments that appear in the function */
	static int numComments;

	int maxDiscrepancy = /* Dondero 4 */ 8;

	switch (progress) {
		case BEGINNING:
			numComments = 0;
			numElements = 0;
			break;
		case MIDDLE:
			isComment ? numComments++ : numElements++;
			break;
		case END:
			if (numElements - numComments > maxDiscrepancy) {
				lyyerror(ERROR_NORMAL, location,
								 "This function probably needs more local comments");
			}
			break;
		default:
			break;
	}

}

/**
 * Check that all fields in a struct have a comment.
 *
 * COS217
 */
void structFieldsHaveComments(YYLTYPE location, int progress) {
	static int numFields;
	int numComments = 0;
	YYLTYPE commentLocation;
	YYLTYPE * searchLocation = allocateLocation(location);

	switch (progress) {
		case BEGINNING:
			numFields = 0;
			break;
		case MIDDLE:
			numFields++;
			break;
		case END:
			while (comment_getCommentWithin(*searchLocation, &commentLocation) != NULL) {
				numComments++;
				searchLocation->first_line = commentLocation.last_line + 1;
			}

			if (numFields > numComments) {
				lyyerrorf(ERROR_NORMAL, location,
									"Please comment all the fields in a struct, you're missing %d",
									numFields-numComments);
			}
			break;
		default:
			break;
	}

	freeLocations(searchLocation, NULL);
}
