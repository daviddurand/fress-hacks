COPTS=-ansi -Wno-pointer-sign -DDEBUG=1 -DDEBUG2=1
.c.o:
	clang -g -c $(COPTS) $*.c

fress.convert: fress.o ERR1.o crip-err.o
	clang -g fress.o ERR1.o crip-err.o -o fress.convert

install:  fress.convert
	cp fress.convert ~/bin/fress.convert

fress.o: fress.c fress.h
crip-err.o: crip-err.c fress.h
ERR1.o: ERR1.c fress.h
