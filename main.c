#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

/**
 * main - Simple shell
 *
 * Return: Always 0
 */
int main(void)
{
	char *input = NULL;
	size_t input_size = 0;
	char *newline;
	char *args[MAX_INPUT_SIZE];
	int f_i;

	while (1)
	{
		if (isatty(0))
		{
			write(STDOUT_FILENO, "#cisfun$ ", 9);
			fflush(stdout);
		}

		if (getline(&input, &input_size, stdin) == -1)
		{
			if (isatty(0))
				write(STDOUT_FILENO, "\n", 1);
			/*perror("getline");*/
			/*exit(EXIT_FAILURE);*/
			return (1);
		}

		/* Remove trailing newline */
		newline = strchr(input, '\n');

		if (newline != NULL)
		{
			*newline = '\0';
		}

		/* Ignore comments (lines starting with '#') */

		/* Tokenize the input */
		custom_tokenize(input, args);

		if (args[0] != NULL)
		{
			execute_command(args);
		}

		/* Check for the exit built-in command */

		/* Check for the env built-in command */


		/* Free memory for tokens */
		for (f_i = 0; args[f_i] != NULL; f_i++)
		{
			free(args[f_i]);
		}
	}

	free(input);

	return (0);
}

/**
 * custom_tokenize - splits string into array
 *
 * @input: string to tokenize
 * @args: tokenized array
 *
 * Return: Always 0.
 */
void custom_tokenize(char *input, char *args[])
{
	int arg_count = 0;
	char *token = strtok(input, " \n");

	while (token != NULL && arg_count < MAX_INPUT_SIZE - 1)
	{
		args[arg_count] = strdup(token);
		if (args[arg_count] == NULL)
		{
			perror("strdup");
			exit(EXIT_FAILURE);
		}
		arg_count++;
		token = strtok(NULL, " \n");
	}

	/* Null-terminate the args array */
	args[arg_count] = NULL;
}

/**
 * execute_command - determines the command to execute
 *
 * @args: args array
 *
 * Return: Always 0.
 */
void execute_command(char *args[])
{
	if (args[0][0] == '/' || args[0][0] == '.')
	{
		execute_directly(args[0], args);
	}
	else
	{
		execute_in_path(args[0], args);
	}
}

/**
 * execute_directly - Run if command is full path
 *
 * @command: full path Command to run
 * @args: array of commands and probably flags
 *
 * Return: Always 0.
 */
void execute_directly(char *command, char *args[])
{
	execute_with_fork(command, args);
}

/**
 * execute_with_fork - excutes the command
 *
 * @command: command to execute
 * @args: array of commands and probably flags
 *
 * Return: Always 0.
 */
void execute_with_fork(char *command, char *args[])
{
	int status;
	pid_t pid = fork();

	if (pid < 0)
	{
		perror(command);
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		/* Child process */
		if (execve(command, args, environ) == -1)
		{
			/*fprintf(stderr, "%s: %d: %s: not found\n", command, getpid(), args[0]);*/
			write(STDERR_FILENO, "./shell: command not found\n", 28);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		/* Parent process */
		if (wait(&status) == -1)
		{
			perror(command);
			exit(EXIT_FAILURE);
		}
	}
}

/**
 * execute_in_path - For builtins append to array of PATH values
 *
 * @command: command to execute
 * @args: array of commands and probably flags
 *
 * Return: Always 0.
 */
int execute_in_path(char *command, char *args[])
{
	char *path_env = getenv("PATH");
	char *path_copy;
	char *token;

	if (path_env == NULL)
	{
		write(STDERR_FILENO, "./shell: No such file or directory\n", 36);
		exit(EXIT_FAILURE);
	}

	path_copy = strdup(path_env);

	if (path_copy == NULL)
	{
		perror(command);
		return (2);
		/*exit(EXIT_FAILURE);*/
	}

	token = strtok(path_copy, ":");

	while (token != NULL)
	{
		char full_path[MAX_INPUT_SIZE];

		/*snprintf(full_path, sizeof(full_path), "%s/%s", token, command);*/
		/* Build full path */
		strcpy(full_path, token);
		strcat(full_path, "/");
		strcat(full_path, command);

		if (access(full_path, X_OK) == 0)
		{
			execute_with_fork(full_path, args);
			free(path_copy);
			return (0);
		}

		token = strtok(NULL, ":");
	}

	/* command not found */
	write(STDERR_FILENO, "./shell: No such file or directory\n", 36);
	/*fprintf(stderr, "%s: %s: not found\n", command, args[0]);*/
	free(path_copy);
	return (3);
}
