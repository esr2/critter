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
		FIRST_AND_UNUSED_TREE_CODE, /* unspecified default to 0 */
		LABELED_STATEMENT,
		COMPOUND_STATEMENT,
		EXPRESSION_STATEMENT,
		SELECTION_STATEMENT,
		ITERATION_STATEMENT,
		JUMP_STATEMENT,
		EMPTY_COMPOUND,
		STATEMENT_COMPOUND,
		IF_SELECTION,
		IF_ELSE_SELECTION,
		LAST_AND_UNUSED_TREE_CODE	/* A convenient way to get a value for
									   NUM_TREE_CODE.  */
};

/* Number of language-independent tree codes.  */
#define NUM_TREE_CODES ((int) LAST_AND_UNUSED_TREE_CODE)

const char* treeCodeLabels[] = {
	"FIRST_AND_UNUSED_TREE_CODE",
	"LABELED_STATEMENT",
	"COMPOUND_STATEMENT",
	"EXPRESSION_STATEMENT",
	"SELECTION_STATEMENT",
	"ITERATION_STATEMENT",
	"JUMP_STATEMENT",
	"EMPTY_COMPOUND",
	"STATEMENT_COMPOUND",
	"IF_SELECTION",
	"IF_ELSE_SELECTION",
	"LAST_AND_UNUSED_TREE_CODE"
};