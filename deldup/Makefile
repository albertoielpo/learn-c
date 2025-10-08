# Compiler and flags
CC = gcc
# Added -static for compatibility, remove if not needed anymore
CFLAGS = -static -Wall -Wextra -Wpedantic -O2 -g -std=c99

# Target executables
TARGETS = hash hashs deldup

# Source files for each target
HASH_SRCS = sha1.c hash.c
HASHS_SRCS = sha1.c hashs.c
DELDUP_SRCS = sha1.c deldup.c

# Object files for each target
HASH_OBJS = $(HASH_SRCS:.c=.o)
HASHS_OBJS = $(HASHS_SRCS:.c=.o)
DELDUP_OBJS = $(DELDUP_SRCS:.c=.o)

# All object files (for cleanup)
ALL_OBJS = sha1.o hash.o hashs.o deldup.o

# Default target - build all executables
all: $(TARGETS)

# Link object files for hash
hash: $(HASH_OBJS)
	$(CC) $(CFLAGS) -o hash $(HASH_OBJS)

# Link object files for hashs
hashs: $(HASHS_OBJS)
	$(CC) $(CFLAGS) -o hashs $(HASHS_OBJS)

# Link object files for deldup
deldup: $(DELDUP_OBJS)
	$(CC) $(CFLAGS) -o deldup $(DELDUP_OBJS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(ALL_OBJS) $(TARGETS)

# Phony targets (not actual files)
.PHONY: all clean