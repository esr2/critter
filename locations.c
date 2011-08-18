/*
 *  locations.c
 *  
 *
 *  Created by Erin Rosenbaum on 2/21/11.
 *  Copyright 2011 CritTer. All rights reserved.
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

int locationIsBeforeOrEqual(YYLTYPE location, YYLTYPE* other, int checkAll) {
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
  if (location == NULL) return;
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
	
	dest->filename = strdup(src->filename);
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
 * Is the comment within compareDistance above the sought location?
 * Returns 0 if valid and 1 if not (to comply with dynarray_search).
 */
int isLocationAbove(const void *element1, const void *element2) {
	// taking advantage of how dynarray compares elements
	YYLTYPE *commentLocation = (YYLTYPE*)element1;
	YYLTYPE *soughtLocation = (YYLTYPE*)element2;
	
	assert(commentLocation != NULL);
	assert(soughtLocation != NULL);
	
	assert(commentLocation->filename != NULL);
	assert(soughtLocation->filename != NULL);
	
	if (strcmp(commentLocation->filename, soughtLocation->filename) == 0) {
		int distance = soughtLocation->first_line - commentLocation->last_line;
		if (distance <= COMPARE_DISTANCE && distance >= 0) {
			return 0;
		}
	}
	
	return 1;
}

/**
 * Is the comment within compareDistance below the sought location?
 * Returns 0 if valid and 1 if not (to comply with dynarray_search).
 */
int isLocationBelow(const void *element1, const void *element2) {
	// taking advantage of how dynarray compares elements
	YYLTYPE *commentLocation = (YYLTYPE*)element1;
	YYLTYPE *soughtLocation = (YYLTYPE*)element2;
	
	assert(commentLocation != NULL);
	assert(soughtLocation != NULL);
	
	assert(commentLocation->filename != NULL);
	assert(soughtLocation->filename != NULL);
	
	if (strcmp(commentLocation->filename, soughtLocation->filename) == 0) {
		int distance = commentLocation->first_line - soughtLocation->last_line;
		if (distance <= COMPARE_DISTANCE && distance >= 0) {
			return 0;
		}
	}
	
	return 1;
}

/**
 * Is the comment within the sought location?
 * Returns 0 if valid and 1 if not (to comply with dynarray_search).
 */
int isLocationWithin(const void *element1, const void *element2) {
	// taking advantage of how dynarray compares elements
	YYLTYPE *commentLocation = (YYLTYPE*)element1;
	YYLTYPE *soughtLocation = (YYLTYPE*)element2;
	
	assert(commentLocation != NULL);
	assert(soughtLocation != NULL);
	
	assert(commentLocation->filename != NULL);
	assert(soughtLocation->filename != NULL);
	
	if (strcmp(commentLocation->filename, soughtLocation->filename) != 0) {
		return 1;
	}
	
	int topDistance = commentLocation->first_line - soughtLocation->first_line;
	
	if (topDistance < 0) {
		/* comment starts above soughtLocation */
		return 1;
	}
	
	if ((topDistance == 0) && 
		(commentLocation->first_column - soughtLocation->first_column < 0)) {
		/* comment starts on the same line as soughtLocation but before the column */
		return 1;
	}
	
	int bottomDistance = soughtLocation->last_line - commentLocation->last_line;
	
	if (bottomDistance < 0) {
		/* comment ends after soughtLocation */
		return 1;
	}
	
	if ((bottomDistance == 0) && 
		(soughtLocation->last_column - commentLocation->last_column < 0)) {
		/* comment ends on the same line as soughtLocation but after the column */
		return 1;
	}
	
	return 0;
}

/**
 * Does the location start on the same line as the sought location?
 * Returns 0 if valid and 1 if not (to comply with dynarray_search).
 */
int doesLocationStartOnTheSameLine(const void *element1, const void *element2) {
	// taking advantage of how dynarray compares elements
	YYLTYPE *commentLocation = (YYLTYPE*)element1;
	YYLTYPE *soughtLocation = (YYLTYPE*)element2;

  assert(commentLocation != NULL);
	assert(soughtLocation != NULL);
	
	assert(commentLocation->filename != NULL);
	assert(soughtLocation->filename != NULL);
  
  if (strcmp(commentLocation->filename, soughtLocation->filename) == 0) {
		int distance = commentLocation->first_line - soughtLocation->last_line;
		if (distance == 0) {
			return 0;
		}
	}
	
	return 1;

}