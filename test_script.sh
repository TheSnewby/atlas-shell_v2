#!/bin/bash

# Define the valgrind command
VALGRIND_CMD="valgrind --leak-check=full --show-reachable=yes --track-origins=yes -s ./hsh --ltrace"
# VALGRIND_CMD="./hsh"  #non-valgrind version

# Commands to test
COMMANDS="ls
whoami
env
cd -
cd
cd -
unsetenv HOME
cd
setenv HOME /etc
cd ~
cd -
cd /root

 
    
       
fakeCommand
/bin/ls"

echo "===== Running in Non-Interactive Mode ====="
echo "$COMMANDS" | $VALGRIND_CMD

echo
echo "===== Running in Interactive Mode ====="

# Create a temporary file for interactive mode
TMPFILE=$(mktemp)
echo "$COMMANDS" > "$TMPFILE"

# Run valgrind interactively using input redirection
$VALGRIND_CMD < "$TMPFILE"

# Clean up
rm "$TMPFILE"
