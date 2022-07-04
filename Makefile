LIB = libalt-diff/altdiff.cpp
INCLUDE = -Iinclude/ -Ilibalt-diff/
LINK = "-lcurl"
CFLAGS = -Wall
BIN = alt-diff/main.cpp

STD = c++17
CC = g++
DEBUG_FLAGS = -g

bin: $(FILES)
	make object
	$(CC)  $(BIN) altdiff.o --std=$(STD) $(CFLAGS) $(INCLUDE) $(LINK) -o altdiff.out

object: $(LIB)
	$(CC) $(LIB) --std=$(STD) $(CFLAGS) $(INCLUDE) $(LINK) -fPIC -c -o altdiff.o

lib: $(LIB)
	make object
	$(CC) altdiff.o  -shared -o altdiff.so

debug: $(LIB) $(FILES)
	$(CC) $(LIB) $(FILES) --std=$(STD) $(CFLAGS) $(DEBUG_FLAGS) $(DYNAMIC) -o alt-diff
clean:
	rm -f altdiff.out altdiff.o altdiff.so
