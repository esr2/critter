default: all

run: 
	a.out test.c

test:
	tester.sh 2> output.txt

all: generate main.c checks.c dynarray.c sax.c comments.c hooks.c locations.c
	gcc main.c c.tab.c checks.c dynarray.c sax.c comments.c hooks.c locations.c

debug: generate main.c checks.c dynarray.c sax.c comments.c hooks.c locations.c
	gcc -g main.c c.tab.c checks.c dynarray.c sax.c comments.c hooks.c locations.c

generate: c.y c.l checks.h
	bison c.y
	flex c.l

clean:
	rm -f *.tab.* lex.yy.c
	rm -f a.out
	rm -rf *.dSYM

spotless:
	rm -f output.txt
