#COPTS=-ansi -Wno-pointer-sign -DDEBUG=1 -DDEBUG2=1
COPTS=-ansi -Wno-pointer-sign -O2
BIN=../bin/
INSTALL=~/bin/

.c.o:
	clang -g -c $(COPTS) *.c

all: $(BIN)fress.convert $(BIN)trans

clean:
	rm *.o $(BIN)*

install: all $(BIN)trans $(BIN)fress.convert $(BIN)slc
	cp $(BIN)trans $(BIN)fress.convert $(BIN)slc $(INSTALL)

$(BIN)slc: slc.o 
	clang -g slc.o -o $(BIN)slc

$(BIN)trans: trans.o 
	clang -g trans.o -o $(BIN)trans

$(BIN)fress.convert: fress.o ERR1.o crip-err.o
	clang -g fress.o ERR1.o crip-err.o -o $(BIN)fress.convert

fress.o: fress.c fress.h
crip-err.o: crip-err.c fress.h
ERR1.o: ERR1.c fress.h
