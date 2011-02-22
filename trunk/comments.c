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
#include <assert.h>
#include "locations.h"
#include <stdlib.h>

DynArray_T commentLocations;
DynArray_T commentTexts;

void comment_intializeComments() {
	// These have to be managed on a program level because files are nested
	commentTexts = DynArray_new(0);
	commentLocations = DynArray_new(0);
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
	/* check if the comments are adjacent, if they are concatenate this
	   to the old comment */
	if (lastCommentLocation.filename &&
			strcmp(lastCommentLocation.filename, location.filename) == 0 &&
			location.first_line - lastCommentLocation.last_line <= 1) {
		/* get and remove the last comment */
		int len = DynArray_getLength(commentLocations) - 1;
		char *text = DynArray_removeAt(commentTexts, len);
		assert(text != NULL);
		
		/* copy lastCommentText and then free */
		strcpy(lastCommentText, text);
		free(text);
		freeLocations(DynArray_removeAt(commentLocations, len), NULL);
		
		/* add a new line to the end of the comment */
		strcat(lastCommentText, "\n");
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
	
	YYLTYPE *loc = allocateLocation(location);
	
	DynArray_add(commentTexts, text);
	DynArray_add(commentLocations, loc);
}

/**
 * Find the comment compareDistance close to location. Returns the text
 * of the comment or NULL if not found.
 */
char* comment_getCommentCloseTo(YYLTYPE location, int compareDistance) {
	int index;
	char* text = NULL;
	
	setCompareDistance(compareDistance);
	index = DynArray_search(commentLocations, &location, compareLocations);
	
	if (index != -1) {
		text = (char*)DynArray_get(commentTexts, index);
	}
	
	return text;
}
