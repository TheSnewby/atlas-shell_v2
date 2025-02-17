# Test cases for logical operators

# Semicolon (;) - sequential execution
echo "Test 1: Semicolon"
echo "hello" ; echo "world"
echo "hello" ; false ; echo "world" # Even if 'false' fails, 'echo "world"' runs

# && (AND) - short-circuiting
echo "Test 2: && (both succeed)"
true && echo "Both commands succeeded"

echo "Test 3: && (first fails)"
false && echo "This should NOT print"

# || (OR) - short-circuiting
echo "Test 4: || (first succeeds)"
true || echo "This should NOT print"

echo "Test 5: || (first fails)"
false || echo "The first command failed"

# Combinations
echo "Test 6: Combination 1"
true && echo "Part 1" || echo "Part 2"  # Only "Part 1" should print

echo "Test 7: Combination 2"
false && echo "Part 1" || echo "Part 2" # Only "Part 2" should print

echo "Test 8: Combination 3"
true || echo "Part 1" && echo "Part 2" # Both "Part 1" and "Part 2" should NOT print

echo "Test 9: Combination 4"
false || echo "Part 1" && echo "Part 2" # Both "Part 1" and "Part 2" should print

echo "Test 10: Combination with semicolons"
true && echo "Part 1" ; echo "Part 2" || echo "Part 3" # "Part 1" and "Part 2" print

echo "Test 11: Combination with non-existent commands"
nonexistent_command && echo "This should NOT print" || echo "Command not found"
nonexistent_command ; echo "This SHOULD print"

echo "Test 12: Empty commands"
; ;  # Empty commands separated by semicolons (should not crash)
&&   # Empty command with && (should not crash)
||   # Empty command with || (should not crash)

#Test 13: Testing spaces around
ls     &&     echo "Spaces around &&"
ls -la ||       echo "space around or"
echo "first"         ;           echo "second"

#Test 14: using /bin/ls
/bin/ls && echo "/bin/ls Succeeded!"
/bin/ls || echo "/bin/ls failed!"

#test 15: commands with arguments
/bin/ls -l && echo "Listing successful"
/bin/ls -z 2>&1 > /dev/null || echo "Invalid option used"