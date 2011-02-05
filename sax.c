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

void beginFunctionDefinition(YYLTYPE location) {
	checkForComment(location);
}

void endFunctionDefinition(YYLTYPE location) {
	isFunctionTooLong(location);
}

void beginWhile(YYLTYPE location) {
	hasBraces(location, BEGINNING);
}

void endWhile(YYLTYPE location) {
	hasBraces(location, END);
}

void beginDoWhile(YYLTYPE location) {
	hasBraces(location, BEGINNING);
}

void endDoWhile(YYLTYPE location) {
	hasBraces(location, END);
}

void beginFor(YYLTYPE location) {
	hasBraces(location, BEGINNING);
}

void endFor(YYLTYPE location) {
	hasBraces(location, END);
}

void beginIf(YYLTYPE location) {
	hasBraces(location, BEGINNING);
}

void endIf(YYLTYPE location) {
	hasBraces(location, END);
}

void beginElse(YYLTYPE location) {
	
}

void endElse(YYLTYPE location) {
	
}

void beginSwitch(YYLTYPE location) {
	switchHasDefault(location, BEGINNING);
}

void registerDefault(YYLTYPE location) {
	switchHasDefault(location, MIDDLE);
}

void endSwitch(YYLTYPE location) {
	switchHasDefault(location, END);
}

/* location points to first bracket */
void beginCompoundStatement(YYLTYPE location) {
	
}

/* location points to the entire statement */
void endCompoundStatement(YYLTYPE location) {
	hasBraces(location, MIDDLE);
}