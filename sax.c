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

/*--------- Overall -----------------------*/
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
}

/**
 * Called at the beginning of the program execution before
 * parsing begins.
 */
void beginProgram() {
	comment_intializeComments();
}

/**
 * Called at the end of the program execution.
 */
void endProgram(YYLTYPE location) {
	comment_freeComments();
}

/*--------- Comments -----------------------*/
void beginComment(YYLTYPE location) {
	comment_beginComment(location);
}

void registerComment(char* text) {
	comment_registerComment(text);
}

void endComment(YYLTYPE location) {
	comment_endComment(location);
}

/*--------- Function -----------------------*/
void beginFunctionDefinition(YYLTYPE location) {
	checkForComment(location, "function");
	globalHasComment(location, BEGINNING);
}

void endFunctionDefinition(YYLTYPE location) {
	isFunctionTooLong(location);
	globalHasComment(location, END);
}

void beginParameterList(YYLTYPE location) {
	tooManyParameters(location, BEGINNING);
	useEnumNotConstOrDefine(location, BEGINNING);
}

void registerParameter(YYLTYPE location) {
	tooManyParameters(location, MIDDLE);
}

void endParameterList(YYLTYPE location) {
	tooManyParameters(location, END);
	useEnumNotConstOrDefine(location, END);
}

/*--------- Iteration -----------------------*/
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

/*--------- Selection -----------------------*/
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
	switchCasesHaveBreaks(location, BEGINNING, -1);
}

void registerDefault(YYLTYPE location) {
	switchHasDefault(location, MIDDLE);
	switchCasesHaveBreaks(location, MIDDLE, 1);
}

void registerCase(YYLTYPE location) {
	switchCasesHaveBreaks(location, MIDDLE, 1);
}

void endSwitch(YYLTYPE location) {
	switchHasDefault(location, END);
	switchCasesHaveBreaks(location, END, -1);
}

/*--------- Statements -----------------------*/
void registerIdentifier(YYLTYPE location, char* identifier) {
	isVariableNameTooShort(location, identifier);
}

void registerConstant(YYLTYPE location, char* constant) {
	isMagicNumber(location, MIDDLE, constant);
}

/* location points to first bracket */
void beginCompoundStatement(YYLTYPE location) {
	tooDeeplyNested(location, BEGINNING);
}

/* location points to the entire statement */
void endCompoundStatement(YYLTYPE location) {
	hasBraces(location, MIDDLE);
	tooDeeplyNested(location, END);
}

void beginDeclaration(YYLTYPE location) {
	isMagicNumber(location, BEGINNING, NULL);
	globalHasComment(location, MIDDLE);
}

void endDeclaration(YYLTYPE location) {
	isMagicNumber(location, END, NULL);
}

void beginStatement(YYLTYPE location) {
	
}

void endStatement(YYLTYPE location) {
	
}

void registerDefineIntegralType(YYLTYPE location) {
	useEnumNotConstOrDefine(location, MIDDLE);
}

void registerConst(YYLTYPE location) {
	useEnumNotConstOrDefine(location, MIDDLE);
}

/* Jump Statements */
void registerGoto(YYLTYPE location) {
	neverUseGotos(location);
}

void registerContinue(YYLTYPE location) {
	
}

void registerBreak(YYLTYPE location) {
	switchCasesHaveBreaks(location, MIDDLE, 0);
}

void registerReturn(YYLTYPE location) {
	
}