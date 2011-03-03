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

static void (*lastCalledFunction)(YYLTYPE);
static int lastCalledWasComment = 0;

void (*lastCalled_get())(YYLTYPE) {
	return lastCalledFunction;
}

void lastCalled_set(void (*func)(YYLTYPE)) {
	lastCalledFunction = func;
	lastCalledWasComment = 0;
}

void lastCalled_setEndComment() {
	lastCalledWasComment = 1;
}

int lastCalled_getWasComment() {
	return lastCalledWasComment;
}

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
	tooManyFunctionsInFile(location, END);
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
	comment_beginComment(location, lastCalled_getWasComment());
}

void registerComment(char* text) {
	comment_registerComment(text);
}

void endComment(YYLTYPE location) {
	comment_endComment(location);
	lastCalled_setEndComment();
}

/*--------- Function -----------------------*/
void beginFunctionDefinition(YYLTYPE location) {
	checkForComment(location, "function");
	isFunctionTooLongByStatements(location, BEGINNING);
	globalHasComment(location, BEGINNING);
	tooManyFunctionsInFile(location, MIDDLE);
	validateComment(location, BEGIN_FUNCTION, NULL);
}

void endFunctionDefinition(YYLTYPE location) {
	isFunctionTooLongByLines(location);
	isFunctionTooLongByStatements(location, END);
	globalHasComment(location, END);
	validateComment(location, END_FUNCTION, NULL);
}

void beginParameterList(YYLTYPE location) {
	tooManyParameters(location, BEGINNING);
	useEnumNotConstOrDefine(location, BEGINNING);
	validateComment(location, BEGIN_PARAM_LIST, NULL);
}

void registerParameter(YYLTYPE location) {
	tooManyParameters(location, MIDDLE);
}

void endParameterList(YYLTYPE location) {
	tooManyParameters(location, END);
	useEnumNotConstOrDefine(location, END);
	validateComment(location, END_PARAM_LIST, NULL);
}

/*-- Iteration (not called through hook) ---*/
void beginWhile(YYLTYPE location) {
	lastCalled_set(beginWhile);
}

void endWhile(YYLTYPE location) {
	hasBraces(location, "while");
	lastCalled_set(endWhile);
	isLoopTooLong(location);
}

void beginDoWhile(YYLTYPE location) {
	lastCalled_set(beginDoWhile);
}

void endDoWhile(YYLTYPE location) {
	hasBraces(location, "doWhile");
	lastCalled_set(endDoWhile);
	isLoopTooLong(location);
}

void beginFor(YYLTYPE location) {
	lastCalled_set(beginFor);
}

void endFor(YYLTYPE location) {
	hasBraces(location, "for");
	lastCalled_set(endFor);
	isLoopTooLong(location);
}

/*-- Selection (not called through hook) ---*/
void beginIf(YYLTYPE location) {
	lastCalled_set(beginIf);
}

void endIf(YYLTYPE location) {
	hasBraces(location, "if");
	checkIfElsePlacement(location, BEGINNING);
	lastCalled_set(endIf);
}

void beginElse(YYLTYPE location) {
	lastCalled_set(beginElse);
	checkIfElsePlacement(location, END);
}

void endElse(YYLTYPE location) {
	hasBraces(location, "else");
	lastCalled_set(endElse);
}

void beginSwitch(YYLTYPE location) {
	lastCalled_set(beginSwitch);
	switchHasDefault(location, BEGINNING);
	switchCasesHaveBreaks(location, BEGINNING, -1);
}

void registerDefault(YYLTYPE location) {
	lastCalled_set(registerDefault);
	switchHasDefault(location, MIDDLE);
	switchCasesHaveBreaks(location, MIDDLE, 1);
}

void registerCase(YYLTYPE location) {
	lastCalled_set(registerCase);
	switchCasesHaveBreaks(location, MIDDLE, 1);
}

void endSwitch(YYLTYPE location) {
	lastCalled_set(endSwitch);
	switchHasDefault(location, END);
	switchCasesHaveBreaks(location, END, -1);
}

/*----- Statements (some through hook) ----------*/
void registerIdentifier(YYLTYPE location, char* identifier) {
	isVariableNameTooShort(location, MIDDLE, identifier);
	validateComment(location, FOUND_IDENTIFIER, identifier);
}

void registerConstant(YYLTYPE location, char* constant) {
	isMagicNumber(location, MIDDLE, constant);
}

/* location points to first bracket */
void beginCompoundStatement(YYLTYPE location) {
	isCompoundStatementEmpty(location, BEGINNING);
	lastCalled_set(beginCompoundStatement);
	tooDeeplyNested(location, BEGINNING);
}

/* location points to the entire statement */
void endCompoundStatement(YYLTYPE location) {
	isCompoundStatementEmpty(location, END);
	lastCalled_set(endCompoundStatement);
	tooDeeplyNested(location, END);
}

void beginDeclaration(YYLTYPE location) {
	isMagicNumber(location, BEGINNING, NULL);
	isVariableNameTooShort(location, BEGINNING, NULL);
}

void endDeclaration(YYLTYPE location) {
	isMagicNumber(location, END, NULL);
	globalHasComment(location, MIDDLE);
	isVariableNameTooShort(location, END, NULL);
}

void beginStatement(YYLTYPE location) {
	
}

void endStatement(YYLTYPE location) {
	isFunctionTooLongByStatements(location, MIDDLE);
}

void registerDefineIntegralType(YYLTYPE location) {
	useEnumNotConstOrDefine(location, MIDDLE);
}

void registerConst(YYLTYPE location) {
	useEnumNotConstOrDefine(location, MIDDLE);
}

/* Jump Statements */
void registerGoto(YYLTYPE location) {
	lastCalled_set(registerGoto);
	neverUseGotos(location);
}

void registerContinue(YYLTYPE location) {
	lastCalled_set(registerContinue);
}

void registerBreak(YYLTYPE location) {
	lastCalled_set(registerBreak);
	switchCasesHaveBreaks(location, MIDDLE, 0);
}

void registerReturn(YYLTYPE location) {
	lastCalled_set(registerReturn);
}

void registerReturnSomething(YYLTYPE location) {
	lastCalled_set(registerReturnSomething);
	validateComment(location, RETURNING, NULL);
}