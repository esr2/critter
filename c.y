/***
 * This grammar file has been adapted from http://www.lysator.liu.se/c/ANSI-C-grammar-y.html
 */
%code {
#include <stdio.h>
#include <stdlib.h>
#include "checks.h"
#include "sax.h"
#include "hooks.h"
#include <stdarg.h>
}

%code requires {
#ifndef YYLTYPE
#define YYLTYPE YYLTYPE
    /* code defined here (and only here) will appear in the header file */
	typedef struct YYLTYPE {
		int first_line;
		int first_column;
		int last_line;
		int last_column;
		char *filename;
	} YYLTYPE;
	
	enum errorLevel {
		ERROR_HIGH,
		ERROR_NORMAL,
		ERROR_LOW,
	};
    
    void lyyerror(enum errorLevel priority, YYLTYPE t, char *s);
    void yyerror(char *s);
    void lyyerrorf(enum errorLevel priority, YYLTYPE location, char* format, ...);
#endif
	
# define YYLLOC_DEFAULT(Current, Rhs, N)								\
	do																	\
		if (N)															\
		{																\
			(Current).first_line	= YYRHSLOC (Rhs, 1).first_line;		\
			(Current).first_column	= YYRHSLOC (Rhs, 1).first_column;	\
			(Current).last_line		= YYRHSLOC (Rhs, N).last_line;		\
			(Current).last_column	= YYRHSLOC (Rhs, N).last_column;	\
			(Current).filename		= YYRHSLOC (Rhs, 1).filename;		\
		}																\
		else															\
		{																\
			(Current).first_line = (Current).last_line	=				\
				YYRHSLOC (Rhs, 0).last_line;							\
			(Current).first_column = (Current).last_column =			\
				YYRHSLOC (Rhs, 0).last_column;							\
			(Current).filename	= YYRHSLOC (Rhs, 0).filename;			\
		}																\
	while (0)
}

%token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%start translation_unit

%error-verbose
%locations
%defines

%%

primary_expression
	: IDENTIFIER	{h_registerIdentifier(@$);}
	| CONSTANT		{h_registerConstant(@$);}
	| STRING_LITERAL
	| '(' expression ')'
	;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER 	{h_registerIdentifier(@3);}
	| postfix_expression PTR_OP IDENTIFIER	{h_registerIdentifier(@3);}
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' cast_expression
	| multiplicative_expression '/' cast_expression
	| multiplicative_expression '%' cast_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression
	: assignment_expression					{h_registerExpression(@$);}
	| expression ',' assignment_expression	{h_registerExpression(@3);}
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers ';'														{h_endDeclaration(@$);}
	| declaration_specifiers init_declarator_list ';'									{h_endDeclaration(@$);}
	| TYPEDEF {h_registerTypedef(@1);} declaration_specifiers type_init_declarator ';'	{h_endDeclaration(@$);}
	;

declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier
	| type_specifier declaration_specifiers
	| type_qualifier
	| type_qualifier declaration_specifiers
	;

init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator
	| declarator '=' initializer
	;

type_init_declarator
	: pointer type_direct_declarator
	| type_direct_declarator
	;

storage_class_specifier
	: EXTERN		{h_registerExtern(@$);}
	| STATIC		{h_registerStatic(@$);}
	| AUTO			{h_registerAuto(@$);}
	| REGISTER		{h_registerRegister(@$);}
	;

type_specifier
	: VOID							{h_registerVoid(@$);}
	| CHAR							{h_registerChar(@$);}
	| SHORT							{h_registerShort(@$);}
	| INT							{h_registerInt(@$);}
	| LONG							{h_registerLong(@$);}
	| FLOAT							{h_registerFloat(@$);}
	| DOUBLE						{h_registerDouble(@$);}
	| SIGNED						{h_registerSigned(@$);}
	| UNSIGNED						{h_registerUnsigned(@$);}
	| struct_or_union_specifier		{h_registerStructUnionSpecifier(@$);}
	| enum_specifier				{h_registerEnumSpecifier(@$);}
	| TYPE_NAME						{h_registerTypeName(@$);}
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;

struct_or_union
	: STRUCT
	| UNION
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER							{h_registerIdentifier(@$);}
	| IDENTIFIER '=' constant_expression	{h_registerIdentifier(@1);}
	;

type_qualifier
	: CONST		{h_registerConst(@$);}
	| VOLATILE	{h_registerVolatile(@$);}
	;

declarator
	: pointer direct_declarator
	| direct_declarator
	;

direct_declarator
	: IDENTIFIER			{h_registerIdentifier(@$);}
	| '(' declarator ')'
	| direct_declarator '[' {h_beginDirectDeclarator(@1);} constant_expression ']'	{h_endDirectDeclarator(@$);}
	| direct_declarator '[' {h_beginDirectDeclarator(@1);} ']'						{h_endDirectDeclarator(@$);}
	| direct_declarator '(' {h_beginDirectDeclarator(@1);} parameter_type_list ')'	{h_endDirectDeclarator(@$);}
	| direct_declarator '(' {h_beginDirectDeclarator(@1);} identifier_list ')'		{h_endDirectDeclarator(@$);}
	| direct_declarator '(' {h_beginDirectDeclarator(@1);} ')'						{h_endDirectDeclarator(@$);}
	;

type_direct_declarator
	: IDENTIFIER				{ addSymbol(NULL);}
	| '(' type_init_declarator ')'
	| type_direct_declarator '[' constant_expression ']'
	| type_direct_declarator '[' ']'
	| type_direct_declarator '(' parameter_type_list ')'
	| type_direct_declarator '(' identifier_list ')'
	| type_direct_declarator '(' ')'
	;

pointer
	: '*' {h_registerPointer(@$);}
	| '*' type_qualifier_list {h_registerPointer(@1);}
	| '*' pointer
	| '*' type_qualifier_list pointer
	;

type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;

beginParamList : /*empty*/ {h_beginParameterList(@$)};

parameter_type_list
	: beginParamList parameter_list					{h_endParameterList(@$);}
	| beginParamList parameter_list ',' ELLIPSIS	{h_endParameterList(@$);}
	;

parameter_list
	: parameter_declaration							{h_registerParameter(@$);}
	| parameter_list ',' parameter_declaration		{h_registerParameter(@3);}
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER						{h_registerIdentifier(@$);}
	| identifier_list ',' IDENTIFIER 	{h_registerIdentifier(@3);}
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

initializer_list
	: initializer
	| initializer_list ',' initializer
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: IDENTIFIER ':' {h_registerIdentifier(@1);} statement
	| CASE {registerCase(@1);} constant_expression ':' statement
	| DEFAULT {registerDefault(@$);} ':' statement
	;

beginCompound : /* empty */ {beginCompoundStatement(@$);}

compound_statement
	: '{' beginCompound '}'									{endCompoundStatement(@$);}
	| '{' beginCompound statement_list '}'					{endCompoundStatement(@$);}
	| '{' beginCompound declaration_list '} '				{endCompoundStatement(@$);}
	| '{' beginCompound declaration_list statement_list '}'	{endCompoundStatement(@$);}
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement
	| statement_list statement
	;

expression_statement
	: ';'
	| expression ';'	{h_endExpressionStatement(@$);}
	| error ';'
	;

beginIF : /*empty*/ {beginIf(@$);}

selection_statement
	: IF beginIF '(' expression ')' statement %prec LOWER_THAN_ELSE							{endIf(@$);}
	| IF beginIF '(' expression ')' statement ELSE {endIf(@6); beginElse(@7);} statement	{endElse(@9);}
	| SWITCH {beginSwitch(@1);} '(' expression ')' statement								{endSwitch(@$);}
	;

beginFOR : /*empty*/ {beginFor(@$);}

iteration_statement
	: WHILE {beginWhile(@1);} '(' expression ')' statement									{endWhile(@$);}
	| DO {beginDoWhile(@1);} statement WHILE '(' expression ')' ';'							{endDoWhile(@$);}
	| FOR beginFOR '(' expression_statement expression_statement ')' statement				{endFor(@$);}
	| FOR beginFOR '(' expression_statement expression_statement expression ')' statement	{endFor(@$);}
	| FOR beginFOR '(' declaration expression_statement ')' statement						{endFor(@$);}
	| FOR beginFOR '(' declaration expression_statement expression ')' statement			{endFor(@$);}
	;

jump_statement
	: GOTO IDENTIFIER ';'	{registerGoto(@$); h_registerIdentifier(@2);}
	| CONTINUE ';'			{registerContinue(@$);}
	| BREAK ';'				{registerBreak(@$);}
	| RETURN ';'			{registerReturn(@$);}
	| RETURN expression ';'	{registerReturnSomething(@$);}
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition
	| declaration
	;

function_definition
	: declaration_specifiers declarator {YYLTYPE l = @2; l.first_line = @1.first_line; l.first_column = @1.first_column; h_beginFunctionDefinition(l);} declaration_list compound_statement {endFunctionDefinition(@$);}
	| declaration_specifiers declarator {YYLTYPE l = @2; l.first_line = @1.first_line; l.first_column = @1.first_column; h_beginFunctionDefinition(l);} compound_statement {endFunctionDefinition(@$);}
	| declarator {h_beginFunctionDefinition(@1);} declaration_list compound_statement {endFunctionDefinition(@$);}
	| declarator {h_beginFunctionDefinition(@1);} compound_statement {endFunctionDefinition(@$);}
	;

%%

void lyyerror(enum errorLevel priority, YYLTYPE t, char *s) {
	char *level;
	switch (priority) {
		case ERROR_HIGH:
			level = "big problem";
			break;
		case ERROR_NORMAL:
			level = "error";
			break;
		case ERROR_LOW:
			level = "low priority";
			break;
		default:
			break;
	}
	
	if(t.first_line) {
		fprintf(stderr, "%s:%d.%d-%d.%d: %s: ", t.filename, t.first_line,
				t.first_column, t.last_line, t.last_column, level);
	} 
	fprintf(stderr, "%s\n", s);
}

void yyerror(char *s)
{
	lyyerror(ERROR_HIGH, yylloc, s);
}

void lyyerrorf(enum errorLevel priority, YYLTYPE location, char* format, ...) {
	char error[500];
	va_list arg_ptr;
	
	va_start(arg_ptr, format);
	vsprintf(error, format, arg_ptr);
	va_end(arg_ptr);
	
	lyyerror(priority, location, error);
}
