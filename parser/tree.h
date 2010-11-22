/*
 *  tree.h
 *  
 *
 *  Created by Erin Rosenbaum on 11/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

/* Codes of tree nodes */

enum tree_code {
		EMPTY_COMPOUND,
		STATEMENT_COMPOUND,
		IF_SELECTION,
		IF_ELSE_SELECTION,
		LAST_AND_UNUSED_TREE_CODE	/* A convenient way to get a value for
									   NUM_TREE_CODE.  */
};

/* Number of language-independent tree codes.  */
#define NUM_TREE_CODES ((int) LAST_AND_UNUSED_TREE_CODE)
