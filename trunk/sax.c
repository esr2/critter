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
#include "comments.h"

/**
 * Called at the beginning of each file before parsing 
 * begins.
 */
void beginFile(char* filename) {
	
}

/**
 * Called at the end of each file. Location.first_line 
 * = last_line.
 */
void endFile(YYLTYPE location) {
	isFileTooLong(location);
	
	/* Make one last call to tooManyParameters to make 
	 * sure that if the last function has an error, the 
	 * error actually gets displayed. */
	location.first_line++;
	location.last_line++;
	tooManyParameters(location);
	
}

/**
 * Called at the beginning of the program execution before
 * parsing begins.
 */
void beginProgram(char* filename) {
	comment_intializeComments();
}

/**
 * Called at the end of the program execution.
 */
void endProgram(YYLTYPE location) {
	comment_freeComments();
}

void beginComment(YYLTYPE location) {
	comment_beginComment(location);
}

void registerComment(char* text) {
	comment_registerComment(text);
}

void endComment(YYLTYPE location) {
	comment_endComment(location);
}

void beginSwitch(YYLTYPE location) {
	switchHasDefault(location, -1);
}

void registerDefault(YYLTYPE location) {
	switchHasDefault(location, 0);
}

void endSwitch(YYLTYPE location) {
	switchHasDefault(location, 1);
}
