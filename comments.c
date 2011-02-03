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
	int i;
	for (i = 0; i < MAX_COMMENT_LENGTH; i++) {
		lastCommentText[i] = '\0';
	}
	
	lastCommentLocation = location;
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