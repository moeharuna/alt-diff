LIB = src/lib/altdiff.cpp
FILES = src/bin/main.cpp
DYNAMIC = "-lcurl"
CFLAGS = -Wall
STD = c++17
CC = clang++


alt-diff: $(LIB) $(FILES)
	$(CC) $(LIB) $(FILES) --std=$(STD) $(CFLAGS) $(DYNAMIC) -o alt-diff
clean:
	rm *.o alt-diff
