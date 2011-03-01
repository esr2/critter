/*
 *  hooks.c
 *  
 *
 *  Created by Erin Rosenbaum on 2/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "hooks.h"
#include "sax.h"
#include "dynarray.h"
#include "locations.h"
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <assert.h>

static DynArray_T functionCallsArray;
static DynArray_T locationsArray;
static DynArray_T identifiersArray;
static DynArray_T constantsArray;

/*---------------- Util -----------------------*/
static void popIdentifier() { }
static void popConstant() { }


static void freeText(void* element, void* extra) {
	char *text = (char*)element;
	free(text);
}

static void addFunctionAndLocationToStacks(void (*f)(YYLTYPE), YYLTYPE location) {
	assert(DynArray_getLength(functionCallsArray) == DynArray_getLength(locationsArray));
	
	DynArray_add(functionCallsArray, (void*)f);
	DynArray_add(locationsArray, allocateLocation(location));
	
	assert(DynArray_getLength(functionCallsArray) == DynArray_getLength(locationsArray));
}

/**
 * Pop functions/locations off the large stacks until a location on the stack
 * matches the given location. MatchWhole controls whether it has to be a full
 * match or just the first line/column. BeginCall is called on first location
 * and can be null. All functions that are popped are called in the proper order.
 */
static void popUntil(YYLTYPE location, int matchWhole, void (*beginCall)(YYLTYPE)) {
	assert(DynArray_getLength(functionCallsArray) == DynArray_getLength(locationsArray));
	
	int len, i, l;
	void (*func)(YYLTYPE);
	YYLTYPE* loc = NULL;
	char *text = NULL;
	DynArray_T locations = DynArray_new(0);
	DynArray_T functions = DynArray_new(0);
	DynArray_T identifiers = DynArray_new(0);
	DynArray_T constants = DynArray_new(0);
	
	/* pop off function/location pairs until location matches input location */
	len = DynArray_getLength(locationsArray) - 1;
	while (len >= 0 && locationIsLessOrEqual(location, 
											 DynArray_get(locationsArray, len),
											 matchWhole)) {
		/* remove function and location from the stacks */
		loc = DynArray_removeAt(locationsArray, len);
		assert(loc != NULL);
		func = DynArray_removeAt(functionCallsArray, len);
		assert(func != NULL);
		
		/* add the function and location to the temporary stacks */
		DynArray_add(locations, loc);
		DynArray_add(functions, func);
		len--;
		
		/* push identifiers and constants onto their own stacks to preserve
		   proper ordering */
		if (func == popIdentifier || func == popConstant) { 
			DynArray_T stack = NULL;
			DynArray_T temporaryStack = NULL;
			
			if (func == popIdentifier) {
				stack = identifiersArray;
				temporaryStack = identifiers;
			} else {
				stack = constantsArray;
				temporaryStack = constants;
			}
			assert(stack);
			assert(temporaryStack);
			
			l = DynArray_getLength(stack) - 1;
			text = DynArray_removeAt(stack, l);
			assert(text != NULL);
			
			DynArray_add(temporaryStack, text);
		}
		
	}
	
	/* if there is a beginning call, make it */
	i = DynArray_getLength(locations) - 1;
	if (beginCall) {
		loc = DynArray_get(locations, i);
		beginCall(*loc);
	}
	
	/* pop off all the items on the temporary stacks to preserve ordering */
	for (; i >=0; i--) {
		/* remove and call functions */
		loc = DynArray_removeAt(locations, i);
		func = DynArray_removeAt(functions, i);
		
		(*func)(*loc);
		lastCalledFunction = func;
		
		/* call special functions for identifiers and constants in order to pass the
		   actual text */
		if (func == popIdentifier || func == popConstant) { 
			DynArray_T temporaryStack = NULL;
			void (*function)(YYLTYPE, char*);
			
			if (func == popIdentifier) {
				temporaryStack = identifiers;
				function = registerIdentifier;
			} else {
				temporaryStack = constants;
				function = registerConstant;
			}
			assert(temporaryStack);
			
			len = DynArray_getLength(temporaryStack) - 1;
			text = DynArray_removeAt(temporaryStack, len);
			
			function(*loc, text);
			free(text);
		}
		
		freeLocations(loc, NULL);
	}
	
	DynArray_free(locations);
	DynArray_free(functions);
	DynArray_free(identifiers);
	DynArray_free(constants);
	
	assert(DynArray_getLength(functionCallsArray) == DynArray_getLength(locationsArray));
}

static void doNothing(YYLTYPE location) {}

/*------------ Overall ----------------------*/
/**
 * Called at the beginning of each file before parsing 
 * begins.
 */
void h_beginFile(char* filename) {
	beginFile(filename);
}

/**
 * Called at the end of each file. Location.first_line 
 * = last_line.
 */
void h_endFile(YYLTYPE location) {
	endFile(location);
	lastCalledFunction = endFile;
}

/**
 * Called at the beginning of the program execution before
 * parsing begins.
 */
void h_beginProgram() {
	functionCallsArray = DynArray_new(0);
	locationsArray = DynArray_new(0);
	identifiersArray = DynArray_new(0);
	constantsArray = DynArray_new(0);
	
	assert(functionCallsArray != NULL);
	assert(locationsArray != NULL);
	assert(identifiersArray != NULL);
	assert(constantsArray != NULL);
	beginProgram();
	lastCalledFunction = beginProgram;
}

/**
 * Called at the end of the program execution.
 */
void h_endProgram(YYLTYPE location) {
	endProgram(location);
	/* don't need to set lastCalledFunction because program is ending */
	
	assert(functionCallsArray != NULL);
	assert(locationsArray != NULL);
	
	// Free the function arrays
	DynArray_map(locationsArray, freeLocations, NULL);
	DynArray_free(locationsArray);
	DynArray_free(functionCallsArray);
	DynArray_map(identifiersArray, freeText, NULL);
	DynArray_free(identifiersArray);
	DynArray_map(constantsArray, freeText, NULL);
	DynArray_free(constantsArray);
}
/*------------------------------------------------*/
void h_registerDefineIntegralType(YYLTYPE location) {
	registerDefineIntegralType(location);
}

/*------------ Declarations ----------------------*/
static int inDeclarator = 0;

void h_registerIdentifier(YYLTYPE location) {
	addFunctionAndLocationToStacks(popIdentifier,location);
}

void h_registerIdentifierText(char* identifier) {
	DynArray_add(identifiersArray, strdup(identifier));
}

void h_registerConstant(YYLTYPE location) {
	addFunctionAndLocationToStacks(popConstant, location);
}

void h_registerConstantText(char* constant) {
	DynArray_add(constantsArray, strdup(constant));
}

void h_endDeclaration(YYLTYPE location) {
	popUntil(location, 0, beginDeclaration);
	endDeclaration(location);
	lastCalledFunction = endDeclaration;
}

void h_beginDirectDeclarator(YYLTYPE location) {
	inDeclarator++;
	addFunctionAndLocationToStacks(doNothing,location);
}

void h_endDirectDeclarator(YYLTYPE location) {
	inDeclarator--;
	addFunctionAndLocationToStacks(doNothing, location);
}

void h_endExpressionStatement(YYLTYPE location) {
	popUntil(location, 0, beginStatement);
	endStatement(location);
	lastCalledFunction = endStatement;
}

/*------------ Function ----------------------*/
void h_beginFunctionDefinition(YYLTYPE location) {
	beginFunctionDefinition(location);
	lastCalledFunction = beginFunctionDefinition;
	
	/* send appropriate calls for declaration_specifier, declarator etc */
	popUntil(location, 1, NULL);
}

void h_beginParameterList(YYLTYPE location) {
	addFunctionAndLocationToStacks(beginParameterList, location);
}

void h_registerParameter(YYLTYPE location) {
	addFunctionAndLocationToStacks(registerParameter, location);
}

void h_endParameterList(YYLTYPE location) {
	addFunctionAndLocationToStacks(endParameterList, location);
}
/*------------ Typedef ----------------------*/

void h_registerTypedef(YYLTYPE location) {
	/* add a call to doNothing so declarations can match on the right
	   beginning location (the typedef) */
	addFunctionAndLocationToStacks(doNothing, location);
}

/*----- Declaration Specifiers --------------*/

static void h_registerDeclarationSpecifiers(YYLTYPE location) {
	
}

/* Type specifiers: */
static void h_registerTypeSpecifier(YYLTYPE location) {
	addFunctionAndLocationToStacks(h_registerDeclarationSpecifiers, location);
}

void h_registerVoid(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerChar(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerShort(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerInt(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerLong(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerFloat(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerDouble(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerSigned(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerUnsigned(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerStructUnionSpecifier(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerEnumSpecifier(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerTypeName(YYLTYPE location) {h_registerTypeSpecifier(location);}

/* Type qualifiers */
static void h_registerTypeQualifier(YYLTYPE location) {
	addFunctionAndLocationToStacks(h_registerDeclarationSpecifiers, location);
}

void h_registerConst(YYLTYPE location) { 
	h_registerTypeQualifier(location);
	addFunctionAndLocationToStacks(registerConst, location);
}

void h_registerVolatile(YYLTYPE location) {
	h_registerTypeQualifier(location);
}


/* Storage class specifiers */
static void h_registerStorageClass(YYLTYPE location) {
	addFunctionAndLocationToStacks(h_registerDeclarationSpecifiers, location);
}

void h_registerExtern(YYLTYPE location) {h_registerStorageClass(location);}
void h_registerStatic(YYLTYPE location) {h_registerStorageClass(location);}
void h_registerAuto(YYLTYPE location) {h_registerStorageClass(location);}
void h_registerRegister(YYLTYPE location) {h_registerStorageClass(location);}