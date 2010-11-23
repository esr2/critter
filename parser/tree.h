/*
 *  tree.h
 *  
 *
 *  Created by Erin Rosenbaum on 11/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

/* Codes of tree nodes */

#define DefCode(x) x
enum tree_code {
	FIRST_AND_UNUSED_TREE_CODE, /* unspecified default to 0 */
#include "tree.txt"
	,LAST_AND_UNUSED_TREE_CODE	/* A convenient way to get a value for
									   NUM_TREE_CODE.  */
};
#undef DefCode

/* Number of language-independent tree codes.  */
#define NUM_TREE_CODES ((int) LAST_AND_UNUSED_TREE_CODE)

#define DefCode(x) #x
const char* treeCodeLabels[] = {
#include "tree.txt"
};
#undef DefCode