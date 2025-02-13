#include "main.h"
/**
 * _realloc - reallocates a block of memory for an array of mallocd strings
 * @ptr: pointer
 * @new_size: desired new size of pointer's block
 *
 * Return: pointer, NULL otherwise
 */
void *_realloc_array(char **ptr, unsigned int new_size)
{
	int i, j, realloc_size, old_size = 0;  /* starting at one for ending null element */
	char **new_ptr;

	if (ptr == NULL)
	{
		new_ptr = malloc(new_size);
		if (new_ptr == NULL)
			return (NULL);
		return (new_ptr);
	}

	while (ptr[old_size] != NULL)   /* find the size of ptr */
		old_size++;

	old_size++;  /* NULL element */

	if ((old_size * sizeof(char *)) == new_size)
		return (ptr);

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}

	new_ptr = malloc(new_size);  /* malloc new pointer */
	if (new_ptr == NULL)
		return (NULL);

	if ((int)(new_size / sizeof(char *)) > old_size)  /* find size to copy */
		{
			realloc_size = old_size - 1;
			/* printf("OLD SIZE\n"); */
		}
	else
		{
			realloc_size = new_size / sizeof(void *);
			/* printf("NEW SIZE\n"); */
		}

	for (i = 0; i < realloc_size; i++)  /* copy elements into array*/
	{
		new_ptr[i] = _strdup(ptr[i]);
		if (new_ptr[i] == NULL)  /* if malloc in _strdup failed */
		{
			fprintf(stderr, "malloc failure in _realloc\n");
			for (j = i - 1; j >= 0; j--)
			{
				free(new_ptr[i]);
				return (NULL);
			}
		}
		free(ptr[i]);
		ptr[i] = NULL;
	}

	free(ptr);
	return (new_ptr);
}

/**
 * _strdup - allocates space in memory of a string
 * @str: string
 *
 * Return: pointer to memory, otherwise NULL
 */

char *_strdup(const char *str)
{
	int i = 0;
	char *str_copy = NULL;

	if (str == NULL)
		return (NULL);
	str_copy = malloc(_strlen(str) + 1);
	if (str_copy == NULL)
		return (NULL);

	for (i = 0; str[i] != '\0'; i++)
		str_copy[i] = str[i];
	str_copy[i] = '\0';

	return (str_copy);
}

/**
 * _strncmp - compares two strings
 * @s1: 1st string
 * @s2: 2nd string
 * @n: number of bytes compared
 *
 * Return: 0 if equal, < 0 if s1 < s2, > 0 otherwise
*/
int _strncmp(const char *s1, const char *s2, int n)
{
	int diff = 0;
	int i;
	int s1end = 1;
	int s2end = 1;

	for (i = 0; (s1end != 0) && (s2end != 0) && (i < n); i++)
	{
		if (s1[i] == '\0')
			s1end = 0;
		if (s2[i] == '\0')
			s2end = 0;
		if (s1[i] != s2[i])
		{
			diff += s1[i] - s2[i];
			break;
		}
	}

	return (diff);
}
/**
 * _strtok_r - tokenize a string, thread-safe and reentrant
 * @str: String to be tokenized.
 * @delim: Delimiters.
 * @saveptr: State variable for strtok_r
 *
 * Return: string
 */
char *_strtok_r(char *str, const char *delim, char **saveptr)
{
	char *token;

	if (str == NULL)
		str = *saveptr;
	if (!str) /*No more tokens.*/
		return (NULL);
	/* Find beginning of token (skip delimiters)*/
	str += _strcspn(str, delim);
	if (*str == '\0') /* If we hit the end, return NULL */
	{
		*saveptr = NULL;
		return (NULL);
	}
	/* Find the end of the token*/
	token = str;
	str += _strcspn(str, delim);
	if (*str != '\0') /* If we hit a delimiter, null-terminate and update saveptr */
	{
		*str = '\0';
		*saveptr = str + 1;
	}
	else
	{
		*saveptr = NULL; /*No more tokens*/
	}
	return (token);
}
