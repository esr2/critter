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
void endSwitch(YYLTYPE location);

/*--------- Statements -----------------------*/
void beginCompoundStatement(YYLTYPE location);
void endCompoundStatement(YYLTYPE location);

#endif