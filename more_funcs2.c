#include "main.h"

/**
 * freeIfCmdCd - frees previous_cwd, home, and pwd inn ifCmdCd
 * @previous_cwd: malloced string from _getenv("OLDPWD")
 * @home: malloced string from _getenv("HOME")
 * @pwd: malloced string form _getenv("PWD")
 *
 * Return: void
 */
void freeIfCmdCd(char *previous_cwd, char *home, char *pwd)
{
	if (previous_cwd)
				free(previous_cwd);
			if (home)
				free(home);
			if (pwd)
				free(pwd);

}
