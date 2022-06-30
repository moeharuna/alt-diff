LIB = src/lib/altdiff.cpp
FILES = src/bin/main.cpp
DYNAMIC = "-lcurl"
CFLAGS = -Wall
STD = c++17
CC = clang++
DEBUG_FLAGS = -g -DALTDIFF_DEBUG


alt-diff: $(LIB) $(FILES)
	$(CC) $(LIB) $(FILES) --std=$(STD) $(CFLAGS) $(DYNAMIC) -o alt-diff
debug: $(LIB) $(FILES)
	$(CC) $(LIB) $(FILES) --std=$(STD) $(CFLAGS) $(DEBUG_FLAGS) $(DYNAMIC) -o alt-diff
clean:
	rm alt-diff
