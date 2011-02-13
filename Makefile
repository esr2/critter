default: all

run: 
	a.out test.c


all: generate main.c checks.c tree.c dynarray.c sax.c comments.c hooks.c
	gcc main.c c.tab.c checks.c tree.c dynarray.c sax.c comments.c hooks.c

generate: c.y c.l tree.txt tree.h checks.h
	bison c.y
	flex c.l

clean:
	rm -f *.tab.* lex.yy.c
	rm -f a.out
