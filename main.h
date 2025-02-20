#ifndef MAIN_H
#define MAIN_H

/* ↓ LIBRARIES ↓ */
#include <ctype.h>	   /* For isspace() in trim_whitespace() */
#include <errno.h>	   /* For errno */
#include <fcntl.h>	   /* For open(), fcntl() - if you use them */
#include <linux/limits.h>	   /* For PATH_MAX */
#include <stdbool.h>   /* For bool type */
#include <stdarg.h>	   /* For va_list, va_start, va_arg, va_end */
#include <stdio.h>	   /* For printf, fprintf, perror, getline etc */
#include <stdlib.h>	   /* For malloc, free, realloc, exit, getenv */
#include <string.h>	   /* kinda useless  -  BIG TRUE */
#include <sys/types.h> /* For pid_t, size_t */
#include <sys/wait.h>  /* For waitpid, WIFEXITED, WEXITSTATUS */
#include <unistd.h>    /* For isatty, fork, execve, chdir, getcwd, etc.. */
#include <sys/stat.h>  /* For stat and S_ISDIR */
#include "colors.h"

/* ↓ STRUCTS AND MISC ↓ */

/**
 * struct path_s - Singly linked list for PATH environment variable
 * @directory: String of directory
 * @next: Next location directory
 *
 * Description: Singly linked list node structure
 */
typedef struct path_s
{
	char *directory;
	struct path_s *next;
} path_t;

/**
 * typedef enum SeparatorType - Enumeration for command separators
 * @SEP_NONE: No separator.
 * @SEP_SEMICOLON: Semicolon separator (;).
 * @SEP_AND: Logical AND separator (&&).
 * @SEP_OR: Logical OR separator (||).
 */
typedef enum
{
	SEP_NONE,
	SEP_SEMICOLON,
	SEP_AND,
	SEP_OR
} SeparatorType;

extern char **environ;		 /* The environment variables */
extern char **saved_environ;
extern char *input;
extern char **tokens;

/* ↓ FUNCTION PROTOTYPES ↓ */

/* --- Main Shell Loop and Control --- */
void shellLoop(int isAtty, char *argv[]);
void executeIfValid(int isAtty, char *const *argv, char **tokens, char *input);
void safeExit(int exit_code);
void printPrompt(int isAtty, char *user, char *hostname, char *path);

/* --- Command Parsing --- */
char **parse_command(char *command);
int split_command_line_on_pipe(char *input, char ***commands, int *num_commands);
char *trim_whitespace(char *str);

/* --- Command Execution --- */
int execute_command(const char *commandPath, char **arguments);
void execute_pipe_command(char **commands, int num_commands);
void execute_logical_commands(char *line);
void execute_commands_separated_by_semicolon(char *line);

/* --- Built-in Command Handlers --- */
int customCmd(char **tokens, int interactive, char *input);
int ifCmdCd(char **tokens);
int ifCmdEnv(char **tokens);
int ifCmdExit(char **tokens, int interactive, char *input);
int ifCmdSelfDestruct(char **tokens);
int ifCmdSetEnv(char **tokens);
int ifCmdUnsetEnv(char **tokens);
void selfDestruct(int countdown);
void freeIfCmdCd(char *previous_cwd, char *home, char *pwd);

/* --- Environment Variable Handling --- */
char *_getenv(const char *name);
int _setenv(const char *name, const char *value, int overwrite);
int _unsetenv(const char *name);
void initialize_environ(void);
path_t *buildListPath(void);
char *findPath(char *name);
void destroyListPath(path_t *h);
char *getUser(void);
char *getHostname(void);
char* _strstr(char *sentence, char *word);
int RightDirect(char *line);
int DoubleRightDirect(char *line);
int LeftDirect(char *line);
int DoubleLeftDirect(char *line);
int StreamDirect(char **tokens);
int ifCmdEcho(char **tokens);
size_t _strcspn(const char *str1, const char *str2);

/* --- Utility Functions --- */
int isNumber(char *str);
int _atoi_safe(const char *s);
void resetAll(char **tokens, ...);
void freeIfCmdCd(char *previous_cwd, char *home, char *pwd);
int is_directory(char *fp);
int isCommand(const char *fp);

/* --- Custom String Functions (Keep these!) --- */
char *_strtok_r(char *str, const char *delim, char **saveptr);
char *_strcat(char *dest, const char *src);
char *_strchr(const char *s, char c);
int _strlen(const char *s);
int _strcmp(const char *s1, const char *s2);
char *_strcpy(char *dest, const char *src);
char *_strdup(const char *str);
int _strncmp(const char *s1, const char *s2, int n);
void *_realloc_array(char **ptr, unsigned int new_size);
int _build_path(const char *cwd, const char *rel_path, char *abs_path);

#endif /* MAIN_H */
