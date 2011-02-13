/*
 *  comments.h
 *  
 *
 *  Created by Erin Rosenbaum on 2/2/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef COMMENTS_INCLUDED
#define COMMENTS_INCLUDED

#include "c.tab.h"
#include "dynarray.h"

void comment_intializeComments();
void comment_freeComments();
void comment_beginComment(YYLTYPE location);
void comment_registerComment(char* text);
void comment_endComment(YYLTYPE location);
char* comment_getCommentCloseTo(YYLTYPE location, int compareDistance);

#endif COMMENTS_INCLUDED