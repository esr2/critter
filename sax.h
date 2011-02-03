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

void beginFile(char* filename);
void endFile(YYLTYPE location);
void beginProgram();
void endProgram(YYLTYPE location);


#endif