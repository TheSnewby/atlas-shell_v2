#include "main.h"

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

	printf("Inside buildListPath\n");
	if (path == NULL)
	{
		printf("PATH is NULL\n"); /*DEBUG*/
		return (NULL);
	}
	temp_path = _strdup(path);
	token = strtok(temp_path, ":");

	free(path); /* Free the duplicated path from _getenv */
	while (token != NULL)
	{
		printf("Adding directory to path: %s\n", token); /*DEBUG*/
		new_node = (path_t *)malloc(sizeof(path_t));
		if (new_node == NULL)
		{
			free(temp_path);
			destroyListPath(head); /* Clean up on failure */
			return (NULL);
		}
		new_node->directory = _strdup(token);
		if (new_node->directory == NULL)
		{
			free(temp_path);
			free(new_node);
			destroyListPath(head);
			return (NULL);
		}
		new_node->next = head;
		head = new_node;
		token = strtok(NULL, ":");
	}
	free(temp_path);
	return (head);
}

/**
 * findPath - finds the path of a given command
 * @name: name of command
 *
 * Return: path if found, name if not found
 */
char *findPath(char *name)
{
	path_t *temp = NULL;
	path_t *head = NULL;
	char *temp_path = NULL, *mallocd_name = NULL;

	printf("Inside findPath, looking for: %s\n", name); /*DEBUG*/
	if (_strchr(name, '/') && !access(name, F_OK)) /* checks if path already */
	{
		mallocd_name = _strdup(name);
		return (mallocd_name);
	}

	head = buildListPath(); /* populates list and points at head */
	if (head == NULL)
	{
		printf("buildListPath returned NULL\n"); /*DEBUG*/
		return (NULL); /*Return copy of name*/
	}
	temp = head; /* iterator initialization */

	while (temp != NULL) /* run until list is empty */
	{					 /* malloc space for path/name\0 */
		temp_path = malloc(_strlen(temp->directory) + _strlen(name) + 2);
		if (temp_path == NULL)
		{
			destroyListPath(head);
			return (NULL);
		}
		_strcpy(temp_path, temp->directory);
		_strcat(temp_path, "/");
		_strcat(temp_path, name);
		printf("Checking path: %s\n", temp_path);/*DEBUG*/
		if (access(temp_path, F_OK) == 0) /* checks if cmd at path exists */
		{
			printf("Executable found at: %s\n", temp_path); /*DEBUG*/
			destroyListPath(head); /* frees list of paths */
			return (temp_path);	   /* returns found path + name */
		}
		free(temp_path);   /* frees temp_path */
		temp = temp->next; /* go to next location */
	}
	destroyListPath(head);
	printf("Command not found in PATH\n"); /*DEBUG*/
	return (NULL);		   /* returns malloced command name without a path */
}

/**
 * destroyListPath - frees the ListPath
 * @h: head of listpath
 */
void destroyListPath(path_t *h)
{
	path_t *temp = NULL;

	while (h != NULL)
	{
		temp = h;
		h = h->next;
		free(temp->directory);
		free(temp);
	}
}
char *getHostname(void)
{
	char *hostname = _getenv("NAME");
	char *temp;

	if (!hostname)
		hostname = _getenv("HOSTNAME");
	if (!hostname)
		hostname = _getenv(("WSL_DISTRO_NAME"));
	if (!hostname)
	{
		return (_strdup("unknown")); /* ALWAYS duplicate */
	}
	else
	{
		temp = _strdup(hostname);
		free(hostname);
		return (temp);
	}
}

/**
 * getUser - retrives user from env or sets a new user
 *
 * Return: user
 */
char *getUser(void)
{
	char *user = _getenv("USER");
	char *temp;

	if (!user)
		user = _getenv("LOGNAME");
	if (!user)
	{
		return (_strdup("unknown")); /* direct return */
	}
	else
	{ /* always duplicate */
		temp = _strdup(user);
		free(user);
		return (temp);
	}
}
