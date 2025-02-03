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
	char *ele_cpy = NULL;

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

	if ((new_size / sizeof(char *)) > old_size)  /* find size to copy */
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

char *_strdup(char *str)
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
