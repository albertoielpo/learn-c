# Compiler and flags
CC      := gcc
CFLAGS  := -std=c99 -Wall -Wextra

# Name of the final executable
PROGRAM_NAME  := hello

# Source files
# Main source must be PROGRAM_NAME with .c ext
SRCS    := $(PROGRAM_NAME).c

# Header dependencies
# Whenever utf8.h changes, we want to recompile <name>.c
HDRS    := utf8.h

# By default, make the 'all' target
all: $(PROGRAM_NAME)

# Build rule for the executable
# If either <name>.c or utf8.h changes, rebuild $(PROGRAM_NAME).
$(PROGRAM_NAME): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

# A 'clean' target to remove the built executable (and any .o if you add them)
.PHONY: clean
clean:
	rm -f $(PROGRAM_NAME) *.o

# Optional: a 'run' target for convenience (runs ./<name> after building)
.PHONY: run
run: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)

