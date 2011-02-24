
D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%x IFILE
%x COMMENT
%x CPP
%x MACRO

%option yylineno
%option noyywrap

%{
#include <stdio.h>
#include "c.tab.h"
#include "checks.h"
#include "sax.h"
#include "hooks.h"

int column = 1;

#define YY_USER_ACTION {yylloc.first_line = yylineno;		\
						yylloc.first_column = column;		\
						yylloc.last_column=column+yyleng;	\
						yylloc.last_line = yylineno;		\
						yylloc.filename = curfilename;}

	void count();
	
	struct bufstack { 
		struct bufstack *prev;
		YY_BUFFER_STATE bs;
		int lineno;
		char *filename;
		FILE *f;
	} *curbs = 0;
	char *curfilename;
	
	/* the symbol table inspired by O'Reily */
	struct symbol {
		char *name;
	};
	
	/* simple symtab of fixed size */ 
#define NHASH 9997 
	struct symbol symboltable[NHASH];
	struct symbol filetable[NHASH];

	struct symbol *lookupSymbol(char* text);
	struct symbol *addSymbol(char* text);
	void printSymTable();

	int newfile(char *fn);
	int popfile();
	int check_type();
%}

%%

"/*" 				{beginComment(yylloc); BEGIN(COMMENT); }
<COMMENT>"*/"				{endComment(yylloc); BEGIN(INITIAL); }
<COMMENT>([^*]|\n)+|.		{registerComment(yytext);}
<COMMENT><<EOF>>			{yyerror("Unterminated comment"); return 0; }
"//"			{CPlusPlusComments(yylloc); beginComment(yylloc); BEGIN(CPP);}
<CPP>.*			{registerComment(yytext);}
<CPP>\n			{endComment(yylloc); BEGIN(INITIAL);}


^"#"[ \t]*include[ \t]*[\"] {BEGIN IFILE;}
<IFILE>[^ \t\n\">]+	{
						{ int c;
						  while((c = input()) && c != '\n') ;
						}
						if(!newfile(yytext)) {
							yyterminate(); /* no such file */ 
						}
						BEGIN INITIAL;
					}
<IFILE>.|\n	{ fprintf(stderr, "%4d bad include line\n", yylineno);
				yyterminate();
			}
<<EOF>>	{h_endFile(yylloc); if(!popfile()) {h_endProgram(yylloc); yyterminate();} }

"#"							{ BEGIN(MACRO); }
<MACRO>"define".*{D}+		{ h_registerDefineIntegralType(yylloc); }
<MACRO>.*
<MACRO>\n					{ BEGIN(INITIAL); }

"auto"			{ count(); return(AUTO); }
"break"			{ count(); return(BREAK); }
"case"			{ count(); return(CASE); }
"char"			{ count(); return(CHAR); }
"const"			{ count(); return(CONST); }
"continue"		{ count(); return(CONTINUE); }
"default"		{ count(); return(DEFAULT); }
"do"			{ count(); return(DO); }
"double"		{ count(); return(DOUBLE); }
"else"			{ count(); return(ELSE); }
"enum"			{ count(); return(ENUM); }
"extern"		{ count(); return(EXTERN); }
"float"			{ count(); return(FLOAT); }
"for"			{ count(); return(FOR); }
"goto"			{ count(); return(GOTO); }
"if"			{ count(); return(IF); }
"int"			{ count(); return(INT); }
"long"			{ count(); return(LONG); }
"register"		{ count(); return(REGISTER); }
"return"		{ count(); return(RETURN); }
"short"			{ count(); return(SHORT); }
"signed"		{ count(); return(SIGNED); }
"sizeof"		{ count(); return(SIZEOF); }
"static"		{ count(); return(STATIC); }
"struct"		{ count(); return(STRUCT); }
"switch"		{ count(); return(SWITCH); }
"typedef"		{ count(); return(TYPEDEF); }
"union"			{ count(); return(UNION); }
"unsigned"		{ count(); return(UNSIGNED); }
"void"			{ count(); return(VOID); }
"volatile"		{ count(); return(VOLATILE); }
"while"			{ count(); return(WHILE); }

{L}({L}|{D})*		{ count(); return(check_type()); }

0[xX]{H}+{IS}?		{ count(); h_registerConstantText(yytext); return(CONSTANT); }
0{D}+{IS}?		{ count(); h_registerConstantText(yytext); return(CONSTANT); }
{D}+{IS}?		{ count(); h_registerConstantText(yytext); return(CONSTANT); }
L?'(\\.|[^\\'])+'	{ count(); h_registerConstantText(yytext); return(CONSTANT); }

{D}+{E}{FS}?		{ count(); h_registerConstantText(yytext); return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ count(); h_registerConstantText(yytext); return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ count(); h_registerConstantText(yytext); return(CONSTANT); }

L?\"(\\.|[^\\"])*\"	{ count(); return(STRING_LITERAL); }

"..."			{ count(); return(ELLIPSIS); }
">>="			{ count(); return(RIGHT_ASSIGN); }
"<<="			{ count(); return(LEFT_ASSIGN); }
"+="			{ count(); return(ADD_ASSIGN); }
"-="			{ count(); return(SUB_ASSIGN); }
"*="			{ count(); return(MUL_ASSIGN); }
"/="			{ count(); return(DIV_ASSIGN); }
"%="			{ count(); return(MOD_ASSIGN); }
"&="			{ count(); return(AND_ASSIGN); }
"^="			{ count(); return(XOR_ASSIGN); }
"|="			{ count(); return(OR_ASSIGN); }
">>"			{ count(); return(RIGHT_OP); }
"<<"			{ count(); return(LEFT_OP); }
"++"			{ count(); return(INC_OP); }
"--"			{ count(); return(DEC_OP); }
"->"			{ count(); return(PTR_OP); }
"&&"			{ count(); return(AND_OP); }
"||"			{ count(); return(OR_OP); }
"<="			{ count(); return(LE_OP); }
">="			{ count(); return(GE_OP); }
"=="			{ count(); return(EQ_OP); }
"!="			{ count(); return(NE_OP); }
";"			{ count(); return(';'); }
("{"|"<%")		{ count(); return('{'); }
("}"|"%>")		{ count(); return('}'); }
","			{ count(); return(','); }
":"			{ count(); return(':'); }
"="			{ count(); return('='); }
"("			{ count(); return('('); }
")"			{ count(); return(')'); }
("["|"<:")		{ count(); return('['); }
("]"|":>")		{ count(); return(']'); }
"."			{ count(); return('.'); }
"&"			{ count(); return('&'); }
"!"			{ count(); return('!'); }
"~"			{ count(); return('~'); }
"-"			{ count(); return('-'); }
"+"			{ count(); return('+'); }
"*"			{ count(); return('*'); }
"/"			{ count(); return('/'); }
"%"			{ count(); return('%'); }
"<"			{ count(); return('<'); }
">"			{ count(); return('>'); }
"^"			{ count(); return('^'); }
"|"			{ count(); return('|'); }
"?"			{ count(); return('?'); }

[ \t\v\n\f]		{ count(); }
.			{ /* ignore bad characters */ }

%%

/**
 * Keep track of the column number.
 */
void count() {
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n') {
			column = 1;
		} else if (yytext[i] == '\t') {
			column += 4 - (column % 4);
		} else {
			column++;
		}
}

/**
 * Check the type of a string to see if it is a TYPE_NAME or IDENTIFIER
 */
int check_type() {
	if (strcmp(yytext, "size_t") == 0 || strcmp(yytext, "FILE") == 0) {
		return TYPE_NAME;
	}

	struct symbol *sp = lookupSymbol(NULL);

	if (sp != NULL) {
		return (TYPE_NAME);
	}

	h_registerIdentifierText(yytext);
	return(IDENTIFIER); 
}

/* FROM O'REILY */
/**
 * Add a file on to the buffer stack and then switch to it.
 *
 * @param fn filename to open
 * @returns 0 on failure, 1 on success and 2 on skipped file due to previous include
 */
int newfile(char *fn) {
	/* don't include if file was already included */
	if (lookupSymbol(fn)) {
		return 2;
	}

	FILE *f = fopen(fn, "r");
	/* if not local then check usr/include */
	if (!f) {
		char *prefix =  "/usr/include/";
		char *filename = calloc(strlen(fn)+strlen(prefix)+5, 1);
		strcpy(filename, prefix);
		strcpy(filename+strlen(prefix), fn);
		f = fopen(filename, "r");
		if (!f) {perror(fn); return 0;}
	} 
	/* add file to included list */
	addSymbol(fn);

	struct bufstack *bs = malloc(sizeof(struct bufstack));

	/* die if no room */ 
	if(!bs) { perror("malloc"); exit(1); }

	/* remember state */ 
	if(curbs) { curbs->lineno = yylineno; }
	bs->prev = curbs;

	/* set up current entry */ 
	bs->bs = yy_create_buffer(f, YY_BUF_SIZE);
	bs->f = f; 
	bs->filename = fn; 
	yy_switch_to_buffer(bs->bs); 
	curbs = bs;
	yylineno = 1; 
	curfilename = fn; 

	/* perform any beginning of file checks */
	h_beginFile(curfilename);

	return 1;
}

/**
 * Pop file from the buffer stack.
 *
 * @returns 0 on failure and 1 on success
 */
int popfile() {
	struct bufstack *bs = curbs;
	struct bufstack *prevbs = NULL;
	if(!bs) return 0;

	/* get rid of current entry */
	fclose(bs->f); 
	yy_delete_buffer(bs->bs);

	/* switch back to previous */ 
	prevbs = bs->prev; 
	free(bs);

	if(prevbs == NULL) {
		return 0;
	}

	yy_switch_to_buffer(prevbs->bs); 
	curbs = prevbs;
	yylineno = curbs->lineno; 
	curfilename = curbs->filename; 

	return 1;
}

/**
 * Hash a symbol 
 */ 
static unsigned symhash(const char *sym) {
	unsigned int hash = 0U; 
	unsigned c;
	
	while(c = *sym++) { 
		hash = hash*9 ^ c;
	} 
	
	return hash % NHASH;
}

/**
 * Looks for a symbol in either the symbol table or file table.
 * 
 * @param text the filename to find or NULL (meaning look for yytext)
 * @returns NULL on failure or the symbol if found
 */
struct symbol * lookupSymbol(char *text) {
	const char* sym = NULL;
	struct symbol *table = NULL;
	if (!text) {
		sym = yytext;
		table = symboltable;
	} else {
		sym = text;
		table = filetable;
	}

	int start = symhash(sym);
	struct symbol *sp = &table[start];
	int scount = NHASH;	// how many have we looked at

	while(--scount >= 0) {
		if(sp->name && !strcmp(sp->name, sym)) {
			return sp; //entry found
		}

		if(!sp->name) {	// empty entry
			return NULL;
		}

		if(++sp >= table+NHASH) sp = table; // try the next entry
	} 

	fputs("symbol table overflow\n", stderr);
	abort();  // tried them all, table is full
}

/**
 * Adds a symbol into either the symbol table or file table.
 * 
 * @param text the filename to add or NULL (meaning add yytext)
 * @returns NULL on add or the symbol if previously found
 */
struct symbol * addSymbol(char* text) {
	const char* sym = NULL;
	struct symbol *table = NULL;
	if (!text) {
		sym = yytext;
		table = symboltable;
	} else {
		sym = text;
		table = filetable;
	}

	int start = symhash(sym);
	struct symbol *sp = &table[start];
	int scount = NHASH;	// how many have we looked at

	while(--scount >= 0) {
		if(sp->name && !strcmp(sp->name, sym)) {
			return sp; //entry already found
		}
	
		if(!sp->name) {	// new entry
			sp->name = strdup(sym); 
			return NULL;
		}

		if(++sp >= table+NHASH) sp = table; // try the next entry
	}

	fputs("symbol table overflow\n", stderr);
	abort();  // tried them all, table is full
}

/**
 * Debugging function to print symbol table 
 */
void printSymTable() {
	printf("printing the symbol table\n");
	struct symbol *sp;
	for(sp = symboltable; sp->name && sp < symboltable+NHASH; sp++) {
		printf("\t%10s\n", sp->name);
	}
}