# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Werror -Wextra -pedantic -g

# Name of the executable
EXECUTABLE = hsh

# Source files
SRC = $(wildcard *.c)

# Object files (automatically generated from source files)
OBJ = $(SRC:.c=.o)

# The 'all' target (builds the executable)
all: $(EXECUTABLE)

# Rule to build the executable
$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXECUTABLE)

# Rule to build object files
%.o: %.c main.h colors.h
	$(CC) $(CFLAGS) -c $< -o $@ -MMD -MF $(@:.o=.d)

# The 'clean' target (removes temporary files)
clean:
	rm -f $(OBJ) $(EXECUTABLE) *.d

.PHONY: all clean

-include $(OBJ:.o=.d)