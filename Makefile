default: all

run: 
	critTer test.c

test:
	if test -e output.txt; then cp output.txt output_old.txt; fi
	tester.sh 2> output.txt

all: generate main.c checks.c dynarray.c sax.c comments.c hooks.c locations.c
	gcc main.c c.tab.c checks.c dynarray.c sax.c comments.c hooks.c locations.c -o critTer

debug: generate main.c checks.c dynarray.c sax.c comments.c hooks.c locations.c
	gcc -g main.c c.tab.c checks.c dynarray.c sax.c comments.c hooks.c locations.c -o critTer

install:
	sudo cp critTer /usr/local/bin/
	sudo chmod 0755 /usr/local/bin/critTer

generate: c.y c.l checks.h
	bison c.y
	flex c.l

clean:
	rm -f *.tab.* lex.yy.c
	rm -f critTer
	rm -rf *.dSYM

spotless: clean
	rm -f output.txt output_old.txt
