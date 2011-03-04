/*
 *  locations.h
 *  
 *
 *  Created by Erin Rosenbaum on 2/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef LOCATIONS_INCLUDED
#define LOCATIONS_INCLUDED

#include "c.tab.h"

int locationsAreEqual(YYLTYPE location, YYLTYPE* other, int checkAll);
int locationIsLessOrEqual(YYLTYPE location, YYLTYPE* other, int checkAll);
void freeLocations(void* element, void* extra);
YYLTYPE* allocateLocation(YYLTYPE location);
YYLTYPE* copyLocation(YYLTYPE* dest, YYLTYPE* src);
void setCompareDistance(int distance);
int isLocationAbove(const void *element1, const void *element2);
int isLocationBelow(const void *element1, const void *element2);
int isLocationWithin(const void *element1, const void *element2);

#endif LOCATIONS_INCLUDED