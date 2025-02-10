#include "main.h"

/**
 * _strcat - concatenates two strings
 * @dest: string being concatenated to
 * @src: string added to end of dest
 *
 * Return: dest
*/
char *_strcat(char *dest, const char *src)
{
	int i = 0;
	int j;

	while (dest[i] != '\0')
	{
		i++;
	}

	for (j = 0; src[j] != '\0'; j++)
	{
		dest[i] = src[j];
		i++;
	}

	dest[i] = '\0';

	return (dest);
}

/**
 * _strchr - locates a character in a string
 * @s: string
 * @c: character
 *
 * Return: pointer or NULL
*/
char *_strchr(const char *s, char c)
{
	int i;

	if ((s == NULL) || (_strlen(s) == 0))
		return (NULL);

	for (i = 0; i < _strlen(s); i++)
	{
		if (s[i] == c)
			return ((char *)&s[i]);
	}

	return (NULL);
}
/**
 * _strlen - returns the length of a string
 * @s: c string?
 *
 * Return: int
 */
int _strlen(const char *s)
{
	int count = 0;
	int index;

	// if (s == NULL)
	// 	return (0);

	for (index = 0; s[index] != '\0'; index++)
		count++;

	return (count);
}

/**
 * _strcmp - compares two strings
 * @s1: 1st string
 * @s2: 2nd string
 *
 * Return: 0 if equal, < 0 if s1 < s2, > 0 otherwise
*/
int _strcmp(const char *s1, const char *s2)
{
	int diff = 0;
	int i;
	int s1end = 1;
	int s2end = 1;

	for (i = 0; s1end != 0 && s2end != 0; i++)
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
 * _strcpy - copies a string to another string
 * @dest: destination string
 * @src: source string
 *
 * Return: pointer to dest
 */
char *_strcpy(char *dest, const char *src)
{
	int i;

	for (i = 0; src[i] != '\0'; i++)
	{
		dest[i] = src[i];
	}
	dest[i] = '\0';

	return (dest);
}
