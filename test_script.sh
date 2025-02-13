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
/bin/ls
"

EXIT_COMMAND1="exit"
EXIT_COMMAND2="exit 5"
EXIT_COMMAND3="exit -5"
EXIT_COMMAND4="exit string"

echo "===== Running in Non-Interactive Mode ====="
echo "$COMMANDS" | $VALGRIND_CMD
echo

echo
echo "===== Running in Interactive Mode ====="

# Create a temporary file for interactive mode
TMPFILE=$(mktemp)
echo "$COMMANDS" > "$TMPFILE"

# Run valgrind interactively using input redirection
$VALGRIND_CMD < "$TMPFILE"
echo

echo "===== Non-Interactive Mode exit no param ====="
echo "$EXIT_COMMAND1" | $VALGRIND_CMD
echo

echo "===== Non-Interactive Mode exit 5 ====="
echo "$EXIT_COMMAND2" | $VALGRIND_CMD
echo

echo "===== Non-Interactive Mode exit -5 ====="
echo "$EXIT_COMMAND3" | $VALGRIND_CMD
echo

echo "===== Non-Interactive Mode exit string ====="
echo "$EXIT_COMMAND4" | $VALGRIND_CMD
echo

# Clean up
rm "$TMPFILE"
