# alt-diff
This repo contains library alt-diff with example application.
alt-diff will get list of all packages from two alt-linux distributive branches
and will compare these branches.

## Building
You can build 4 targets: 
  1. Statically* linked executable
  2. Dynamically linked executable
  3. Object file to statically link library
  4. Shared library
  
Each has its own make command.

  1. `make`
  2. `make bin`
  3. `make object`
  4. `make lib`
  
All builds are inside `build/` directory.

*Statically linked to libaltdiff, libcurl is still dependency
## Running
`./altdiff.out <branch1> <branch2> <arch>`

Where: 
- branch1 and branch2 are names of branches you want to compare.
- arch is an optional argument to filter result by some architecture. 

The program will take some time and output will be huge,
so you probably would want to redirect output into a file like so:

`./altdiff.out "p9" "p10" > result.txt`

or to pipe it into less

`./altdiff.out "p9" "p10" | less`

## Installing
1. Open Makefile. Make sure that variables `INCLUDE_PATH`, `LIBRARY_PATH` and `BIN_PATH` are exists and that they point to the right directories for your system.
2. Run `make install` to install libaltdiff. Or alternatively if you want to install both executable and libaltdiff run `make installbin`.

## Uninstalling
Run `make remove`.

## Requirements
- libCurl

