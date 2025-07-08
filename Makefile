# Define the C compiler
CC = gcc

# Define CFLAGS (compiler flags) - customize as needed
CFLAGS = -Wall -Werror -pedantic -std=c17 -O1
# Use pkg-config to get PulseAudio compiler and linker flags (linker flags go after the sources)
PULSEAUDIOSIMPLE_CFLAGS := $(shell pkg-config --cflags libpulse-simple)
PULSEAUDIOSIMPLE_LIBS := $(shell pkg-config --libs libpulse-simple)

# Find all C files in the current directory
SRCS = $(wildcard *.c)

# Generate the executable names by removing the .c extension
BINS = $(SRCS:.c=)

# Default target: build all executables
all: $(BINS)

# Rule to compile each C file into an executable
%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(PULSEAUDIOSIMPLE_LIBS) $(PULSEAUDIOSIMPLE_CFLAGS) 
 

# Phony target for cleaning up
.PHONY: clean
clean:
	rm -f $(BINS) *.o

