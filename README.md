# alt-diff
This repo contains library alt-diff with example application.
alt-diff will get list of all packages from two alt-linux distributive branches
and will compare these branches.

## Building
Run `make` in repo directory to build project.
You can also run `make lib` to build only shared library.

## Running
`./altdiff <branch1> <branch2> <arch>`

Where: 
- branch1 and branch2 are names of branches you want to compare.
- arch is an optional argument to filter result by some architecture. 

The program will take some time and output will be huge,
so you probably would want to redirect output into a file like so:

`./altdiff "p9" "p10" > result.txt`

or to pipe it into less

`./altdiff "p9" "p10" | less`

## Installing
Run `make install` to install altdiff. Or alternatively if you don't need alt-diff binary run `make libinstall`

## Uninstalling
Run `make remove`.

## Requirements
- libCurl
- Boost
