
D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%x IFILE
%x COMMENT

%code {
#include <stdio.h>
#include "c.tab.h"

int column = 0;
int lineNum = 1;
char linebuf[500];

#define YY_USER_ACTION {yylloc.first_line = lineNum;	\
						yylloc.first_column = column;	\
						int colnum=column+yyleng;		\
						yylloc.last_column=colnum;		\
						yylloc.last_line = lineNum;}

void countLines();
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
struct symbol symtab[NHASH];

struct symbol *lookupSymbol();
struct symbol *addSymbol();
void printSymTable();

}

%%
\n.*  { countLines();
		strncpy(linebuf, yytext+1, sizeof(linebuf)); /* save the next line */
		yyless(1);      /* give back all but the \n to rescan */
		}

"/*" 				{ BEGIN(COMMENT); }
<COMMENT>"*/"				{ BEGIN(INITIAL); }
<COMMENT>([^*]|\n)+|.	
<COMMENT><<EOF>>			{ printf("%s:%d: Unterminated comment\n",
									 curfilename, lineNum); return 0; }


^"#"[ \t]*include[ \t]*\" {BEGIN IFILE;}
<IFILE>[^ \t\n\">]+	{
						{ int c;
						  while((c = input()) && c != '\n') ;
						}
						yylineno++;
						if(!newfile(yytext)) {
							yyterminate(); /* no such file */ 
						}
						BEGIN INITIAL;
					}
<IFILE>.|\n	{ fprintf(stderr, "%4d bad include line\n", yylineno);
				yyterminate();
			}
<<EOF>>	{if(!popfile()) yyterminate(); }

"#"			{ directive(); }

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

0[xX]{H}+{IS}?		{ count(); return(CONSTANT); }
0{D}+{IS}?		{ count(); return(CONSTANT); }
{D}+{IS}?		{ count(); return(CONSTANT); }
L?'(\\.|[^\\'])+'	{ count(); return(CONSTANT); }

{D}+{E}{FS}?		{ count(); return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ count(); return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ count(); return(CONSTANT); }

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

yywrap() {
	return(1);
}




comment() {
	char c, c1;

loop:
	while ((c = input()) != '*' && c != 0) {
		if (c == '\n') { 
			lineNum++; 
		}
	}

	if ((c1 = input()) != '/' && c != 0)
	{
		unput(c1);
		goto loop;
	}
}

include() {
	char c;

loop:
	while ((c = input()) != '\n' && c != 0) {
		putchar(c);
	}
	
	if(!newfile(yytext)) { 
		yyterminate(); /* no such file */
	}

	BEGIN INITIAL;
}

directive() {
	char c;

	while ((c = input()) != '\n' && c != 0) {
		/*putchar(c);*/;
	}

}

void countLines() {
	column = 0;
	lineNum++;
}

void count() {
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n') {
			countLines();
		} else if (yytext[i] == '\t') {
			column += 4 - (column % 4);
		} else {
			column++;
		}
}


int check_type() {
/*
* pseudo code --- this is what it should check
*
*	if (yytext == type_name)
*		return(TYPE_NAME);
*
*	return(IDENTIFIER);
*
*	it actually will only return IDENTIFIER
*/
	if (strcmp(yytext, "size_t") == 0) {
		return TYPE_NAME;
	}

	struct symbol *sp = lookupSymbol();

	if (sp != NULL) {
		return (TYPE_NAME);
	}

	return(IDENTIFIER); 
}

/* FROM O'REILY */
int newfile(char *fn) {
	FILE *f = fopen(fn, "r");
	struct bufstack *bs = malloc(sizeof(struct bufstack));

	/* die if no file or no room */ 
	if(!f) { perror(fn); return 0; } 
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
	//yylineno = 1; 
	lineNum = 1;
	curfilename = fn; 
	return 1;
}

int popfile(void) {
	struct bufstack *bs = curbs;
	struct bufstack *prevbs;
	if(!bs) return 0;

	/* get rid of current entry */
	fclose(bs->f); 
	yy_delete_buffer(bs->bs);

	/* switch back to previous */ 
	prevbs = bs->prev; 
	free(bs);

	if(!prevbs) return 0;

	yy_switch_to_buffer(prevbs->bs); 
	curbs = prevbs;
	//yylineno = curbs->lineno; 
	lineNum = curbs->lineno;
	curfilename = curbs->filename; 

	return 1;
}

/* hash a symbol */ 
static unsigned symhash(const char *sym) {
	unsigned int hash = 0U; 
	int i = 0;
	unsigned c;
	
	while(c = *sym++) { 
		hash = hash*9 ^ c;
	} 
	
	return hash % NHASH;
}

struct symbol * lookupSymbol() {
	const char* sym = yytext;
	int start = symhash(sym);
	struct symbol *sp = &symtab[start];
	int scount = NHASH;	// how many have we looked at

	while(--scount >= 0) {
		if(sp->name && !strcmp(sp->name, sym)) {
			return sp; //entry found
		}

		if(!sp->name) {	// empty entry
			return NULL;
		}

		if(++sp >= symtab+NHASH) sp = symtab; // try the next entry
	} 

	fputs("symbol table overflow\n", stderr);
	abort();  // tried them all, table is full
}

struct symbol * addSymbol() {
	const char* sym = yytext;
	int start = symhash(sym);
	struct symbol *sp = &symtab[start];
	int scount = NHASH;	// how many have we looked at

	while(--scount >= 0) {
		if(sp->name && !strcmp(sp->name, sym)) {
			return sp; //entry already found
		}
	
		if(!sp->name) {	// new entry
			sp->name = strdup(sym); 
			return NULL;
		}

		if(++sp >= symtab+NHASH) sp = symtab; // try the next entry
	}

	fputs("symbol table overflow\n", stderr);
	abort();  // tried them all, table is full
}

/* debugging function to print symbol table */
void printSymTable() {
	printf("printing the symbol table\n");
	struct symbol *sp;
	for(sp = symtab; sp->name && sp < symtab+NHASH; sp++) {
		printf("\t%10s\n", sp->name);
	}
}