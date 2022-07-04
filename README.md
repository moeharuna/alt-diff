# alt-diff
This repo contains library alt-diff with example application.
alt-diff will get list of all packages from alt-linux distributive branches
and will compare these branches.

## Building
To build the example, run these commands in your terminal:
```sh
git clone git@github.com:moeharuna/alt-diff.git
cd alt-diff
make
```
## Running
`./alt-diff.out <branch1> <branch2> <arch>`

Where: 
- branch1 and branch2 are names of branches you want to compare.
- arch is an optional argument to filter result by some architecture. 

The program will take some time and output will be huge,
so you probably would want to redirect output into a file like so:

`./alt-diff.out "p9" "p10" > result.txt`

or to pipe it into less

`./alt-diff.out "p9" "p10" | less`

## Installing
- Binary
  1. Build executable using `make` or `make bin`.
  2. Copy altdiff.out to any directory from your $PATH env variable.
- Library
  1. Build library using `make lib`.
  2. Copy `libalt-diff/altdiff.h` to your include path.
  3. Copy `libaltdiff.so` to your LD_LIBRARY_PATH.
  4. Link with `libaltdiff.so` by passing `-laltdiff` flag to your compiler.
## Requirements
- libCurl

