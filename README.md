# Atlas Shell (hsh)

## Authors

*   Nathan Wilson
*   Stephen Newby
*   Deshon Stout

## Description

Atlas Shell (hsh) is a simple Unix shell implemented in C as a learning project.  It supports:

*   **Basic command execution:** Running external commands found in the `PATH`.
*   **Built-in commands:**  `cd`, `exit` (and `quit`), `env`, `setenv`, `unsetenv`, and a custom `selfdestruct` command.
*   **Piping:**  Connecting the standard output of one command to the standard input of another (e.g., `ls -l | wc -l`).  Supports single pipes.
*   **Logical Operators:**  `&&` (AND), `||` (OR), and `;` (semicolon) for conditional and sequential command execution.
*   **Input/Output Redirection**: `>` (create/overwrite) and `>>` (append).
*   **Environment Variable Handling:**  Correctly handles the `PATH`, `HOME`, and `OLDPWD` environment variables.  Allows users to set and unset environment variables.
*   **Error Handling:**  Provides informative error messages for common errors (command not found, permission denied, etc.).
*   **Signal Handling:** Handles Ctrl+C (SIGINT) and Ctrl+D (EOF) gracefully.
*   **No Memory Leaks:**  The shell has been thoroughly tested with Valgrind to ensure there are no memory leaks or double-free errors.

## Building the Shell

To build the shell, you will need a C compiler (like GCC) and `make`.

1.  **Clone the repository:** (Assuming you have a Git repository)

    ```bash
    git clone <your_repository_url>
    cd <repository_directory>
    ```
    If you don't have a git repository, you would tell the user to download the files.

2.  **Compile:**

    ```bash
    make
    ```

    This will create an executable file named `hsh`.

## Running the Shell

To run the shell, simply execute the `hsh` executable:

```bash
./hsh

# Atlas shell (hsh)

## Authors

*   Nathan Wilson
*   Stephen Newby
*   Deshon Stout

## Description

Atlas Shell (hsh) is a simple Unix shell implemented in C as a learning project.  It supports:

*   **Basic command execution:** Running external commands found in the `PATH`.
*   **Built-in commands:**  `cd`, `exit` (and `quit`), `env`, `setenv`, `unsetenv`, and a custom `selfdestruct` command.
*   **Piping:**  Connecting the standard output of one command to the standard input of another (e.g., `ls -l | wc -l`).  Supports single pipes.
*   **Logical Operators:**  `&&` (AND), `||` (OR), and `;` (semicolon) for conditional and sequential command execution.
*   **Input/Output Redirection**: `>` (create/overwrite) and `>>` (append).
*   **Environment Variable Handling:**  Correctly handles the `PATH`, `HOME`, and `OLDPWD` environment variables.  Allows users to set and unset environment variables.
*   **Error Handling:**  Provides informative error messages for common errors (command not found, permission denied, etc.).
*   **Signal Handling:** Handles Ctrl+C (SIGINT) and Ctrl+D (EOF) gracefully.
*   **No Memory Leaks:**  The shell has been thoroughly tested with Valgrind to ensure there are no memory leaks or double-free errors.

## Building the Shell

To build the shell, you will need a C compiler (like GCC) and `make`.

1.  **Clone the repository:** (Assuming you have a Git repository)

    ```bash
    git clone <your_repository_url>
    cd <repository_directory>
    ```
    If you don't have a git repository, you would tell the user to download the files.

2.  **Compile:**

    ```bash
    make
    ```

    This will create an executable file named `hsh`.

## Running the Shell

To run the shell, simply execute the `hsh` executable:

```bash
./hsh
This will start the shell in interactive mode. You will see a prompt like this:

[Go$H] | user@hostname:path$
You can then type commands and press Enter to execute them.

To run the shell in non-interactive mode (e.g., to execute a script), redirect the script to the shell's standard input:

Bash

./hsh < script.sh
Supported Commands
External Commands
The shell can execute any external command that is found in the directories listed in the PATH environment variable.  Examples:

ls
ls -l
date
whoami
cat <filename>
grep <pattern> <filename>
Built-in Commands
The shell supports the following built-in commands:

cd [directory]: Change the current working directory.
With no arguments, cd changes to the home directory ($HOME).
cd - changes to the previous working directory ($OLDPWD).
cd <directory> changes to the specified directory (absolute or relative paths are supported).
exit [status]: Exit the shell.
With no arguments, exits with a status of 0 (success).
With a numeric argument, exits with that status code.
With a non-numeric argument, exits with status code of 2.
quit: An alias for exit.
env: Print the current environment variables.
setenv <variable> <value>: Set an environment variable. If <value> is omitted, it sets the variable to an empty string. If the variable already exists, it overwrites it.
unsetenv <variable>: Remove an environment variable.
selfdestruct [countdown]: A fun command that simulates a self-destruct sequence (for demonstration purposes). It takes an optional countdown timer in seconds.
echo [arguments...]: Displays a line of text, and can also be used for I/O redirection.
echo [arguments] > [filename]: Redirects into a new file.
echo [arguments] >> [filename]: Appends into an existing file.
Piping
Commands can be connected with pipes (|) to send the output of one command to the input of another:

ls -l | wc -l  # Counts the number of files and directories.
Logical Operators
;:
ls -l ; date
&&: Execute the second command only if the first command succeeds (returns 0).
||: Execute the second command only if the first command fails (returns non-zero).
;: Execute commands sequentially, regardless of success or failure.
ls -l && echo "Listing successful"  # "Listing successful" is printed only if ls succeeds.
ls non_existent_file || echo "File not found"  # "File not found" is printed if ls fails.
command1 ; command2 ; command3  # All three commands are executed.
Input/Output Redirection
> (Output Redirection - Create/Overwrite)
Bash

ls -l > file_list.txt   # Saves the output of ls -l to file_list.txt
>> (Output Redirection - Append)
Bash

date >> log.txt       # Appends the current date and time to log.txt
Error Handling
The shell handles the following error conditions:

Command Not Found: If an external command is not found in the PATH, a "not found" message is printed.
cd Errors: Prints error messages if cd fails (e.g., directory not found, HOME or OLDPWD not set).
exit Errors: Prints error messages if you enter an invalid exit code.
execve Errors: The shell uses perror
Piping Errors: The shell implements multiple error handling for pipe creation.
Limitations
Signal handling for SIGINT.
Known Bugs
There are no known bugs. Report any bugs at [insert contact here].
