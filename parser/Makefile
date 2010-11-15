default: all

run: 
	echo "==================="
	a.out


all: generate main.c
	gcc main.c c.tab.c

generate: c.y c.l
	bison -d c.y
	flex c.l

clean:
	rm -f *.tab.* lex.yy.c
	rm -f a.out
