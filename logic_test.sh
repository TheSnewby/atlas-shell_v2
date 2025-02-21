#!/bin/bash

# Test cases for logical operators in your custom shell.
# This script assumes your shell is named 'hsh' and is in the current directory.

# --- Helper Function ---
run_test() {
    local test_num=$1
    local command=$2
    local expected_output=$3
    local expected_exit_code=$4

    echo "-----------------------------------------"
    echo "Test $test_num: $command"
    echo "Expected Output: '$expected_output'"
    echo "Expected Exit Code: $expected_exit_code"

    # Run the command through your shell
    output=$(echo "$command" | ./hsh 2>&1)  # Capture both stdout and stderr
    actual_exit_code=$?

    echo "Actual Output: '$output'"
    echo "Actual Exit Code: $actual_exit_code"

    # Check if the output matches (using grep -q for partial matching)
    if echo "$output" | grep -q "$expected_output"; then
        output_check="PASSED"
    else
        output_check="FAILED"
    fi

    # Check if the exit code matches
    if [ "$actual_exit_code" -eq "$expected_exit_code" ]; then
        exit_code_check="PASSED"
    else
        exit_code_check="FAILED"
    fi

    echo "Output Check: $output_check"
    echo "Exit Code Check: $exit_code_check"

    if [ "$output_check" = "PASSED" ] && [ "$exit_code_check" = "PASSED" ]; then
        echo "Test $test_num: PASSED"
        return 0
    else
        echo "Test $test_num: FAILED"
        return 1
    fi
}

# --- Test Cases ---

# Semicolon (;) - sequential execution
run_test 1 "echo hello ; echo world" "hello\nworld" 0
run_test 2 "echo hello ; false ; echo world" "hello\nworld" 0  # Check exit code of *last* command

# && (AND) - short-circuiting
run_test 3 "true && echo Both commands succeeded" "Both commands succeeded" 0
run_test 4 "false && echo This should NOT print" "" 1

# || (OR) - short-circuiting
run_test 5 "true || echo This should NOT print" "" 0
run_test 6 "false || echo The first command failed" "The first command failed" 0

# Combinations
run_test 7 "true && echo Part 1 || echo Part 2" "Part 1" 0
run_test 8 "false && echo Part 1 || echo Part 2" "Part 2" 0
run_test 9 "true || echo Part 1 && echo Part 2" "" 0  #Short circuit
run_test 10 "false || echo Part 1 && echo Part 2" "Part 1\nPart 2" 0

run_test 11 "true && echo Part 1 ; echo Part 2 || echo Part 3" "Part 1\nPart 2" 0 # Check semicolon precedence

run_test 12 "nonexistent_command && echo This should NOT print || echo Command not found" "Command not found" 0  #Corrected expected output and exit
run_test 13 "nonexistent_command ; echo This SHOULD print" "This SHOULD print" 0  #Corrected expected output and exit

# Empty commands (should not crash, exit code should be 0 for empty command)
run_test 14 "; ;" "" 0
run_test 15 "&&" "" 1 #error code
run_test 16 "||" "" 0  # or any non-zero code as appropriate for your shell

# Testing spaces around
run_test 17 "ls     &&     echo Spaces around &&" "Spaces around &&" 0
run_test 18 "ls -la ||       echo space around or" "" 0
run_test 19 "echo first         ;           echo second" "first\nsecond" 0

# Using /bin/ls
run_test 20 "/bin/ls && echo /bin/ls Succeeded!" "/bin/ls Succeeded!" 0
run_test 21 "/bin/ls || echo /bin/ls failed!" "" 0  # This will likely still pass.  It's testing YOUR shell, not bash.

# Commands with arguments, including redirection (to test interaction)
run_test 22 "/bin/ls -l && echo Listing successful" "Listing successful" 0
run_test 23 "/bin/ls -z 2>&1 > /dev/null || echo Invalid option used" "Invalid option used" 0
run_test 24 "/bin/ls -z 2>&1 | grep \"invalid option\" && echo found" "found" 0

# More Complex Combinations (to thoroughly test precedence)
run_test 25 "true && (false || echo It works!)" "It works!" 0
run_test 26 "(true && false) || echo It works!" "It works!" 0
run_test 27 "echo A; (false && echo B) || echo C; echo D" "A\nC\nD" 0
run_test 28 "true && false; echo hello" "hello" 0

# Exit code testing (ensure your shell propagates exit codes correctly)
run_test 29 "false; echo \$?" "1" 0  # Check that $? works.  VERY IMPORTANT.
run_test 30 "true; echo \$?" "0" 0
run_test 31 "nonexistent_command; echo \$?" "127" 0 #Or whatever exit you print.
run_test 32 "(exit 42); echo \$?" "42" 0
run_test 33 "true && (exit 42); echo \$?" "42" 0

#Test pipes and logic operators
run_test 34 "echo hello | tr 'a-z' 'A-Z' && echo done" "HELLO\ndone" 0
run_test 35 "echo hello | tr 'a-z' 'A-Z' || echo done" "HELLO" 0
run_test 36 "ls -la | grep Makefile && echo Found it!" "Found it!" 0

# Clean up after the script
rm -f output.txt test.txt

echo "-----------------------------------------"
echo "All tests completed."

exit_code=0

#Check exit code
for i in $(seq 1 36);
do
  if [ ! -f "test_result_$i" ]; then
    exit_code=1
  fi
done

exit $exit_code