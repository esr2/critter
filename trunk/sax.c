/*
 *  sax.c
 *  
 *
 *  Created by Erin Rosenbaum on 2/2/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "sax.h"
#include "checks.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Called at the beginning of each file before parsing begins.
 */
void beginningOfFile(char* filename) {
	
}

/**
 * Called at the end of each file. Location.first_line = last_line.
 */
void endOfFile(YYLTYPE location) {
	isFileTooLong(location);
	
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
	// These have to be managed on a program level because files are nested
	commentTexts = DynArray_new(0);
	commentLocations = DynArray_new(0);
}

static void freeLocations(void* element, void* extra) { 
	assert(element != NULL);
	YYLTYPE *location = (YYLTYPE *)element;
	if (location->filename != NULL) { free(location->filename); }
	free(element); 
}
static void freeComments(void* element, void* extra) { free(element); }

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
