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
	printf("Segmentation fault\n"); /* fake seg fault */
	sleep(1);
	printf(CLR_RED_BOLD); /* sets the text color to red */
	printf("Shellf destruct mode activated.\n\n");

	if (countdown > 3)
		printf(CLR_DEFAULT); /* reset color so it's no still red bold */

	sleep(2); /* Use sleep() directly */

	while (countdown) /* prints countdown */
	{
		if (countdown == 3)
			printf(CLR_RED); /* sets the text color to red for last 3 seconds */

		printf("%d\n", countdown);
		countdown--;
		sleep(1);
	}

	printf("%s\nThe %sGates Of Shell%s have closed. Goodbye.\n%s",
		   CLR_YELLOW_BOLD, CLR_RED_BOLD, CLR_YELLOW_BOLD, CLR_DEFAULT);

	safeExit(EXIT_SUCCESS);
}

/**
 * isNumber - check for non-number characters in a string.
 * If any of the supplied numbers have non-number characters in them,
 * return 0.
 *
 * @number: string to be checked if it's all numbers
 *
 * Return: 1 if it's all numbers; 0 if not.
 */
int isNumber(char *number)
{
	int i;

	for (i = 0; i < _strlen(number); i++)
	{
		if (number[i] > '9' || number[i] < '0')
			return (0);
	}

	return (1);
}

/**
 * _atoi_safe - Converts a string to an integer (safe version).
 * @s: The string to convert.
 *
 * Return: The converted integer, or 0 if the string is invalid.
 */
int _atoi_safe(const char *s)
{
	int result = 0;
	int sign = 1;

	if (*s == '-')
	{
		sign = -1;
		s++;
	}

	while (*s)
	{
		if (*s >= '0' && *s <= '9')
		{
			result = result * 10 + (*s - '0');
			/* Basic overflow check (won't catch all cases, but it's a something) */
			if (result < 0)
			{
				return 0; /* Indicate error */
			}
		}
		else
		{
			return 0; /* Indicate error (invalid character) */
		}
		s++;
	}

	return result * sign;
}

/**
 * runCommand - runs execve on a command. Handles forking and errors.
 *
 * @commandPath: command to run, including path
 * @args: array of args for commandPath, including the command (without path)
 *
 *
 * Return: 0 on success, -1 on failure, errno on failure from child process.
 */
int runCommand(char *commandPath, char **args)
{
	int child_status, wexitstat;
	pid_t fork_rtn;

	if (commandPath == NULL)
	{
		return (127);
	}

	if (access(commandPath, F_OK) != 0) /* checks if cmd doesn't exist */
		return (127);

	fork_rtn = fork();	/* split process into 2 processes */
	if (fork_rtn == -1) /* Fork! It failed */
		return (-1);	/* indicate error */
	if (fork_rtn == 0)	/* child process */
	{
		if (execve(commandPath, args, environ) == -1) /*executes user-command*/
			safeExit(errno);						  /* indicate error */
	}
	else /* parent process; fork_rtn contains pid of child process */
	{
		if (waitpid(fork_rtn, &child_status, WUNTRACED) == -1)
		{
			return (-1); /* indicate error */
		}
		/* waits until child process terminates */
		if (WIFEXITED(child_status))
		{
			wexitstat = WEXITSTATUS(child_status);
			return (wexitstat);
		}
	}
	return (0); /* success */
}

void echor(const char *input, const char *file) 
{
	int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("Error opening file");
		return;
	}
	if (write(fd, input, strlen(input)) < 0)
	{
		perror("Error writing to file");
		close(fd);
		return;
	}
	if (write(fd, "\n", 1) < 0)
    {
        perror("Error writing newline");
        close(fd);
        return;
    }
	close(fd);
}

char* cat(const char *file)
{
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return NULL;
    }

    char *buffer = malloc(1024);
    if (!buffer) {
        perror("Memory allocation failed");
        close(fd);
        return NULL;
    }

    ssize_t bytesRead = read(fd, buffer, 1023);
    if (bytesRead == -1) {
        perror("Error reading file");
        free(buffer);
        close(fd);
        return NULL;
    }

    buffer[bytesRead] = '\0';
    close(fd);
    return buffer;
}

void echodr(const char *input, const char *file) 
{
	int fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		perror("Error opening file");
		return;
	}
	if (write(fd, input, strlen(input)) < 0)
	{
		perror("Error writing to file");
		close(fd);
		return;
	}
	if (write(fd, "\n", 1) < 0)
    {
        perror("Error writing newline");
        close(fd);
        return;
    }
	close(fd);
}

void rev(char *str, ssize_t len)
{
    ssize_t i, j;
    char temp;
    for (i = 0, j = len - 1; i < j; i++, j--)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

void echol(const char *file)
{
    int fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening file");
        return;
    }

    char buffer[1024];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    
    if (bytes_read < 0)
    {
        perror("Error reading file");
        close(fd);
        return;
    }

    buffer[bytes_read] = '\0';

    rev(buffer, bytes_read);
    printf("%s", buffer);

    close(fd);
}
