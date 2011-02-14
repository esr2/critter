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
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <assert.h>

static DynArray_T functionCallsArray;
static DynArray_T locationsArray;

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
}

/**
 * Called at the beginning of the program execution before
 * parsing begins.
 */
void h_beginProgram() {
	functionCallsArray = DynArray_new(0);
	locationsArray = DynArray_new(0);
	
	assert(functionCallsArray != NULL);
	assert(locationsArray != NULL);
	beginProgram();
}

static void freeLocations(void* element, void* extra) { 
	YYLTYPE *location = (YYLTYPE *)element;
	if (location->filename != NULL) { free(location->filename); }
	free(element); 
}

static YYLTYPE* allocateLocation(YYLTYPE location) {
	YYLTYPE *loc = malloc(sizeof(YYLTYPE));
	loc->filename = malloc(sizeof(char) * strlen(location.filename));
	strcpy(loc->filename, location.filename);
	loc->first_line = location.first_line;
	loc->first_column = location.first_column;
	loc->last_line = location.last_line;
	loc->last_column = location.last_column;
	
	return loc;
}

/**
 * Called at the end of the program execution.
 */
void h_endProgram(YYLTYPE location) {
	endProgram(location);
	
	assert(functionCallsArray != NULL);
	assert(locationsArray != NULL);
	
	// Free the function arrays
	DynArray_map(locationsArray, freeLocations, NULL);
	DynArray_free(locationsArray);
	DynArray_free(functionCallsArray);
}

/*---------------- Util -----------------------*/

static int locationsAreEqual(YYLTYPE location, YYLTYPE* other) {
	if (other == NULL) { 
		return 0;
	}
	
	/*	fprintf(stderr, "%s vs %s\n", location.filename, (*other).filename);
	 
	 if (strcmp(location.filename, (*other).filename) != 0) {
	 fprintf(stderr, "bad filename\n");
	 return 0;
	 }
	 */	
	if (location.first_line != other->first_line) {
		return 0;
	}
	
	if (location.first_column != other->first_column) {
		return 0;
	}
	
	if (location.last_line != other->last_line) {
		return 0;
	}
	
	if (location.last_column != other->last_column) {
		return 0;
	}
	
	return 1;
}

static void addFunctionAndLocationToStacks(void (*f)(YYLTYPE), YYLTYPE location) {
	DynArray_add(functionCallsArray, (void*)f);
	DynArray_add(locationsArray, allocateLocation(location));
}


/*---------------------------------------------*/
static int inDeclarator = 0;

void h_registerIdentifier(YYLTYPE location) {
	
}

void h_beginDirectDeclarator(YYLTYPE location) {
	assert(functionCallsArray != NULL);
	assert(locationsArray != NULL);
	
	inDeclarator++;
	addFunctionAndLocationToStacks(beginDeclarator,location);
}

void h_endDirectDeclarator(YYLTYPE location) {
	inDeclarator--;
	addFunctionAndLocationToStacks(endDeclarator, location);
}

static void h_registerDeclarationSpecifiers(YYLTYPE location) {
	lyyerror(location, "registering declaration specifier");
}

void h_beginFunctionDefinition(YYLTYPE location) {
	beginFunctionDefinition(location);
	
	/* send appropriate calls for declaration_specifier, declarator etc */
	int len, i;
	void (*func)(YYLTYPE);
	YYLTYPE* loc = NULL;
	DynArray_T locations = DynArray_new(0);
	DynArray_T functions = DynArray_new(0);
	
	/* pop off function/location pairs until location matches
	   input location (the first declaration specifier) */
	while (!locationsAreEqual(location, loc)) {
		len = DynArray_getLength(locationsArray) - 1;
		loc = DynArray_removeAt(locationsArray, len);
		assert(loc != NULL);
		
		func = DynArray_removeAt(functionCallsArray, len);
		assert(func != NULL);
	
		DynArray_add(locations, loc);
		DynArray_add(functions, func);
	}
	
	for (i = DynArray_getLength(locations) - 1; i >=0 ; i--) {
		loc = DynArray_removeAt(locations, i);
		func = DynArray_removeAt(functions, i);
		
		(*func)(*loc);
		freeLocations(loc, NULL);
	}
	
	DynArray_free(locations);
	DynArray_free(functions);
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
	//h_registerDeclarationSpecifiers(location);
	addFunctionAndLocationToStacks(h_registerDeclarationSpecifiers, location);
}

void h_registerExtern(YYLTYPE location) {h_registerStorageClass(location);}
void h_registerStatic(YYLTYPE location) {h_registerStorageClass(location);}
void h_registerAuto(YYLTYPE location) {h_registerStorageClass(location);}
void h_registerRegister(YYLTYPE location) {h_registerStorageClass(location);}