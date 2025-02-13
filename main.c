#include "main.h"
#include <signal.h>

/**
 * main - starts the program and the loop
 * @argc: number of arguments
 * @argv: array of arguments
 *
 * Return: 0
 */
int main(int argc, char *argv[])
{
	int isInteractive = isatty(STDIN_FILENO);

	/* ------------------- On entry - one time execution ------------------- */
	(void)argc;
	if (isInteractive)
		printf("%sWelcome to the %sGates Of Shell%s. Type 'exit' to quit.\n\n",
			   CLR_YELLOW_BOLD, CLR_RED_BOLD, CLR_YELLOW_BOLD);
	/* --------------------------------------------------------------------- */
	initialize_environ(); /* makes environ dynamically allocated */

	shellLoop(isInteractive, argv); /* main shell loop */

	/* ------------------- On exit - one time execution ------------------- */
	if (isInteractive)
		printf("%s\nThe %sGates Of Shell%s have closed. Goodbye.\n%s",
			   CLR_YELLOW_BOLD, CLR_RED_BOLD, CLR_YELLOW_BOLD, CLR_DEFAULT);

	return (EXIT_SUCCESS);
	/* -------------------------------------------------------------------- */
}

/**
 * executeIfValid - check if a command is a valid custom or built-in command;
 * run the command if it is valid; if child process fails,stop it
 * from re-entering loop
 *
 * @isAtty: result of isatty(), 1 if interactive, 0 otherwsie
 * @argv: carrier of filename in [0]
 * @tokens: array of strings of user inputs delimited by spaces
 */
void executeIfValid(int isAtty, char *const *argv, char **tokens, char *input)
{
	int custom_cmd_rtn;
  
  /* Check for empty command *before* doing anything else. */
	if (tokens[0] == NULL)
	{
		return;
	}

	/* --- Built-in Command Handling --- */
	if (custom_cmd_rtn)  /* user input is customCmd */
	{
		if (custom_cmd_rtn == 2) /* false directory */
			fprintf(stderr, "%s: 1: cd: can't cd to %s\n", argv[0], tokens[1]);
		else if (custom_cmd_rtn == 3)  /* too many arguments */
			fprintf(stderr, "%s: 1: cd: too many arguments\n", argv[0]);

		if ((custom_cmd_rtn == -1) && !isAtty)
			{
				resetAll(tokens, input, NULL);
				safeExit(EXIT_SUCCESS);
			}
    return;
	}

	/* --- External Command Handling --- */
	/* *Now* we find the path, since it's not a built-in. */
	char *full_path = findPath(tokens[0]);
	if (full_path == NULL)
	{
		fprintf(stderr, "%s: 1: %s: not found\n", argv[0], tokens[0]);
		if (!isAtty)
		{
			safeExit(127);
		}
		return; /* Return here after handling "not found" */
	}

	/* Execute the command (using the FULL PATH and ALL arguments) */
	int run_cmd_rtn = execute_command(full_path, tokens);
	free(full_path); /* Free *after* execute_command */

	/* Handle errors from execute_command */
	if (run_cmd_rtn != 0)
	{
		if (run_cmd_rtn == -1)
		{
			perror("fork"); /* Most likely cause if execute_command fails */
		}
    else if(run_cmd_rtn == 2)
				;
		else
		{
      /* Other execve errors: use perror to print a descriptive message */
			fprintf(stderr, "%s: 1: %s: ", argv[0], tokens[0]);
			errno = run_cmd_rtn;
			perror("Command failed"); /* Generic error, use perror */
		}

		if (!isAtty)
		{
			safeExit(run_cmd_rtn); /* Exit with the error code */
		}
	}
}

/**
 * resetAll - frees all dynamically allotted memory to reset for next cmd
 * @tokens: array of strings needing free()
 * ...: list of variables to free
 * NOTE: must be called with a final paramater VOID
 */
void resetAll(char **tokens, ...)
{
	va_list vars;
	char *free_me;

	if (tokens != NULL)
		free(tokens);
	va_start(vars, tokens);
	free_me = va_arg(vars, char *);

	while (free_me != NULL)
	{
		if (free_me)
			free(free_me);
		free_me = NULL;
		free_me = va_arg(vars, char *);
	}
	va_end(vars);
}

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
	// if (input != NULL)
	// {
	// 	free(input);
	// }
	// if (tokens != NULL)
	// {
	// 	free(tokens);
	// }
	exit(exit_code);
}
