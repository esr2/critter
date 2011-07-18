/*
 *  comments.h
 *  
 *
 *  Created by Erin Rosenbaum on 2/2/11.
 *  Copyright 2011 CritTer. All rights reserved.
 *
 */

#ifndef COMMENTS_INCLUDED
#define COMMENTS_INCLUDED

#include "c.tab.h"
#include "dynarray.h"

void comment_intializeComments();
void comment_freeComments();
void comment_beginComment(YYLTYPE location, int contiguous);
void comment_registerComment(char* text);
void comment_endComment(YYLTYPE location);
char* comment_getCommentAbove(YYLTYPE location, int compareDistance, YYLTYPE *returnLocation);
char* comment_getCommentBelow(YYLTYPE location, int compareDistance, YYLTYPE *returnLocation);
char* comment_getCommentWithin(YYLTYPE location, YYLTYPE *returnLocation);
char* comment_getCommentOnSameLine(YYLTYPE location, YYLTYPE *returnLocation);  
int comment_isContentful(const char* text);
char* strstrInsensitive(const char* haystack, const char* needle);
int comment_contains(const char* text, const char* needle, int ignoreCase);

#endif