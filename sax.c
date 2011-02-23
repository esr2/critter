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
	/* check for a file level comment - start by creating a location
	   to compare against. Do this at the end of the file so that all the
	   comments have been found already. */
	YYLTYPE begin = {1, 1, 1, 1, location.filename};
	checkForComment(begin, "file");
	
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

/*--- Comments (not called through hook) ---*/
void beginComment(YYLTYPE location) {
	comment_beginComment(location, (lastCalledFunction == endComment));
	lastCalledFunction = beginComment;
}

void registerComment(char* text) {
	comment_registerComment(text);
	/* don't setup lastCalledFunction because it provides no context */
}

void endComment(YYLTYPE location) {
	comment_endComment(location);
	lastCalledFunction = endComment;
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

/*-- Iteration (not called through hook) ---*/
void beginWhile(YYLTYPE location) {
	lastCalledFunction = beginWhile;
	hasBraces(location, BEGINNING);
}

void endWhile(YYLTYPE location) {
	isLoopEmpty(location, beginWhile, "while");
	lastCalledFunction = endWhile;
	hasBraces(location, END);
	isLoopTooLong(location);
}

void beginDoWhile(YYLTYPE location) {
	lastCalledFunction = beginDoWhile;
	hasBraces(location, BEGINNING);
}

void endDoWhile(YYLTYPE location) {
	isLoopEmpty(location, beginDoWhile, "doWhile");
	lastCalledFunction = endDoWhile;
	hasBraces(location, END);
	isLoopTooLong(location);
}

void beginFor(YYLTYPE location) {
	lastCalledFunction = beginFor;
	hasBraces(location, BEGINNING);
}

void endFor(YYLTYPE location) {
	isLoopEmpty(location, beginFor, "for");
	lastCalledFunction = endFor;
	hasBraces(location, END);
	isLoopTooLong(location);
}

/*-- Selection (not called through hook) ---*/
void beginIf(YYLTYPE location) {
	lastCalledFunction = beginIf;
	hasBraces(location, BEGINNING);
}

void endIf(YYLTYPE location) {
	lastCalledFunction = endIf;
	hasBraces(location, END);
}

void beginElse(YYLTYPE location) {
	lastCalledFunction = beginElse;
}

void endElse(YYLTYPE location) {
	lastCalledFunction = endElse;
}

void beginSwitch(YYLTYPE location) {
	lastCalledFunction = beginSwitch;
	switchHasDefault(location, BEGINNING);
	switchCasesHaveBreaks(location, BEGINNING, -1);
}

void registerDefault(YYLTYPE location) {
	lastCalledFunction = registerDefault;
	switchHasDefault(location, MIDDLE);
	switchCasesHaveBreaks(location, MIDDLE, 1);
}

void registerCase(YYLTYPE location) {
	lastCalledFunction = registerDefault;
	switchCasesHaveBreaks(location, MIDDLE, 1);
}

void endSwitch(YYLTYPE location) {
	lastCalledFunction = endSwitch;
	switchHasDefault(location, END);
	switchCasesHaveBreaks(location, END, -1);
}

/*----- Statements (some through hook) ----------*/
void registerIdentifier(YYLTYPE location, char* identifier) {
	isVariableNameTooShort(location, identifier);
}

void registerConstant(YYLTYPE location, char* constant) {
	isMagicNumber(location, MIDDLE, constant);
}

/* location points to first bracket */
void beginCompoundStatement(YYLTYPE location) {
	isCompoundStatementEmpty(location, BEGINNING);
	lastCalledFunction = beginCompoundStatement;
	tooDeeplyNested(location, BEGINNING);
}

/* location points to the entire statement */
void endCompoundStatement(YYLTYPE location) {
	isCompoundStatementEmpty(location, END);
	lastCalledFunction = endCompoundStatement;
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
	lastCalledFunction = registerGoto;
	neverUseGotos(location);
}

void registerContinue(YYLTYPE location) {
	lastCalledFunction = registerContinue;
}

void registerBreak(YYLTYPE location) {
	lastCalledFunction = registerBreak;
	switchCasesHaveBreaks(location, MIDDLE, 0);
}

void registerReturn(YYLTYPE location) {
	lastCalledFunction = registerReturn;
}