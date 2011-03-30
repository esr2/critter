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

static DynArray_T functionCallsQueue;
static DynArray_T locationsQueue;
static DynArray_T identifiersQueue;
static DynArray_T constantsQueue;

/*---------------- Util -----------------------*/
static void popIdentifier() { }
static void popConstant() { }
static void ignoreIdentifier() { }
static void ignoreIdentifierCall(YYLTYPE location, char* identifier) { }

static void freeText(void* element, void* extra) {
	char *text = (char*)element;
	free(text);
}

static void enqueueFunctionAndLocation(void (*f)(YYLTYPE), YYLTYPE location) {
	assert(DynArray_getLength(functionCallsQueue) == DynArray_getLength(locationsQueue));
	
	DynArray_add(functionCallsQueue, (void*)f);
	DynArray_add(locationsQueue, allocateLocation(location));
	
	assert(DynArray_getLength(functionCallsQueue) == DynArray_getLength(locationsQueue));
}

/**
 * Dequeue functions/locations off the large queues until a location on the stack
 * matches the given location. MatchWhole controls whether it has to be a full
 * match or just the first line/column. BeginCall is called on first location
 * and can be null. All functions that are dequeued are called in the proper order.
 */
static void dequeueUntil(YYLTYPE location, int matchWhole, void (*beginCall)(YYLTYPE)) {
	assert(DynArray_getLength(functionCallsQueue) == DynArray_getLength(locationsQueue));
	
	int i;
	void (*func)(YYLTYPE);
	YYLTYPE* loc = NULL;
	char *text = NULL;
	int firstLocation = 0;
	int numIdentifiers = 0;
	int numConstants = 0;
	
	/* first find how many popIdentifiers and popConstants exist in the 
	   functionCallsArray so we know where to start the queues */
	for (i = 0; i < DynArray_getLength(locationsQueue); i++) {
		if (!locationIsBeforeOrEqual(location, 
								   DynArray_get(locationsQueue, i),
								   matchWhole)) {
			/*ignore the beginning of the queue which is irrelevant */
			firstLocation++;
			continue;
		}
		
		func = DynArray_get(functionCallsQueue, i);
		if (func == popIdentifier || func == ignoreIdentifier) {
			numIdentifiers++;
		} else if (func == popConstant) {
			numConstants++;
		}
	}
	
	int identifierQPos = DynArray_getLength(identifiersQueue) - numIdentifiers;
	if (identifierQPos < 0) identifierQPos = 0;
	int constantQPos = DynArray_getLength(constantsQueue) - numConstants;
	if (constantQPos < 0) constantQPos = 0;
	
	/* dequeue function/location pairs after the starting location */
	for (i = firstLocation; i < DynArray_getLength(locationsQueue); ) {
		
		/* if there is a beginning call, make it */
		if (beginCall) {
			loc = DynArray_get(locationsQueue, i);
			beginCall(*loc);
			beginCall = NULL;
		}
		
		/* remove function and location from the queues */
		loc = DynArray_removeAt(locationsQueue, i);
		assert(loc != NULL);
		func = DynArray_removeAt(functionCallsQueue, i);
		assert(func != NULL);
		
		(*func)(*loc);
		lastCalled_set(func);
		
		/* call special functions for identifiers and constants in order to pass the
		   actual text */
		if (func == popIdentifier || func == popConstant ||
									 func == ignoreIdentifier) { 
			DynArray_T queue = NULL;
			void (*function)(YYLTYPE, char*);
			int position = 0;
			
			if (func == popIdentifier) {
				queue = identifiersQueue;
				function = registerIdentifier;
				position = identifierQPos;
			} else if (func == popConstant) {
				queue = constantsQueue;
				function = registerConstant;
				position = constantQPos;
			} else {
				queue = identifiersQueue;
				function = ignoreIdentifierCall;
				position = identifierQPos;
			}
			
			assert(queue);
			
			text = DynArray_removeAt(queue, position);
			
			function(*loc, text);
			free(text);
		}
		
		freeLocations(loc, NULL);
	}
	
	assert(DynArray_getLength(functionCallsQueue) == DynArray_getLength(locationsQueue));
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
	lastCalled_set(endFile);
}

/**
 * Called at the beginning of the program execution before
 * parsing begins.
 */
void h_beginProgram() {
	functionCallsQueue = DynArray_new(0);
	locationsQueue = DynArray_new(0);
	identifiersQueue = DynArray_new(0);
	constantsQueue = DynArray_new(0);
	
	assert(functionCallsQueue != NULL);
	assert(locationsQueue != NULL);
	assert(identifiersQueue != NULL);
	assert(constantsQueue != NULL);
	beginProgram();
	lastCalled_set(beginProgram);
}

/**
 * Called at the end of the program execution.
 */
void h_endProgram(YYLTYPE location) {
	endProgram(location);
	/* don't need to set lastCalledFunction because program is ending */
	
	assert(functionCallsQueue != NULL);
	assert(locationsQueue != NULL);
	
	// Free the function arrays
	DynArray_map(locationsQueue, freeLocations, NULL);
	DynArray_free(locationsQueue);
	DynArray_free(functionCallsQueue);
	DynArray_map(identifiersQueue, freeText, NULL);
	DynArray_free(identifiersQueue);
	DynArray_map(constantsQueue, freeText, NULL);
	DynArray_free(constantsQueue);
}
/*------------------------------------------------*/
void h_registerDefineIntegralType(YYLTYPE location) {
	registerDefineIntegralType(location);
}

/*------------ Declarations ----------------------*/
void h_registerIdentifier(YYLTYPE location) {
	enqueueFunctionAndLocation(popIdentifier,location);
}

void h_registerIdentifierText(char* identifier) {
	DynArray_add(identifiersQueue, strdup(identifier));
}

void h_ignoreIdentifierText(YYLTYPE location) {
	/* remove the last enqueued identifier text because I'm not going to call
	 register identifier for this occurence (enums, structs, etc) */
	enqueueFunctionAndLocation(ignoreIdentifier, location);
}

void h_registerConstant(YYLTYPE location) {
	enqueueFunctionAndLocation(popConstant, location);
}

void h_registerConstantText(char* constant) {
	DynArray_add(constantsQueue, strdup(constant));
}

void h_endDeclaration(YYLTYPE location) {
	dequeueUntil(location, 0, beginDeclaration);
	endDeclaration(location);
	lastCalled_set(endDeclaration);
}

void h_beginDirectDeclarator(YYLTYPE location) {
	enqueueFunctionAndLocation(doNothing,location);
}

void h_endDirectDeclarator(YYLTYPE location) {
	enqueueFunctionAndLocation(doNothing, location);
}

void h_endExpressionStatement(YYLTYPE location) {
	dequeueUntil(location, 0, beginStatement);
	endStatement(location);
	lastCalled_set(endStatement);
}

void h_registerExpression(YYLTYPE location) {
	dequeueUntil(location, 0, NULL);
}

/*------------ Function ----------------------*/
void h_beginFunctionDefinition(YYLTYPE location) {
	beginFunctionDefinition(location);
	lastCalled_set(beginFunctionDefinition);
	
	/* send appropriate calls for declaration_specifier, declarator etc */
	dequeueUntil(location, 0, NULL);
}

void h_beginParameterList(YYLTYPE location) {
	enqueueFunctionAndLocation(beginParameterList, location);
}

void h_registerParameter(YYLTYPE location) {
	enqueueFunctionAndLocation(registerParameter, location);
}

void h_endParameterList(YYLTYPE location) {
	enqueueFunctionAndLocation(endParameterList, location);
}

/*------------ Struct ----------------------*/

void h_beginStructDefinition(YYLTYPE location) {
	enqueueFunctionAndLocation(beginStructDefinition, location);	
}

void h_endStructDefinition(YYLTYPE location) {
	enqueueFunctionAndLocation(endStructDefinition, location);
}

/*------------ Pointer ----------------------*/

void h_registerPointer(YYLTYPE location) {
	enqueueFunctionAndLocation(registerPointer, location);
}

/*------------ Typedef ----------------------*/

void h_registerTypedef(YYLTYPE location) {
	/* add a call to doNothing so declarations can match on the right
	   beginning location (the typedef) */
	enqueueFunctionAndLocation(doNothing, location);
}

/*----- Declaration Specifiers --------------*/

static void h_registerDeclarationSpecifiers(YYLTYPE location) {
	
}

/* Type specifiers: */
static void h_registerTypeSpecifier(YYLTYPE location) {
	enqueueFunctionAndLocation(h_registerDeclarationSpecifiers, location);
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
	enqueueFunctionAndLocation(h_registerDeclarationSpecifiers, location);
}

void h_registerConst(YYLTYPE location) { 
	h_registerTypeQualifier(location);
	enqueueFunctionAndLocation(registerConst, location);
}

void h_registerVolatile(YYLTYPE location) {
	h_registerTypeQualifier(location);
}


/* Storage class specifiers */
static void h_registerStorageClass(YYLTYPE location) {
	enqueueFunctionAndLocation(h_registerDeclarationSpecifiers, location);
}

void h_registerExtern(YYLTYPE location) {h_registerStorageClass(location);}
void h_registerStatic(YYLTYPE location) {h_registerStorageClass(location);}
void h_registerAuto(YYLTYPE location) {h_registerStorageClass(location);}
void h_registerRegister(YYLTYPE location) {h_registerStorageClass(location);}