/*
 *  hooks.c
 *  
 *
 *  Created by Erin Rosenbaum on 2/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "hooks.h"
#include "sax.h"

static int areLocationsAdjacent(YYLTYPE one, YYLTYPE two) {
	if (!one.filename || !two.filename) { return -1; }
	if (strcmp(one.filename, two.filename) != 0) { return 0; }
	
	if ((one.first_line - two.last_line <= 1) || 
		(two.first_line - one.last_line <= 1)) { return 1; }
}


static YYLTYPE lastDirectDeclarator;
static YYLTYPE firstDeclarationSpecifier;
static int inDeclarator = 0;

void h_registerIdentifier(YYLTYPE location) {
	
}

void h_beginDirectDeclarator(YYLTYPE location) {
	inDeclarator++;
}

void h_endDirectDeclarator(YYLTYPE location) {
	inDeclarator--;
	lastDirectDeclarator = location;
}

static void h_beginDeclarationSpecifiers(YYLTYPE location) {
		
}

static void h_endDeclarationSpecifiers(YYLTYPE location) {
	
}

void h_beginFunctionDefinition(YYLTYPE location) {
	beginFunctionDefinition(location);
	/* send appropriate calls for declaration_specifier, declarator etc */
	
}


/* Type specifiers: */
static void h_registerTypeSpecifier(YYLTYPE location) {
	if (!inDeclarator) firstDeclarationSpecifier = location;
}

void h_registerVoid(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerChar(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerShort(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerInt(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerLong(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerFloat(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerDouble(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerSigned(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerUnsigned(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerStructUnionSpecifier(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerEnumSpecifier(YYLTYPE location) {h_registerTypeSpecifier(location);}
void h_registerTypeName(YYLTYPE location) {h_registerTypeSpecifier(location);}
