/*
 *  comments.c
 *  
 *
 *  Created by Erin Rosenbaum on 2/2/11.
 *  Copyright 2011 CritTer. All rights reserved.
 *
 */

#include "comments.h"
#include "strings.h"
#include <assert.h>
#include "locations.h"
#include <stdlib.h>

DynArray_T commentLocations;
DynArray_T commentTexts;

#define MAX_COMMENT_LENGTH 2000
static char lastCommentText[MAX_COMMENT_LENGTH];
static YYLTYPE *lastCommentLocation = NULL;

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
	
	freeLocations(lastCommentLocation, NULL);
}

void comment_beginComment(YYLTYPE location, int contiguous) {
	assert(DynArray_getLength(commentLocations) == DynArray_getLength(commentTexts));
	
	/* check if the comments are adjacent, if they are concatenate this
	   to the old comment */
	if (contiguous && 
			lastCommentLocation &&
			lastCommentLocation->filename &&
			strcmp(lastCommentLocation->filename, location.filename) == 0 &&
			location.first_line - lastCommentLocation->last_line <= 1) {
		/* get and remove the last comment */
		int len = DynArray_getLength(commentLocations) - 1;
		free(DynArray_removeAt(commentTexts, len));
		freeLocations(DynArray_removeAt(commentLocations, len), NULL);
		
		/* add a new line to the end of the comment */
		strcat(lastCommentText, "\n");
	} else {
		/* reset lastComment */
		int i;
		for (i = 0; i < MAX_COMMENT_LENGTH; i++) {
			lastCommentText[i] = '\0';
		}
		
		lastCommentLocation = copyLocation(lastCommentLocation, &location);
	}
	
	assert(DynArray_getLength(commentLocations) == DynArray_getLength(commentTexts));
}

void comment_registerComment(char* text) {
	size_t size = MAX_COMMENT_LENGTH - strlen(lastCommentText);
	strncat(lastCommentText, text, size);
}
	
void comment_endComment(YYLTYPE location) {
	assert(DynArray_getLength(commentLocations) == DynArray_getLength(commentTexts));
	assert(lastCommentLocation);
	
	lastCommentLocation->last_line = location.last_line;
	lastCommentLocation->last_column = location.last_column;
	
	// add the latest comment to the arrays
	char *text = strdup(lastCommentText);
	YYLTYPE *loc = allocateLocation(*lastCommentLocation);
	
	DynArray_add(commentTexts, text);
	DynArray_add(commentLocations, loc);
	
	assert(DynArray_getLength(commentLocations) == DynArray_getLength(commentTexts));
}

static char* getComment(YYLTYPE *location, 
						int (*compareFunction)(const void*, const void*),
						YYLTYPE *returnLocation) {
	int index;
	char* text = NULL;
	
	index = DynArray_search(commentLocations, location, compareFunction);
	
	if (index != -1) {
		text = (char*)DynArray_get(commentTexts, index);
		if (returnLocation != NULL) {
			*returnLocation = *(YYLTYPE*)DynArray_get(commentLocations, index);
		}
	}
	
	return text;
	
}

/**
 * Find the comment within compareDistance above the location. Returns the text
 * of the comment or NULL if not found.
 */
char* comment_getCommentAbove(YYLTYPE location, int compareDistance, YYLTYPE *returnLocation) {
	setCompareDistance(compareDistance);
	return getComment(&location, isLocationAbove, returnLocation);
}

/**
 * Find the comment within compareDistance below the location. Returns the text
 * of the comment or NULL if not found.
 */
char* comment_getCommentBelow(YYLTYPE location, int compareDistance, YYLTYPE *returnLocation) {
	setCompareDistance(compareDistance);
	return getComment(&location, isLocationBelow, returnLocation);
}

/**
 * Find the comment within the location. Returns the text of the comment or
 * NULL if not found.
 */
char* comment_getCommentWithin(YYLTYPE location, YYLTYPE *returnLocation) {
	if (location.first_line > location.last_line) {
		returnLocation = NULL;
		return NULL;
	}
	
	if (location.first_line == location.last_line &&
		location.first_column > location.last_column) {
		returnLocation = NULL;
		return NULL;
	}
	
	return getComment(&location, isLocationWithin, returnLocation);
}

/**
 * Checks if the given text contains a letter (most likely meaning it has words).
 * Returns -1 on error, 0 on false and 1 on true;
 */
int comment_isContentful(const char* text) {
	if (text == NULL) {
		return -1;
	}
	
	while (*text != '\0') {
		if (isalpha(*text)) {
			return 1;
		}
		text++;
	}
	
	return 0;
}

/**
 * Case insensitive form of strstr. Inspired by http://www.daniweb.com/code/snippet216564.html
 */
char* strstrInsensitive(const char* haystack, const char* needle) {
	char* hay = (char*)haystack;
	
	while (*hay != '\0') {
		if (toupper(*hay) == toupper(*needle)) {
			/* matched starting character, loop through rest of needle */
			const char *h, *n;
			for (h = hay, n = needle; *h && *n; ++h, ++n )
			{
				if ( toupper(*h) != toupper(*n) )
				{
					break;
				}
			}
			if (*n == '\0') {
				/* matched all of 'needle' to null termination, 
				   return the start of the match */
				return hay;
			}
		}
		hay++;
	}
	
	return NULL;
}

/**
 * Checks to see if the comment contains the needle.
 */
int comment_contains(const char* text, const char* needle, int ignoreCase) {
	if (text == NULL || needle == NULL) {
		return -1;
	}
	
	char* position = ignoreCase ? strstrInsensitive(text, needle) : strstr(text, needle);
	
	return (position != NULL);
}
