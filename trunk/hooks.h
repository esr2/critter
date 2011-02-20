/*
 *  hooks.h
 *  
 *
 *  Created by Erin Rosenbaum on 2/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef HOOKS_INCLUDED
#define HOOKS_INCLUDED

#include "c.tab.h"

void h_beginFile(char* filename);
void h_endFile(YYLTYPE location);
void h_beginProgram();
void h_endProgram(YYLTYPE location);

void h_registerDefineIntegralType(YYLTYPE location);

void h_registerIdentifier(YYLTYPE location);
void h_registerIdentifierText(char* identifier);

void h_endDeclaration(YYLTYPE location);
void h_beginDirectDeclarator(YYLTYPE location);
void h_endDirectDeclarator(YYLTYPE location);

void h_beginFunctionDefinition(YYLTYPE location);

void h_beginParameterList(YYLTYPE location);
void h_registerParameter(YYLTYPE location);
void h_endParameterList(YYLTYPE location);

void h_registerTypedef(YYLTYPE location);

void h_registerVoid(YYLTYPE location);
void h_registerChar(YYLTYPE location);
void h_registerShort(YYLTYPE location);
void h_registerInt(YYLTYPE location);
void h_registerLong(YYLTYPE location);
void h_registerFloat(YYLTYPE location);
void h_registerDouble(YYLTYPE location);
void h_registerSigned(YYLTYPE location);
void h_registerUnsigned(YYLTYPE location);
void h_registerStructUnionSpecifier(YYLTYPE location);
void h_registerEnumSpecifier(YYLTYPE location);
void h_registerTypeName(YYLTYPE location);

void h_registerConst(YYLTYPE location);
void h_registerVolatile(YYLTYPE location);

void h_registerExtern(YYLTYPE location);
void h_registerStatic(YYLTYPE location);
void h_registerAuto(YYLTYPE location);
void h_registerRegister(YYLTYPE location);

#endif HOOKS_INCLUDED