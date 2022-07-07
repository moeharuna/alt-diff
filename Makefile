INCLUDE_PATH=/usr/include
LIBRARY_PATH=/usr/lib
BIN_PATH=/usr/bin
LIB = libalt-diff/altdiff.cpp
BIN = alt-diff/main.cpp
INCLUDE = -Ilibalt-diff/
LINK = -lcurl
CFLAGS = -Wall
STD = c++17
CC = gcc
DEBUG_FLAGS = -g
COMPILE_COMMAND = $(CC) --std=$(STD) $(CFLAGS) $(INCLUDE) $(LINK)

all: lib default

bin: lib
	mkdir -p build
	$(COMPILE_COMMAND) -Lbuild -laltdiff $(BIN)  -o build/altdiff

lib:
	mkdir -p build
	$(COMPILE_COMMAND) -fPIC -shared $(LIB) -o build/libaltdiff.so
default:
	mkdir -p build
	$(COMPILE_COMMAND) $(LIB) $(BIN)  -o build/altdiff

install: bininstall

libinstall: lib
	install build/libaltdiff.so $(LIBRARY_PATH)
	install libalt-diff/altdiff.h $(INCLUDE_PATH)

bininstall: libinstall bin
	install build/altdiff $(BIN_PATH)/altdiff

remove:
	rm -f $(LIBRARY_PATH)/libaltdiff.so
	rm -f $(INCLUDE_PATH)/altdiff.h
	rm -f $(BIN_PATH)/altdiff

clean:
	rm -f build/altdiff build/libaltdiff.so build/libaltdiff.o
