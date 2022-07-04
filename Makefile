LIB = libalt-diff/altdiff.cpp
INCLUDE = -Iinclude/ -Ilibalt-diff/
LINK = -lcurl
CFLAGS = -Wall
BIN = alt-diff/main.cpp

STD = c++17
CC = g++
DEBUG_FLAGS = -g

bin: $(FILES)
	make lib
	$(CC)  $(BIN) --std=$(STD) $(CFLAGS) $(INCLUDE)  $(LINK)  -L. -laltdiff  -o altdiff.out

lib: $(LIB)
	$(CC) $(LIB) --std=$(STD) $(CFLAGS) $(INCLUDE) $(LINK) -fPIC -shared -o libaltdiff.so

debug: $(LIB) $(FILES)
	$(CC) $(LIB) $(FILES) --std=$(STD) $(CFLAGS) $(DEBUG_FLAGS) $(DYNAMIC) -o alt-diff
clean:
	rm -f altdiff.out libaltdiff.so
