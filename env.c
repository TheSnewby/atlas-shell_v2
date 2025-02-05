#include "main.h"

char **saved_environ = NULL;

/**
 * _getenv - gets malloc'd environmental value from a name-value pair in environ
 * @name: name in name-value pair
 *
 * Return: dynamically allocated value if found, NULL if not
 */
char *_getenv(const char *name)
{
	char **current;
	char *token;
	char *temp_line;
	char *value = NULL;

	if (!environ || !name)
		return (NULL);

	for (current = environ; *current; current++)
	{
		temp_line = _strdup(*current);
		if (!temp_line)
			return (NULL);

		token = strtok(temp_line, "=");
		if (_strcmp(token, name) == 0)
		{
			value = strtok(NULL, "=");
			if (value)
				value = _strdup(value); /* ensures value isn't dependent on
									   temp_line ptr */
			free(temp_line);
			return (value);
		}
		free(temp_line);
	}
	return (NULL);
}

/**
 * _setenv - sets an environmental variable to a new value,
 * or appends a new value if not found
 * @name: name of environmental variable to be set
 * @value: value to set the environmental variable to
 * @overwrite: nonzero to change the value, zero to not change
 *
 * Return: 0 on success, -1 on failure
 */
int _setenv(const char *name, const char *value, int overwrite)
{
	int i, size_environ = 0;
	char *temp, *new_line, *temp_line;
	char **new_environ;

	if (!name || !value || (_strlen(name) == 0) || _strchr(name, '='))
		return (-1);

	/* Calculate the size needed for "name=value\0" */
	size_t new_line_len = _strlen(name) + _strlen(value) + 2;
	new_line = malloc(new_line_len);
	if (new_line == NULL)
		return (-1);

	/* Build the "name=value" string using allowed functions */
	_strcpy(new_line, name);
	_strcat(new_line, "=");
	_strcat(new_line, value);

	for (i = 0; environ[i] != NULL && overwrite != 0; i++) /* looks for name */
	{
		temp_line = _strdup(environ[i]);
		if (!temp_line)
		{
			free(new_line);
			return (-1);
		}

		temp = strtok(temp_line, "=");

		if (_strcmp(temp, name) == 0) /* name found in environ */
		{
			free(environ[i]);
			environ[i] = _strdup(new_line);
			free(temp_line);
			free(new_line);
			return (0);
		}
		free(temp_line);
	}

	size_environ = i;
	if (overwrite != 0 && environ[i] == NULL) /* if adding at end */
	{
		new_environ = _realloc_array(environ, sizeof(char *) * (size_environ + 2));
		if (new_environ == NULL)
		{
			free(new_line);
			return (-1);
		}

		new_environ[size_environ] = new_line; /* sets new env variable */
		new_environ[size_environ + 1] = NULL; /* makes final as NULL */
		environ = new_environ;
	}
	else
	{
		free(new_line); /* Free if not added */
	}

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

	if ((name == NULL) || (_strlen(name) == 0))
		return (-1);

	/* find size of array and location of possible match */
	for (i = 0; environ[i] != NULL; i++)
	{
		size_environ++;
		if ((_strncmp(environ[i], name, _strlen(name)) == 0) &&
			(environ[i][_strlen(name)] == '='))
			location = i;
	}
	/* if match found rebuild environ without the found element */
	if ((location < i) && (location != -1))
	{
		new_environ = malloc(sizeof(char *) * size_environ);
		for (i = 0; i < size_environ; i++)
		{
			if (i != location) /* copy all except target variable */
			{
				new_environ[new_environ_index] = _strdup(environ[i]);
				new_environ_index++;
			}
			free(environ[i]);
		}
		new_environ[new_environ_index] = NULL;

		free(environ);
		environ = new_environ;
	}
	return (0);
}

/**
 * ifCmdUnsetEnv - unsets an env variable if found
 * @tokens: tokenized list of commands
 *
 * Return: 1 if successful, otherwise 0
 */
int ifCmdUnsetEnv(char **tokens)
{
	if (tokens[0] != NULL && (_strcmp(tokens[0], "unsetenv") == 0))
	{
		if (_unsetenv(tokens[1]) == 0)
			return (1);
	}
	return (0);
}

/**
 * initialize_environ - makes environ a dynamically allocated variable
 */
void initialize_environ(void)
{
	int i = 0, size_environ = 0;
	char **new_environ;

	while (environ[i] != NULL)
		i++;
	size_environ = i;

	new_environ = malloc(sizeof(char *) * (size_environ + 1));
	if (new_environ == NULL)
	{
		perror("malloc failed");
		return;
	}

	for (i = 0; i < size_environ; i++) /* populates new_environ */
	{
		new_environ[i] = _strdup(environ[i]);
		if (new_environ[i] == NULL) /* if malloc in _strdup fails, undo all */
		{
			for (i = i - 1; i >= 0; i--)
			{
				free(new_environ[i]);
			}
			free(new_environ);
			fprintf(stderr, "malloc fail in initialize_environ\n");
			return; /* consider changing to safeExit(errno) */
		}
	}
	new_environ[size_environ] = NULL;
saved_environ = environ;
	environ = new_environ;
}
