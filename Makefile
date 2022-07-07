INCLUDE_PATH=/usr/include
LIBRARY_PATH=/usr/lib
BIN_PATH=/usr/bin
LIB = libalt-diff/altdiff.cpp
BIN = alt-diff/main.cpp
INCLUDE = -Ilibalt-diff/
LINK = -lcurl
CFLAGS = -Wall
STD = c++17
CC = g++
DEBUG_FLAGS = -g
COMPILE_COMMAND = $(CC) --std=$(STD) $(CFLAGS) $(INCLUDE) $(LINK)

staticbin: $(BIN) $(LIB)
	make object
	$(COMPILE_COMMAND) build/libaltdiff.o $(BIN) -o build/altdiff.out

object: $(LIB)
	mkdir -p build
	$(COMPILE_COMMAND) -fPIC -c  $(LIB)  -o  build/libaltdiff.o

bin: $(BIN) $(LIB)
	make lib
	$(COMPILE_COMMAND) -Lbuild -laltdiff $(BIN)  -o build/altdiff.out

lib: $(LIB)
	mkdir -p build
	$(COMPILE_COMMAND) -fPIC -shared $(LIB) -o build/libaltdiff.so

install: $(LIB)
	make lib
	strip build/libaltdiff.so
	cp build/libaltdiff.so $(LIBRARY_PATH)
	cp libalt-diff/altdiff.h $(INCLUDE_PATH)

installbin: $(BIN) $(LIB)
	make install
	make bin
	cp build/altdiff.out $(BIN_PATH)/altdiff

remove:
	rm -f $(LIBRARY_PATH)/libaltdiff.so
	rm -f $(INCLUDE_PATH)/altdiff.h
	rm -f $(BIN_PATH)/altdiff

clean:
	rm -f build/altdiff.out build/libaltdiff.so build/libaltdiff.o
