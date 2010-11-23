default: all

run: 
	echo "==================="
	a.out


all: generate main.c checks.c
	gcc main.c c.tab.c checks.c

generate: c.y c.l tree.txt tree.h checks.h
	bison -d c.y
	flex c.l

clean:
	rm -f *.tab.* lex.yy.c
	rm -f a.out
