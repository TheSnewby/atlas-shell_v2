<img width="1467" height="369" alt="image" src="https://github.com/user-attachments/assets/08a541b4-0d80-4b32-8cbb-03103686f523" />

# Atlas Shell (hsh)

"The Shell Project" was my favorite project at Atlas School where we had the chance to build a CLI with significant functionality and personality. There were multiple challenges including merge resolve conflicts, the order of operations for parsing and executing the commands, and a lack of being able to meet in person. But in the end it's one of my favorite projects for it's technical accomplishments and memorable quirks.

[Atlas Shell v2 Project Page](https://github.com/TheSnewby/atlas-shell_v2)
[Stephen Newby's LinkedIn Profile](https://www.linkedin.com/in/stephenjnewby/)

## Authors

*   Stephen Newby
*   Nathan Wilson
*   Deshon Stout
*	Daniel Stelljes (v1)

## Description

Atlas Shell (hsh) is a simple Unix shell implemented in C as a learning project.  It supports:

*   **Basic command execution:** Running external commands found in the `PATH`.
*   **Built-in commands:**  `cd`, `exit` (and `quit`), `env`, `setenv`, `unsetenv`, `cd`, and a custom `selfdestruct` command.
*   **Piping:**  Connecting the standard output of one command to the standard input of another (e.g., `ls -l | wc -l`).  Supports single pipes.
*   **Logical Operators:**  `&&` (AND), `||` (OR), and `;` (semicolon) for conditional and sequential command execution.
*   **Input/Output Redirection**: `>` (create/overwrite), `>>` (append), `<` (read from file), and `<<` (here document).
*   **Environment Variable Handling:**  Correctly handles the all environmental variables like `PATH`, `HOME`, and `OLDPWD`.  Allows users to set and unset environment variables.
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

This will start the shell in interactive mode. You will se a prompt like this:

[Go$H] | user@hostname:path$
You can then type commands and press **Enter** to execute them.

To run the shell in **non-interactive mode** (e.g., to execute a script), redirect the script to the shell's standard input:

```bash
./hsh < script.sh
```

## Supported Commands  

### External Commands  

The shell can execute any external command found in the directories listed in the `PATH` environment variable. Examples:

- `ls`
- `ls -l`
- `date`
- `whoami`
- `cat <filename>`
- `grep <pattern> <filename>`

### Built-in Commands  

- **`cd [directory]`**: Change the current working directory.  
  - With no arguments, `cd` changes to the home directory (`$HOME`).  
  - `cd -` changes to the previous working directory (`$OLDPWD`).  
  - `cd <directory>` changes to the specified directory (absolute or relative paths are supported).  

- **`exit [status]`**: Exit the shell.  
  - With no arguments, exits with a status of `0` (success).  
  - With a numeric argument, exits with that status code.  
  - With a non-numeric argument, exits with status code `2`.  

- **`quit`**: Alias for `exit`.  

- **`env`**: Print the current environment variables.  

- **`setenv <variable> <value>`**: Set an environment variable.  
  - If `<value>` is omitted, it sets the variable to an empty string.  
  - If the variable already exists, it overwrites it.  

- **`unsetenv <variable>`**: Remove an environment variable.  

- **`selfdestruct [countdown]`**: A fun command that simulates a self-destruct sequence (for demonstration purposes). It takes an optional countdown timer in seconds.  

## Piping  

Commands can be connected with pipes (`|`) to send the output of one command to the input of another:

```bash
ls -l | wc -l  # Counts the number of files and directories.
```

## Logical Operators  

- **`&&`**: Execute the second command **only if** the first command succeeds (returns `0`).
- **`||`**: Execute the second command **only if** the first command fails (returns non-zero).
- **`;`**: Execute commands sequentially, regardless of success or failure.

Examples:

```bash
ls -l && echo "Listing successful"  # "Listing successful" is printed only if ls succeeds.
ls non_existent_file || echo "File not found"  # "File not found" is printed if ls fails.
command1 ; command2 ; command3  # All three commands are executed.
```

## Input/Output Redirection  

- **`>` (Output Redirection - Create/Overwrite)**  
  ```bash
  ls -l > file_list.txt   # Saves the output of ls -l to file_list.txt
  ```
- **`>>` (Output Redirection - Append)**  
  ```bash
  date >> log.txt       # Appends the current date and time to log.txt
  ```
- **`<` (Input Redirection - Read from File)**  
  ```bash
  sort < unsorted.txt   # Reads input from unsorted.txt
  ```
- **`<<` (Here Document - Multiline Input)**  
  ```bash
  cat << EOF
  This is a multi-line input
  that ends when EOF is encountered.
  EOF
  ```

## Error Handling  

The shell handles the following error conditions:

- **Command Not Found**: If an external command is not found in `PATH`, a "not found" message is printed.  
- **`cd` Errors**: Prints error messages if `cd` fails (e.g., directory not found, `HOME` or `OLDPWD` not set).  
- **`exit` Errors**: Prints error messages if you enter an invalid exit code.  
- **`execve` Errors**: The shell uses `perror` to display system errors.  
- **Piping Errors**: The shell implements multiple error handling mechanisms for pipe creation.  

## Limitations  

- **Signal handling for SIGINT (Ctrl+C).**  

## Known Bugs  

There are no known bugs.
