#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <limits.h>
#include <fcntl.h>
#include <ctype.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// --- Colors (for the prompt) ---
#define CLR_DEFAULT "\x1b[0m"
#define CLR_RED_BOLD "\x1b[1;31m"
#define CLR_GREEN_BOLD "\x1b[1;32m"
#define CLR_YELLOW_BOLD "\x1b[1;33m"
#define CLR_BLUE_BOLD "\x1b[1;34m"
#define CLR_DEFAULT_BOLD "\x1b[1m" // Bold, but default color

// --- Constants for logical operator handling ---
typedef enum
{
	SEP_NONE,
	SEP_AND,
	SEP_OR,
	SEP_SEMICOLON
} SeparatorType;

// --- Function Prototypes ---
char **parse_command(char *command);
int execute_command(char *commandPath, char **arguments);
int execute_logical(char *line);
char *findPath(char *command);
void printPrompt(int isAtty, char *user, char *hostname, char *path);
void safeExit(int exit_code);
void shellLoop(int isAtty, char *argv[]);
int customCmd(char **tokens, int isAtty, char *input);
char *_strtok_r(char *str, const char *delim, char **saveptr); // Your custom strtok_r
extern char **environ;
void initialize_environ(void);
char **split_line(char *line, const char *delimiters); // Kept to reuse in logical
int execute_pipe_command(char **commands, int num_commands);
int handle_redirection(char **args);

// --- Global Exit Status Variable ---
int g_exit_status = 0;

// --- Helper Functions ---

void initialize_environ(void)
{
	char **new_environ;
	int i = 0, size = 0;

	/* Calculate initial size of environ */
	while (environ[size] != NULL)
		size++;
	new_environ = malloc((size + 1) * sizeof(char *));
	if (!new_environ)
	{
		perror("malloc failed");
		exit(1); /*  Exit early on allocation failure. */
	}
	for (i = 0; environ[i] != NULL; i++)
	{
		new_environ[i] = strdup(environ[i]); /* make copy */
		if (!new_environ[i])
		{
			perror("strdup failed");
			while (--i >= 0) /* Frees any copies made */
				free(new_environ[i]);
			free(new_environ);
			exit(1); /* Exit on failure */
		}
	}
	new_environ[size] = NULL; /* array has to be NULL terminated */
	environ = new_environ;	  /* makes copy global */
}

char *_strdup(const char *s)
{
	if (s == NULL)
		return NULL;
	size_t len = strlen(s) + 1;
	char *new_str = malloc(len);
	if (new_str == NULL)
		return NULL;
	return memcpy(new_str, s, len);
}
int _strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return *(unsigned char *)s1 - *(unsigned char *)s2;
}
size_t _strcspn(const char *s, const char *reject)
{
	size_t count = 0;
	while (*s)
	{
		if (strchr(reject, *s))
		{
			return count;
		}
		else
		{
			s++;
			count++;
		}
	}
	return count;
}
char *_strchr(const char *s, int c)
{
	while (*s != '\0')
	{
		if (*s == (char)c)
		{
			return (char *)s; // Cast away const for the return
		}
		s++;
	}
	if (c == '\0')
	{
		return (char *)s; // Special case: check for null terminator
	}
	return NULL;
}

char *_strcpy(char *dest, const char *src)
{
	char *start = dest;

	while (*src)
	{
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
	return (start);
}

char *_strcat(char *dest, const char *src)
{
	char *start = dest;

	while (*dest)
		dest++;

	while (*src)
	{
		*dest = *src;
		dest++;
		src++;
	}

	*dest = '\0';
	return (start);
}

char *_strstr(const char *haystack, const char *needle)
{
	if (!*needle) /*  Empty needle */
		return ((char *)haystack);

	while (*haystack)
	{
		const char *h = haystack;
		const char *n = needle;

		while (*h && *n && (*h == *n))
		{
			h++;
			n++;
		}

		if (!*n) /*  Needle found */
			return ((char *)haystack);

		haystack++;
	}

	return (NULL); /*  Needle not found */
}

int _strncmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
	{
		return (0);
	}
	while (--n && *s1 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}
/**
 * _strtok_r - A reentrant version of strtok.
 * @str: The string to be tokenized.  If NULL, continues from last token.
 * @delim: The delimiter characters.
 * @saveptr: A pointer to a char*, used internally to store the next token.
 *
 * Return: A pointer to the next token, or NULL if no more tokens are found.
 */
char *_strtok_r(char *str, const char *delim, char **saveptr)
{
	char *token_start;
	char *token_end;

	if (str != NULL)
	{
		// New string to tokenize
		token_start = str;
	}
	else
	{
		// Continue from last saved position
		if (*saveptr == NULL)
		{
			return NULL; // No previous state, no more tokens
		}
		token_start = *saveptr;
	}

	// Skip leading delimiters
	while (*token_start && strchr(delim, *token_start))
	{
		token_start++;
	}

	if (*token_start == '\0')
	{
		*saveptr = NULL; // No more tokens
		return NULL;
	}

	// Find the end of the token
	token_end = token_start;
	while (*token_end && !strchr(delim, *token_end))
	{
		token_end++;
	}

	if (*token_end != '\0')
	{
		*token_end = '\0';		  // Null-terminate the token
		*saveptr = token_end + 1; // Save the next position
	}
	else
	{
		*saveptr = NULL; // No more tokens after this one
	}

	return token_start;
}

/**
 * free_tokens - Frees a NULL-terminated array of strings.
 * @tokens: The array of strings to free.
 */
void free_tokens(char **tokens)
{
	if (tokens == NULL)
		return;
	for (int i = 0; tokens[i] != NULL; i++)
	{
		free(tokens[i]);
		tokens[i] = NULL; // Prevent dangling pointers
	}
	free(tokens);
}

//-----------------END STRING FUNCS----------------------
//-----------------START BUILTINS------------------------

/**
 * execute_builtin - checks for built-in commands, runs if exists
 *
 * @tokens: tokenized user input
 *
 * Return: 1 if custom, 0 if not
 */
int customCmd(char **tokens, int isAtty, char *input)
{
	if (!tokens || !tokens[0])
		return (0); /* handles NULL */

	if (!_strcmp(tokens[0], "exit"))
	{					   /* built-in #1 "exit" */
		int exit_code = 0; /* handles no arg exit */

		if (tokens[1])					 /* checks if user entered 2nd arg */
			exit_code = atoi(tokens[1]); /* makes 2nd arg the code */

		if (!isAtty)
			free(input);	 /* getline alloc'd so must be freed */
		free_tokens(tokens); /* frees tokens array */
		safeExit(exit_code); /* exits with appropriate code */
		return (1);
	}
	else if (!_strcmp(tokens[0], "env"))
	{ /* built-in #2 "env" */
		char **env = environ;

		while (*env != NULL)
		{
			printf("%s\n", *env);
			env++;
		}
		g_exit_status = 0;
		return (1);
	}
	else if (!_strcmp(tokens[0], "cd"))
	{
		char *dir = tokens[1];
		char *home_dir = getenv("HOME");

		if (dir == NULL || _strcmp(dir, "") == 0)
		{
			if (home_dir == NULL)
			{
				fprintf(stderr, "hsh: cd: HOME not set\n");
				g_exit_status = 1;
				return (1);
			}
			dir = home_dir;
		}
		else if (_strcmp(dir, "-") == 0)
		{
			char *prev_dir = getenv("OLDPWD");
			if (prev_dir == NULL)
			{
				fprintf(stderr, "hsh: cd: OLDPWD not set\n");
				g_exit_status = 1;
				return (1);
			}
			dir = prev_dir;
			printf("%s\n", dir); // Print the directory we're changing to
		}

		char cwd[1024]; // Buffer for current working directory
		if (getcwd(cwd, sizeof(cwd)) == NULL)
		{
			perror("hsh: getcwd failed");
			g_exit_status = 1;
			return (1);
		}

		if (chdir(dir) != 0)
		{
			perror("hsh: cd failed");
			g_exit_status = 1;
			return 1;
		}

		if (setenv("OLDPWD", cwd, 1) != 0) // Update OLDPWD *before* PWD
		{
			perror("hsh: setenv OLDPWD failed");
			g_exit_status = 1;
			return (1);
		}

		if (getcwd(cwd, sizeof(cwd)) == NULL)
		{
			perror("hsh: getcwd failed");
			g_exit_status = 1;
			return (1);
		}
		if (setenv("PWD", cwd, 1) != 0) // Update PWD
		{
			perror("hsh: setenv PWD failed");
			g_exit_status = 1;
			return (1);
		}

		g_exit_status = 0;
		return (1);
	}
	else if (_strcmp(tokens[0], "setenv") == 0)
	{
		if (tokens[1] == NULL || tokens[2] == NULL || tokens[3] != NULL)
		{
			fprintf(stderr, "hsh: setenv: Usage: setenv VARIABLE VALUE\n");
			g_exit_status = 1;
			return (1);
		}
		if (setenv(tokens[1], tokens[2], 1) != 0)
		{
			perror("hsh: setenv failed");
			g_exit_status = 1;
			return (1);
		}
		g_exit_status = 0;
		return (1);
	}
	else if (_strcmp(tokens[0], "unsetenv") == 0)
	{
		if (tokens[1] == NULL || tokens[2] != NULL)
		{
			fprintf(stderr, "hsh: unsetenv: Usage: unsetenv VARIABLE\n");
			g_exit_status = 1;
			return (1);
		}
		if (unsetenv(tokens[1]) != 0)
		{
			perror("hsh: unsetenv failed");
			g_exit_status = 1;
			return (1);
		}
		g_exit_status = 0;
		return (1);
	}
	return (0);
}

//-------------------END BUILTINS-----------------------
//-------------------FIND PATH--------------------------
/**
 * findPath - Locates the full path of a command.
 * @command: The command name.
 *
 * Return: Full path to the command if found and executable,
 *         otherwise returns a copy of the command.  The caller
 *         is responsible for freeing the returned string.
 */
char *findPath(char *command)
{
	char *path = getenv("PATH");
	char *path_copy;
	char *dir;
	char full_path[PATH_MAX];

	// If command contains a '/', it's an absolute or relative path
	if (_strchr(command, '/'))
	{
		if (access(command, X_OK) == 0)
		{
			return _strdup(command); // Return a copy, caller must free
		}
		else
		{
			return NULL; // Not found or not executable
		}
	}

	if (!path)
	{
		return NULL; // No PATH variable
	}

	path_copy = _strdup(path); // Duplicate PATH
	if (!path_copy)
	{
		perror("strdup");
		return NULL;
	}

	char *saveptr; // To be used by strtok

	dir = _strtok_r(path_copy, ":", &saveptr);
	while (dir != NULL)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
		if (access(full_path, X_OK) == 0)
		{
			free(path_copy);
			return _strdup(full_path); // Return a copy!
		}
		dir = _strtok_r(NULL, ":", &saveptr);
	}

	free(path_copy);
	return NULL; // Not found
}
//-------------------END FIND PATH------------------------
//-------------------START EXECUTER-----------------------
int handle_redirection(char **args)
{
	int i = 0;
	int fd;
	int flags = 0;	// To store flags for open
	int append = 0; // Flag to track >> operator

	while (args[i] != NULL)
	{
		if (_strcmp(args[i], ">") == 0)
		{
			flags = O_WRONLY | O_CREAT | O_TRUNC;
			append = 0; // Reset append flag for >
		}
		else if (_strcmp(args[i], ">>") == 0)
		{
			flags = O_WRONLY | O_CREAT | O_APPEND;
			append = 1;
		}
		else if (_strcmp(args[i], "<") == 0)
		{
			flags = O_RDONLY;
		}
		else
		{
			i++;
			continue; // Not a redirection operator, move to the next arg
		}

		if (args[i + 1] == NULL)
		{
			fprintf(stderr, "hsh: syntax error near unexpected token `newline'\n");
			return -1; // Syntax error: no file specified
		}

		// Handle the file
		if (_strcmp(args[i], "<") == 0)
		{
			fd = open(args[i + 1], flags);
		}
		else
		{
			fd = open(args[i + 1], flags, 0666); // Create with rw-rw-rw-
		}

		if (fd == -1)
		{
			perror("hsh: open");
			return -1; // File open failed
		}

		// Redirect
		if (_strcmp(args[i], "<") == 0)
		{
			if (dup2(fd, STDIN_FILENO) == -1)
			{
				perror("hsh: dup2");
				close(fd);
				return -1;
			}
		}
		else
		{ // ">" or ">>"
			if (dup2(fd, STDOUT_FILENO) == -1)
			{
				perror("hsh: dup2");
				close(fd);
				return -1;
			}
		}
		close(fd); // Close the file descriptor after dup2

		// Remove the redirection operator and filename from args
		free(args[i]);
		free(args[i + 1]);
		args[i] = NULL;
		args[i + 1] = NULL;

		// Shift the remaining args
		int j = i + 2;
		while (args[j] != NULL)
		{
			args[j - 2] = args[j];
			j++;
		}
		args[j - 2] = NULL; // Null terminate
	}
	return 0; // Success
}

int execute_command(char *commandPath, char **arguments)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return -1; // Indicate fork failure
	}
	else if (pid == 0)
	{
		// Child process
		// Handle Redirection
		if (handle_redirection(arguments) == -1)
		{
			exit(1); // Exit if redirection fails
		}

		// Attempt execvp first (search PATH)
		execve(commandPath, arguments, environ);

		// If execve returns, there was an error
		fprintf(stderr, "hsh: %s: command not found\n", arguments[0]);
		exit(127); // Command not found exit status
	}
	else
	{
		// Parent process
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
		{
			return WEXITSTATUS(status);
		}
		else if (WIFSIGNALED(status))
		{
			return 128 + WTERMSIG(status); // Consistent with bash signal handling
		}
		else
		{
			// Should not normally get here
			return -1; // Indicate an unexpected error
		}
	}
}

int execute_pipe_command(char **commands, int num_commands)
{
	if (num_commands <= 0)
	{
		return 0; /* Nothing to do */
	}

	if (num_commands == 1)
	{
		/* If there's only one command, we don't need pipes */
		char **args = parse_command(commands[0]);
		if (args != NULL)
		{
			if (args[0] != NULL)
			{
				char *full_path = findPath(args[0]);
				if (full_path != NULL)
				{
					g_exit_status = execute_command(full_path, args);
					free(full_path);
				}
				else
				{
					fprintf(stderr, "%s: command not found\n", args[0]);
					g_exit_status = 127;
				}
			}
			free_tokens(args);
		}
		return g_exit_status;
	}

	/* Create pipes and execute commands */
	int pipes[num_commands - 1][2]; /* Array of pipes */
	pid_t pids[num_commands];
	char **args = NULL;		/* Array to store child PIDs */

	/* Create all necessary pipes */
	for (int i = 0; i < num_commands - 1; i++)
	{
		if (pipe(pipes[i]) < 0)
		{
			perror("pipe");
			return 1; /* Handle pipe creation failure */
		}
	}

	/* Fork and execute each command */
	for (int i = 0; i < num_commands; i++)
	{
		pids[i] = fork();
		if (pids[i] < 0)
		{
			perror("fork");
			return 1; /* Handle fork failure */
		}
		else if (pids[i] == 0)
		{
			/* Child process */
			if (handle_redirection(args) == -1)
			{
				exit(1);
			}

			/* Redirect input (except for the first command) */
			if (i > 0)
			{
				if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0)
				{
					perror("dup2 (stdin)");
					exit(EXIT_FAILURE);
				}
			}

			/* Redirect output (except for the last command) */
			if (i < num_commands - 1)
			{
				if (dup2(pipes[i][1], STDOUT_FILENO) < 0)
				{
					perror("dup2 (stdout)");
					exit(EXIT_FAILURE);
				}
			}

			/* Close all pipe ends in the child */
			for (int j = 0; j < num_commands - 1; j++)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
			}

			/* Parse the command */
			args = parse_command(commands[i]);
			if (args == NULL || args[0] == NULL)
			{
				fprintf(stderr, "Invalid command\n");
				exit(EXIT_FAILURE);
			}

			// Handle Redirection
			if (handle_redirection(args) == -1)
			{
				exit(1);
			}

			char *full_path = findPath(args[0]);
			if (full_path == NULL)
			{
				fprintf(stderr, "%s: command not found\n", args[0]);
				exit(127);
			}

			/* Execute the command */
			execve(full_path, args, environ);
			perror("execve"); /* If execve returns, it failed */
			exit(EXIT_FAILURE);
		}
	}

	/* Parent process: Close all pipe ends */
	for (int i = 0; i < num_commands - 1; i++)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
	}

	/* Wait for all child processes to finish */
	for (int i = 0; i < num_commands; i++)
	{
		waitpid(pids[i], NULL, 0);
	}

	return g_exit_status;
}
//-------------------END EXECUTER-----------------------
//-------------------START PARSER-----------------------
char **split_line(char *line, const char *delimiters)
{
	char **tokens = NULL;
	char *token;
	int bufsize = 64, position = 0;
	char *saveptr;

	tokens = malloc(
		bufsize * sizeof(char *));
	if (!tokens)
	{
		perror("hsh: allocation error");
		return NULL;
	}

	token = _strtok_r(line, delimiters, &saveptr);
	while (token != NULL)
	{
		tokens[position] = _strdup(token); // Duplicate the token!
		if (!tokens[position])
		{
			perror("hsh: allocation error");
			// Clean up previously allocated tokens
			while (--position >= 0)
			{
				free(tokens[position]);
			}
			free(tokens);
			return NULL;
		}
		position++;

		if (position >= bufsize)
		{
			bufsize += 64;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens)
			{
				perror("hsh: allocation error");
				// Clean up previously allocated tokens (including the newly strdup'd one)
				while (--position >= 0)
				{
					free(tokens[position]);
				}
				free(tokens);
				return NULL;
			}
		}

		token = _strtok_r(NULL, delimiters, &saveptr);
	}
	tokens[position] = NULL;
	return tokens;
}
/**
 * parse_command - Parses a command string into an array of arguments.
 * @command: The command string.
 *
 * Return: A NULL-terminated array of strings (arguments) or NULL on error.
 */
char **parse_command(char *command)
{
	char **args = split_line(command, " \t\r\n\a");
	if (args == NULL || args[0] == NULL)
		return args;

	// Handle variable expansion
	for (int i = 0; args[i] != NULL; i++)
	{
		if (args[i][0] == '$')
		{
			char *var_name = args[i] + 1;
			char *var_value = getenv(var_name);
			if (var_value != NULL)
			{
				free(args[i]);
				args[i] = _strdup(var_value);
				if (args[i] == NULL)
				{
					free_tokens(args);
					return NULL;
				}
			}
		}
	}
	return args;
}
int split_command_line_on_pipe(char *input, char ***commands, int *num_commands)
{
	char *token;
	int count = 0;
	char *input_copy;

	/* Count the number of pipes */
	for (int i = 0; input[i] != '\0'; i++)
	{
		if (input[i] == '|')
		{
			count++;
		}
	}

	/* Allocate memory for commands array + NULL terminator */
	*commands = malloc(sizeof(char *) * (count + 2));
	if (*commands == NULL)
	{
		perror("malloc");
		return -1;
	}

	input_copy = _strdup(input); /* Duplicate the input string */
	if (input_copy == NULL)
	{
		perror("_strdup");
		free(*commands);
		return -1;
	}
	char *saveptr;
	token = _strtok_r(input_copy, "|", &saveptr); /* Tokenize based on pipe*/

	int i = 0;
	while (token != NULL)
	{
		(*commands)[i] = _strdup(token); /* Duplicate and store each token */
		if ((*commands)[i] == NULL)
		{
			perror("_strdup");
			/* Free previously allocated memory before returning */
			for (int j = 0; j < i; j++)
			{
				free((*commands)[j]);
			}
			free(*commands);
			free(input_copy);
			return -1;
		}
		i++;
		token = _strtok_r(NULL, "|", &saveptr); /* get next command */
	}

	(*commands)[i] = NULL; /* Null-terminate array */
	*num_commands = i;	   /* sets number of commands */
	free(input_copy);
	return 0; /* success */
}

//-------------------END PARSER-----------------------
//-------------------START LOGICAL OPS----------------

/**
 * execute_command_group - Executes a group of commands separated by && or ||
 * @command_group: The group of commands to execute
 *
 * Return: The exit status of the last command executed.
 */
int execute_command_group(char *command_group) {
    char *and_op;
    char *or_op;
    char *next_group;
    int status;

    // Handle Empty string
    if (!command_group || *command_group == '\0'){
        return 0;
    }
    // Split on && first.  Find the *first* occurrence.
    and_op = _strstr(command_group, "&&");
    if (and_op != NULL) {
        *and_op = '\0';  // Split the string
        next_group = and_op + 2;
        status = execute_command_group(command_group);
        if (status == 0) {
            return execute_command_group(next_group);
        }
        return status;
    }

    // If no &&, split on ||.  Find the *first* occurrence.
    or_op = _strstr(command_group, "||");
    if (or_op != NULL) {
        *or_op = '\0';  // Split the string
        next_group = or_op + 2;
        status = execute_command_group(command_group);
        if (status != 0) {
            return execute_command_group(next_group);
        }
        return status;
    }

    // Base case: no && or || operators.  Execute as a single command.
    char **args = parse_command(command_group);
    if (args == NULL) {
        return 1; // Parse error
    }
    if (args[0] == NULL){
        free_tokens(args);
        return 0;
    }
     char *full_path = findPath(args[0]);
    if (full_path == NULL) {
        fprintf(stderr, "hsh: %s: command not found\n", args[0]);
        free_tokens(args);
        return 127;
    }
     g_exit_status = execute_command(full_path, args); // Update global status
    free(full_path);
    free_tokens(args);
    return g_exit_status; // Return the status
}
/**
 * execute_logical - Executes a sequence of commands with logical operators.
 * @line: The command line.
 *
 * Return: The exit status of the last executed command.
 */
int execute_logical(char *line)
{
	return execute_command_group(line);
}
//---------------------END LOGICAL OPS-----------------
//-------------------START SHELL LOOP------------------
void handle_input(char *input, int isAtty, char *argv[]) {
    int num_commands;
    char **commands = NULL;

    // Handle NULL input
    if (input == NULL) {
        fprintf(stderr, "hsh: input error\n");
        return;
    }

    // Remove trailing newline
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }

    // --- Expand environment variables (including $?) ---
    char *dollar_sign = strchr(input, '$');
    while (dollar_sign != NULL) {
        char *var_name_start = dollar_sign + 1;
        char *var_name_end = var_name_start;

        // Find the end of the variable name (alphanumeric and underscore)
        while (*var_name_end != '\0' && (isalnum(*var_name_end) || *var_name_end == '_' || *var_name_end == '?')) {
            var_name_end++;
        }

        // Create the variable name string
        size_t var_name_len = var_name_end - var_name_start;
        char *var_name = malloc(var_name_len + 1);
        if (var_name == NULL) {
            perror("malloc");
            return;
        }
        strncpy(var_name, var_name_start, var_name_len);
        var_name[var_name_len] = '\0';

        // Get the variable's value
        char *var_value;

        if (strcmp(var_name, "?") == 0) { // Handle $? specially
            char exit_status_str[16];
            snprintf(exit_status_str, sizeof(exit_status_str), "%d", g_exit_status);
            var_value = exit_status_str;
        }
        else {
             var_value = getenv(var_name);
        }


        if (var_value != NULL) {
            // Calculate sizes for memmove and memcpy
            size_t prefix_len = dollar_sign - input; // Length before $
            size_t value_len = strlen(var_value);    // Length of the variable's value
            size_t suffix_len = strlen(var_name_end);    // Length after variable name
            size_t new_len = prefix_len + value_len + suffix_len; // Total new length

            // Create a temporary buffer to hold the expanded string
             char *temp_input = malloc(new_len + 1);
            if (temp_input == NULL){
                perror("malloc");
                free(var_name);
                return;
            }

            // 1. Copy the part *before* the '$'
            strncpy(temp_input, input, prefix_len);

            // 2. Copy the *value* of the variable
            strcpy(temp_input + prefix_len, var_value);

            // 3. Copy the part *after* the variable name
            strcpy(temp_input + prefix_len + value_len, var_name_end);

            //Update input
            free(input);
            input = temp_input;

        }
        free(var_name);

        // Find the *next* occurrence of '$' (if any) in the MODIFIED input
        dollar_sign = strchr(input, '$');
    }
    // --- END OF ENVIRONMENT VARIABLE EXPANSION ---

    // First, split by semicolons
    char **semicolon_separated = split_line(input, ";");
    if (semicolon_separated == NULL)
    {
        return; // Allocation error
    }

    for (int i = 0; semicolon_separated[i] != NULL; i++)
    {
        // Now, check for pipes WITHIN each semicolon-separated part
        if (_strchr(semicolon_separated[i], '|') != NULL)
        {
            if (split_command_line_on_pipe(semicolon_separated[i], &commands, &num_commands) == 0)
            {
                g_exit_status = execute_pipe_command(commands, num_commands);
                for (int j = 0; j < num_commands; j++)
                {
                    free(commands[j]);
                }
                free(commands);
            }
            else
            {
                fprintf(stderr, "Failed to split commands for piping\n");
                g_exit_status = 1; // Indicate an error
            }
        }
        else if (_strstr(semicolon_separated[i], "&&") || _strstr(semicolon_separated[i], "||"))
        {
            // Handle logical operators WITHIN the pipe stage (or if no pipes)
            g_exit_status = execute_logical(semicolon_separated[i]);
        }
        else
        {
            // Single command (no pipes, no logical operators)
            char **tokens = parse_command(semicolon_separated[i]);
            if (tokens == NULL)
            {
                free_tokens(semicolon_separated);
                return;
            }

            if (tokens[0] != NULL)
            {
                if (customCmd(tokens, isAtty, semicolon_separated[i]) == 1)
                {
                    free_tokens(tokens);
                    continue;
                }

                char *full_path = findPath(tokens[0]);
                if (full_path == NULL)
                {
                    fprintf(stderr, "%s: 1: %s: not found\n", argv[0], tokens[0]);
                    g_exit_status = 127;
                }
                else
                {
                    g_exit_status = execute_command(full_path, tokens);
                    free(full_path);
                }
            }
            free_tokens(tokens);
        }
    }
    free_tokens(semicolon_separated); // Free the semicolon-separated array

}

void shellLoop(int isAtty, char *argv[])
{
	size_t size;
	char *user, *hostname, path[PATH_MAX], *input;

	while (1)
	{
		// Initialize variables
		getcwd(path, sizeof(path));
		user = getenv("USER"); // Use getenv for USER
		hostname = getenv("HOSTNAME");
		if (!user)
			user = "user"; // Default val
		if (!hostname)
			hostname = "hostname";
		size = 0;
		input = NULL;

		printPrompt(isAtty, user, hostname, path);

		if (getline(&input, &size, stdin) == -1)
		{
			if (isAtty)
			{
				printf("\n"); // Newline on Ctrl+D in interactive mode
			}
			free(input);			 // Free before safe exit
			safeExit(g_exit_status); // Exit with the last command's status
		}

		// Remove trailing newline
		size_t len = strlen(input);
		if (len > 0 && input[len - 1] == '\n')
		{
			input[len - 1] = '\0';
		}

		handle_input(input, isAtty, argv);

		free(input); // Free input
	}
}

/**
 * printPrompt - prints prompt in color ("[Go$H] | user@hostname:path$ ")
 *
 * @isAtty: is interactive mode
 * @user: environment variable for user's username
 * @hostname: environment variable for user's hostname or device name.
 * @path: current working directory
 */
void printPrompt(int isAtty, char *user, char *hostname, char *path)
{
	if (isAtty)
	{
		printf("%s[%sGo$H%s]%s | ", CLR_YELLOW_BOLD, CLR_RED_BOLD,
			   CLR_YELLOW_BOLD, CLR_DEFAULT);
		printf("%s%s@%s", CLR_GREEN_BOLD, user, hostname);
		printf("%s:%s%s", CLR_DEFAULT_BOLD, CLR_BLUE_BOLD, path);
		printf("%s$ ", CLR_DEFAULT);
	}
}

//---------------------END SHELL LOOP-----------------

/**
 * safeExit - exits after freeing environ
 * @exit_code: exit code for exit()
 *
 * Return: void
 */
void safeExit(int exit_code)
{
	int i;

	if (environ)
	{
		for (i = 0; environ[i] != NULL; i++)
			free(environ[i]);
		free(environ);
		environ = NULL;
	}

	exit(exit_code);
}

int main(int argc, char *argv[])
{
	int isInteractive = isatty(STDIN_FILENO);

	initialize_environ();
	(void)argc;

	shellLoop(isInteractive, argv);

	return (EXIT_SUCCESS); // Should never reach here in normal operation
}
