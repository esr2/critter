/*
 *  tree.h
 *  
 *
 *  Created by Erin Rosenbaum on 11/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TREE_INCLUDED
#define TREE_INCLUDED

/* Codes of tree nodes */

#define DefCode(x) x
enum tree_code {
	FIRST_AND_UNUSED_TREE_CODE, /* unspecified default to 0 */
#include "tree.txt"
	,LAST_AND_UNUSED_TREE_CODE	/* A convenient way to get a value for
									   NUM_TREE_CODE.  */
};
#undef DefCode

extern const char* treeCodeLabels[];

#endif