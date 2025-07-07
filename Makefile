# Define the C compiler
CC = gcc

# Define CFLAGS (compiler flags) - customize as needed
CFLAGS = -Wall -Werror -pedantic -std=c17 -O1

# Find all C files in the current directory
SRCS = $(wildcard *.c)

# Generate the executable names by removing the .c extension
BINS = $(SRCS:.c=)

# Default target: build all executables
all: $(BINS)

# Rule to compile each C file into an executable
%: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Phony target for cleaning up
.PHONY: clean
clean:
	rm -f $(BINS) *.o

