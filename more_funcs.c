#include "main.h"
#include "colors.h"

/**
 * selfDestruct - a fun way of slowly exiting the shell with a
 * self-destruct sequence. Prints a fake segmentation fault,
 * counts down from the given number, and exits.
 *
 * @countdown: the number of seconds to count down from before exiting.
 */
void selfDestruct(int countdown)
{
	char *paths[1] = {NULL}; /* environment for execve */
	char **sleep1args = malloc(sizeof(char *) * 3); /* sleep args for execve */
	char **sleep2args = malloc(sizeof(char *) * 3); /* sleep args for execve */

	/* execve sleep command arguments setup */
	sleep1args[0] = "sleep"; /* command to pass to execve */
	sleep2args[0] = "sleep"; /* command to pass to execve */
	sleep1args[1] = "1"; /* specifies how many seconds to sleep (1) */
	sleep2args[1] = "2"; /* specifies how many seconds to sleep (2) */
	sleep1args[2] = NULL; /* Null terminate */
	sleep2args[2] = NULL; /* Null terminate */

	printf("Segmentation fault\n"); /* fake seg fault */
	runCommand("/usr/bin/sleep", sleep1args, paths); /* 1 second delay */
	printf(CLR_RED_BOLD); /* sets the text color to red */
	printf("Shellf destruct mode activated.\n\n");

	if (countdown > 3)
		printf(CLR_DEFAULT); /* reset color so it's no still red bold */

	runCommand("/usr/bin/sleep", sleep2args, paths); /* 2 second delay */

	while (countdown) /* prints countdown */
	{
		if (countdown == 3)
			printf(CLR_RED); /* sets the text color to red for last 3 seconds */

		printf("%d\n", countdown);
		countdown--;
		runCommand("/usr/bin/sleep", sleep1args, paths); /* 1 second delay */
	}

	printf("%s\nThe %sGates Of Shell%s have closed. Goodbye.\n%s",
		   CLR_YELLOW_BOLD, CLR_RED_BOLD, CLR_YELLOW_BOLD, CLR_DEFAULT);

	/* free memory */
	free(sleep1args);
	free(sleep2args);

	exit(EXIT_SUCCESS);
}

/**
 * initCmd - initialize cmd to the command to pass to execve
 * @cmd: variable to be initialized
 * @tokens: tokens
 *
 * Return: command
 */
void initCmd(char **cmd, char *const *tokens)
{
	if (tokens[0][0] != '/' && tokens[0][0] != '.') /* if input isn't a path */
		*cmd = findPath(tokens[0]);
	else /* if user's input is a path */
		*cmd = strdup(tokens[0]); /* initialize cmd to the input path */
}
