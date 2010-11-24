/*
 *  checks.h
 *  
 *
 *  Created by Erin Rosenbaum on 11/22/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CHECKS_INCLUDED
#define CHECKS_INCLUDED

#include "tree.h"
#include "c.tab.h"

void ifHasBraces(enum tree_code statementValue, YYLTYPE location);

#endif