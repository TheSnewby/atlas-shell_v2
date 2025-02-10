#include "main.h"
/**
 * _build_path - Builds an absolute path from a current working directory
 *               and a relative path.  Handles ".." and ".".
 * @cwd: The current working directory (absolute path).
 * @rel_path: The relative path.
 * @abs_path: Buffer to store the resulting absolute path.  Must be large
 *            enough (e.g., PATH_MAX + 2).
 *
 * Return: 0 on success, -1 on failure (e.g., buffer overflow).
 */
int _build_path(const char *cwd, const char *rel_path, char *abs_path)
{
	char *current_dir;
	char *path_copy;
	int i = 0;

	if (rel_path[0] == '/')
	{
		/* Absolute path provided, no need to combine with cwd */
		if (_strlen(rel_path) + 1 > PATH_MAX + 2)
		{
			return -1; /* Path to long */
		}
		_strcpy(abs_path, rel_path);
		return 0;
	}
	/* 1. Start with the current working directory. */
	if (_strlen(cwd) + 1 > PATH_MAX + 2)
	{
		return -1;
	}
	_strcpy(abs_path, cwd);

	path_copy = _strdup(rel_path);
	if (path_copy == NULL)
	{
		return -1;
	}
	current_dir = strtok(path_copy, "/");

	while (current_dir != NULL)
	{
		if (_strcmp(current_dir, "..") == 0)
		{
			/* Go up one level.  Find the last '/'. */
			i = _strlen(abs_path);
			while (i > 0)
			{

				if (abs_path[i] == '/')
				{
					abs_path[i] = '\0'; /* Truncate */
					break;
				}
				i--;
			}
			if (i == 0)
			{
				abs_path[1] = '\0'; /*Ensure that it is still root.*/
			}
		}
		else if (_strcmp(current_dir, ".") != 0)
		{
			/* Append. */
			if (_strlen(abs_path) > 1) /* avoid double slash at root */
			{
				_strcat(abs_path, "/");
			}
			if (_strlen(abs_path) + _strlen(current_dir) + 1 > PATH_MAX + 2)
			{
				free(path_copy);
				return -1; /* buffer overflow */
			}
			_strcat(abs_path, current_dir);
		}
		current_dir = strtok(NULL, "/");
	}
	free(path_copy);
	return 0;
}
