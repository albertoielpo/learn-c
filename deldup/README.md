# Deldup
deldup is a unix utils designed to remove files duplicates. If two files have the same hash then it means that are equals.

## Please note

The program 
- uses SHA-1 as hash function
- is single thread
- check the files only in one folder, it's not recursive
- the compilation inside Makefile uses -static flag. Remove it if the build and the target system are the same in order to reduce the program size.
- is not cross platform. It's designed thinking about a linux system

## Build
```bash
# build using make
make clean && make
```
It produces <code>deldup</code> executable.

<code>hash</code> executable does the same of deldup but does not deletes duplicates

<code>hashs</code> create a sha1 hash from args

## Usage
```bash
# compare files using args
deldup <file1> <file2>...
# compare only pdf
deldup *.pdf
# compare all files
deldup *
```