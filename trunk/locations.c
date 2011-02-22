/*
 *  locations.c
 *  
 *
 *  Created by Erin Rosenbaum on 2/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "locations.h"
#include <stdlib.h>
#include <strings.h>
#include <assert.h>

int locationsAreEqual(YYLTYPE location, YYLTYPE* other, int checkAll) {
	if (other == NULL) { 
		return 0;
	}
	
	if (strcmp(location.filename, (*other).filename) != 0) {
		return 0;
	}
	
	if (location.first_line != other->first_line) {
		return 0;
	}
	
	if (location.first_column != other->first_column) {
		return 0;
	}
	
	if (checkAll && location.last_line != other->last_line) {
		return 0;
	}
	
	if (checkAll && location.last_column != other->last_column) {
		return 0;
	}
	
	return 1;
}

int locationIsLessOrEqual(YYLTYPE location, YYLTYPE* other, int checkAll) {
	if (strcmp(location.filename, (*other).filename) != 0) {
		return 0;
	}
	
	if (location.first_line > other->first_line) {
		return 0;
	}
	
	if (location.first_column > other->first_column) {
		return 0;
	}
	
	if (checkAll && location.last_line > other->last_line) {
		return 0;
	}
	
	if (checkAll && location.last_column > other->last_column) {
		return 0;
	}
	
	return 1;
}

void freeLocations(void* element, void* extra) { 
	YYLTYPE *location = (YYLTYPE *)element;
	if (location->filename != NULL) { free(location->filename); }
	free(element); 
}

YYLTYPE* allocateLocation(YYLTYPE location) {
	YYLTYPE *loc = malloc(sizeof(YYLTYPE));
	/* intialize filename to some temporary space which will be immediately freed
	 * from copyLocation */
	loc->filename = (char*)malloc(sizeof(char) * 10); 
	copyLocation(loc, &location);
	
	return loc;
}

YYLTYPE* copyLocation(YYLTYPE* dest, YYLTYPE* src) {
	if (dest == NULL) {
		dest = malloc(sizeof(YYLTYPE));
		dest->filename = NULL;
	} else {
		free(dest->filename);
	}
	
	dest->filename = (char*)malloc(sizeof(char) * strlen(src->filename));
	strncpy(dest->filename, src->filename, strlen(src->filename)+1);
	dest->first_line = src->first_line;
	dest->first_column = src->first_column;
	dest->last_line = src->last_line;
	dest->last_column = src->last_column;
	
	return dest;
}

static int COMPARE_DISTANCE;

void setCompareDistance(int distance) {
	COMPARE_DISTANCE = distance;
}

/**
 * Compare two locations - meant to be used by Dynarray_search. Returns 0 if
 * equal and 1 if not.
 */
int compareLocations(const void *element1, const void *element2) {
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
