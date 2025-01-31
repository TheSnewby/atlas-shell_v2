#include "main.h"

/**
 * _getenv - gets an environmental value from a name-value pair in environ
 * @name: name in name-value pair
 *
 * Return: value if found, NULL if not
 */
char *_getenv(const char *name) /* gets an environmental variable */
{
	char **current;
	char *token;
	char *temp_line;
	char *temp = NULL;

	if (environ == NULL)
		return (NULL);
	for (current = environ; *current; current++)
	{
		temp_line = strdup(*current);
		token = strtok(temp_line, "=");
		if (strcmp(token, name) == 0)
		{
			temp = strdup(strtok(NULL, "="));
			free(temp_line);
			return (temp);
		}
		free(temp_line);
	}
	return (NULL);
}

/**
 * buildListPath - builds a singly linked list off the environ variable PATH
 *
 * Return: singly linked list of PATH variables, NULL if failed
 */
path_t *buildListPath(void)
{
	path_t *new_node = NULL;
	path_t *head = NULL;
	char *path = _getenv("PATH");
	char *temp_path = NULL;
	char *token = NULL;

	if (path == NULL)
		return (NULL);
	temp_path = strdup(path);
	token = strtok(temp_path, ":");

	free(path);
	while (token != NULL)
	{
		new_node = (path_t *)malloc(sizeof(path_t));
		if (new_node == NULL)
		{
			free(temp_path);
			return (NULL);
		}
		new_node->directory = strdup(token);
		new_node->next = head;
		head = new_node;
		token = strtok(NULL, ":");
	}
	free(temp_path);
	return (head);
}

/**
 * _setenv - sets an environmental variable to a new value,
 * or appends a new value if not found
 * @name: name of environmental variable to be set
 * @value: value to set the environmental variable to
 * @overwrite: nonzero to actually change the value, zero to not change
 *
 * Return: 0 on success, -1 on failure
 */
int _setenv(const char *name, const char *value, int overwrite)
{
	int i;
	char *temp;
	char *new_line;
	char *temp_line;
	char found = -1; /* flag for finding name, 0 if found */
	int size_environ = 0;
	char **new_environ;

	if (name == NULL)
		return (-1);
	else if (strlen(name) == 0) /* || strchr(name, '=') == NULL) */
		return (-1);

	new_line = malloc(strlen(name) + strlen(value) + 2); /* line replacement */
	if (new_line == NULL)
		return (-1);
	strcpy(new_line, name);
	strcat(new_line, "=");
	strcat(new_line, value);

	for (i = 0; environ[i] != NULL && overwrite != 0; i++) /* looks for name */
	{
		temp_line = strdup(environ[i]);
		temp = strtok(temp_line, "=");
		size_environ++;

		if (strcmp(temp, name) == 0) /* name found in environ */
		{
			environ[i] = strdup(new_line);
			found = 0;
			break;
		}
	}
	if (found != 0 && overwrite != 0 && environ[i] == NULL) /* if adding at end */
	{
		new_environ = malloc(sizeof(char *) * size_environ + 2);
		if (new_environ == NULL)
		{
			free(new_line);
			return (-1);
		}
		for (i = 0; i < size_environ; i++)
		{
			new_environ[i] = strdup(environ[i]);
		}
		new_environ[i] = new_line;
		new_environ[i + 1] = NULL;
	}
	environ = new_environ;
	return (0);
}

/**
 * _unsetenv - unsets an environmental variable
 * @name: name of environmental variable to remove
 *
 * Return: 0 if success, -1 on failure
 */
int _unsetenv(const char *name)
{
	/* check name, check environ, if found new_environ without name */
	int size_environ = 0;
	char **new_environ;
	int location = -1, i, new_environ_index = 0;

	if (name == NULL)
		return (-1);
	else if (strlen(name) == 0)
		return (-1);
	/* find size of array and location of possible match */
	for (i = 0; environ[i] != NULL; i++)
	{
		size_environ++;
		if (strncmp(environ[i], name, strlen(name)) == 0)
			location = i;
	}
	/* if match found rebuild environ without the found element */
	if (location < i && location != -1)
	{
		new_environ = malloc(sizeof(char *) * size_environ);
		for (i = 0; i < size_environ; i++)
		{
			if (i != location)
			{
				new_environ[new_environ_index] = strdup(environ[i]);
				new_environ_index++;
			}
		}
		new_environ[new_environ_index] = NULL;
		environ = new_environ;
	}
	return (0);
}

/**
 * runCommand - runs execve on a command. Handles forking and errors.
 *
 * @commandPath: command to run, including path
 * @args: array of args for commandPath, including the command (without path)
 * @envPaths: paths for the environment
 *
 * Return: 0 on success, -1 on failure, errno on failure from child process.
 */
int runCommand(char *commandPath, char **args, char **envPaths)
{
	int exec_rtn = 0, child_status, wexitstat;
	pid_t fork_rtn, wait_rtn;

	if (commandPath == NULL)
	{
		if (isatty(STDIN_FILENO))
			return (0);
		exit(0);
	}

	if (access(commandPath, F_OK) != 0) /* checks if cmd doesn't exist */
	{
		return (127);
	}

	fork_rtn = fork(); /* split process into 2 processes */
	if (fork_rtn == -1) /* Fork! It failed */
	{
		return (EXIT_FAILURE); /* indicate error */
	}
	if (fork_rtn == 0) /* child process */
	{
		exec_rtn = execve(commandPath, args, envPaths);/*executes user-command*/
		if (exec_rtn == -1)
		{
			exit(errno); /* indicate error */
		}
	} else /* parent process; fork_rtn contains pid of child process */
	{
		wait_rtn = waitpid(fork_rtn, &child_status, WUNTRACED);
		/* waits until child process terminates */
		if (WIFEXITED(child_status))
		{
			wexitstat = WEXITSTATUS(child_status);
			return (wexitstat);
		}
		else if (wait_rtn == -1)
		{
			return (-1); /* indicate error */
		}
	}
	return (0); /* success */
}
