/*
 *  tree.c
 *  
 *
 *  Created by Erin Rosenbaum on 11/24/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include "tree.h"

/* Number of language-independent tree codes.  */
#define NUM_TREE_CODES ((int) LAST_AND_UNUSED_TREE_CODE)

#define DefCode(x) #x
const char* treeCodeLabels[] = {
	"FIRST_AND_UNUSED",
#include "tree.txt"
	"LAND_AND_UNUSED"
};
#undef DefCode
