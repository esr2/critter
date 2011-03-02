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
	lastCalledFunction = beginWhile;
}

void endWhile(YYLTYPE location) {
	isLoopEmpty(location, beginWhile, "while");
	hasBraces(location);
	lastCalledFunction = endWhile;
	isLoopTooLong(location);
}

void beginDoWhile(YYLTYPE location) {
	lastCalledFunction = beginDoWhile;
}

void endDoWhile(YYLTYPE location) {
	isLoopEmpty(location, beginDoWhile, "doWhile");
	hasBraces(location);
	lastCalledFunction = endDoWhile;
	isLoopTooLong(location);
}

void beginFor(YYLTYPE location) {
	lastCalledFunction = beginFor;
}

void endFor(YYLTYPE location) {
	isLoopEmpty(location, beginFor, "for");
	hasBraces(location);
	lastCalledFunction = endFor;
	isLoopTooLong(location);
}

/*-- Selection (not called through hook) ---*/
void beginIf(YYLTYPE location) {
	lastCalledFunction = beginIf;
}

void endIf(YYLTYPE location) {
	hasBraces(location);
	checkIfElsePlacement(location, BEGINNING);
	lastCalledFunction = endIf;
}

void beginElse(YYLTYPE location) {
	lastCalledFunction = beginElse;
	checkIfElsePlacement(location, END);
}

void endElse(YYLTYPE location) {
	hasBraces(location);
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
	isVariableNameTooShort(location, MIDDLE, identifier);
	validateComment(location, FOUND_IDENTIFIER, identifier);
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

void registerReturnSomething(YYLTYPE location) {
	lastCalledFunction = registerReturnSomething;
	validateComment(location, RETURNING, NULL);
}