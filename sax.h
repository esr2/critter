/*
 *  sax.h
 *  
 *
 *  Created by Erin Rosenbaum on 2/2/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SAX_INCLUDED
#define SAX_INCLUDED

#include "c.tab.h"

void (*lastCalled_get())(YYLTYPE);
void lastCalled_set(void (*func)(YYLTYPE));
void lastCalled_setEndComment();
int lastCalled_getWasComment();

/*--------- Overall -----------------------*/
void beginFile(char* filename);
void endFile(YYLTYPE location);

void beginProgram();
void endProgram(YYLTYPE location);

/*--------- Comments -----------------------*/
void beginComment(YYLTYPE location);
void registerComment(char* text);
void endComment(YYLTYPE location);

/*--------- Function -----------------------*/
void beginFunctionDefinition(YYLTYPE location);
void endFunctionDefinition(YYLTYPE location);

void beginParameterList(YYLTYPE location);
void registerParameter(YYLTYPE location);
void endParameterList(YYLTYPE location);

/*--------- Iteration -----------------------*/
void beginWhile(YYLTYPE location);
void endWhile(YYLTYPE location);
void beginDoWhile(YYLTYPE location);
void endDoWhile(YYLTYPE location);

void beginFor(YYLTYPE location);
void endFor(YYLTYPE location);

/*--------- Selection -----------------------*/
void beginIf(YYLTYPE location);
void endIf(YYLTYPE location);
void beginElse(YYLTYPE location);
void endElse(YYLTYPE location);

void beginSwitch(YYLTYPE location);
void registerDefault(YYLTYPE location);
void registerCase(YYLTYPE location);
void endSwitch(YYLTYPE location);

/*--------- Statements -----------------------*/
void registerIdentifier(YYLTYPE location, char* identifier);
void registerConstant(YYLTYPE location, char* constant);

void beginCompoundStatement(YYLTYPE location);
void endCompoundStatement(YYLTYPE location);

void beginDeclaration(YYLTYPE location);
void endDeclaration(YYLTYPE location);

void beginStatement(YYLTYPE location);
void endStatement(YYLTYPE location);

void beginStructDefinition(YYLTYPE location);
void registerStructField(YYLTYPE location);
void endStructDefinition(YYLTYPE location);

void registerPointer(YYLTYPE location);

void registerDefineIntegralType(YYLTYPE location);

void registerConst(YYLTYPE location);

void registerGoto(YYLTYPE location);
void registerContinue(YYLTYPE location);
void registerBreak(YYLTYPE location);
void registerReturn(YYLTYPE location);

#endif