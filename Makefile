default: all

run: 
	a.out


all: generate main.c checks.c tree.c
	gcc main.c c.tab.c checks.c tree.c

generate: c.y c.l tree.txt tree.h checks.h
	bison c.y
	flex c.l

clean:
	rm -f *.tab.* lex.yy.c
	rm -f a.out
