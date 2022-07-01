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
`./alt-diff <branch1> <branch2> <arch>`

Where: 
- branch1 and branch2 are names of branches you want to compare.
- arch is an optional argument to filter result by some architecture. 

The program will take some time and output will be huge,
so you probably would want to redirect output into a file like so:

`./alt-diff "p9" "p10" > result.txt`

or to pipe it into less

`./alt-diff "p9" "p10" | less`

## Requirements
- libCurl

