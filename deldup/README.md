# Deldup
Deldup is a Unix utility designed to remove duplicate files. When two files share the same hash, they are considered identical.

## Important Notes

This program:

- Uses SHA-1 as its hash function
- Runs on a single thread
- Only checks files within a single folder (non-recursive)
- Compiles with the -static flag in the Makefile. Remove this flag if the build and target systems are identical to reduce binary size
- Is not cross-platform and is designed specifically for Linux systems

## Build
```bash
# build using make
make clean && make
```
This produces three executables:

<code>deldup</code> - Main executable that identifies and deletes duplicate files
<code>hash</code> - Functions identically to deldup but does not delete duplicates
<code>hashs</code> - Generates SHA-1 hashes from command-line arguments

## Usage
```bash
# compare files using args
deldup <file1> <file2>...
# compare only pdf
deldup *.pdf
# compare all files
deldup *
```
