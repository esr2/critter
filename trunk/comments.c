/*
 *  comments.c
 *  
 *
 *  Created by Erin Rosenbaum on 2/2/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "comments.h"
#include "strings.h"
#include "assert.h"
#include <stdlib.h>

DynArray_T commentLocations;
DynArray_T commentTexts;

void comment_intializeComments() {
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

void comment_freeComments() {
	assert(commentTexts != NULL);
	assert(commentLocations != NULL);
	// Free the comment arrays
	DynArray_map(commentLocations, freeLocations, NULL);
	DynArray_free(commentLocations);
	DynArray_map(commentTexts, freeComments, NULL);
	DynArray_free(commentTexts);
	
}

#define MAX_COMMENT_LENGTH 2000
static char lastCommentText[MAX_COMMENT_LENGTH];
static YYLTYPE lastCommentLocation;

void comment_beginComment(YYLTYPE location) {
	/* check if the comments are adjacent */
	if (lastCommentLocation.filename &&
			strcmp(lastCommentLocation.filename, location.filename) == 0 &&
			location.first_line - lastCommentLocation.last_line <= 1) {
		/* get and remove the last comment */
		int len = DynArray_getLength(commentLocations) - 1;
		char *text = DynArray_removeAt(commentTexts, len);
		assert(text != NULL);
		strcpy(lastCommentText, text);
		free(text);
		free(DynArray_removeAt(commentLocations, len));
		
	} else {
		/* reset lastComment */
		int i;
		for (i = 0; i < MAX_COMMENT_LENGTH; i++) {
			lastCommentText[i] = '\0';
		}
		
		lastCommentLocation = location;
	}
}

void comment_registerComment(char* text) {
	size_t size = MAX_COMMENT_LENGTH - strlen(lastCommentText);
	strncat(lastCommentText, text, size);
}
	
void comment_endComment(YYLTYPE location) {
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
}

static int COMPARE_DISTANCE;

/**
 * Compare two locations - meant to be used by Dynarray_search. Returns 0 if
 * equal and 1 if not.
 */
static int compareLocations(const void *element1, const void *element2) {
	// taking advantage of how dynarray compares elements
	YYLTYPE *commentLocation = (YYLTYPE*)element1;
	YYLTYPE *functionLocation = (YYLTYPE*)element2;
	
	assert(commentLocation != NULL);
	assert(functionLocation != NULL);
	
	assert(commentLocation->filename != NULL);
	assert(functionLocation->filename != NULL);
	
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
 * Find the comment compareDistance close to location. Returns the text
 * of the comment or NULL if not found.
 */
char* comment_getCommentCloseTo(YYLTYPE location, int compareDistance) {
	int index;
	char* text = NULL;
	
	COMPARE_DISTANCE = compareDistance;
	index = DynArray_search(commentLocations, &location, compareLocations);
	
	if (index != -1) {
		text = (char*)DynArray_get(commentTexts, index);
	}
	
	return text;
}
