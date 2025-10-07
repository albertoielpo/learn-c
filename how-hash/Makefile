# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -O2 -g -std=c99

# Target executable
TARGET = hash

# Source files
SRCS = sha1.c hash.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files into executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets (not actual files)
.PHONY: all clean