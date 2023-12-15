#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024

extern char **environ;

/**
 * free_token_array - a
 * @tokens:  b
 *
 */
void free_tokens_array(char **tokens)
{
	int i = 0;

	while (tokens[i] != NULL)
	{
		free(tokens[i]);
		i++;
	}
	free(tokens);
}

/**
 * main - Simple shell
 *
 * Return: Always 0.
 */
int main(void)
{
	char *command = NULL;
	int num_read;
	size_t n = 0;
	char **argv = NULL;
	pid_t child_pid;
	int status;
	int j;

	while (1)
	{
		if (isatty(0))
		{
			write(STDOUT_FILENO, "($) ", 4);
			fflush(stdout);
		}

		num_read = getline(&command, &n, stdin);
		if (num_read == -1)
		{
			if (isatty(0))
			write(STDOUT_FILENO, "\n", 1);
			free(command);
			return (EXIT_SUCCESS);
		}

		argv = tokenize(command, " \n\t");

		if (argv[0] != NULL)
		{
			if (access(argv[0], F_OK | X_OK) == -1)
			{
				write(2, "Error: execve failed for command ", 33);
				for (j = 0; argv[j] != NULL; ++j)
				{
					write(2, argv[j], strlen(argv[j]));
					write(2, " ", 1);
				}
				write(2, "\n", 1);
				free_tokens_array(argv);
				continue;
			}

			child_pid = fork();
			if (child_pid == -1)
			{
				free_tokens_array(argv);
				exit(EXIT_FAILURE);
			}
			if (child_pid == 0)
			{
				if (execve(argv[0], argv, environ) == -1)
					exit(EXIT_FAILURE);
			}
			else
			{
				free_tokens_array(argv);
				wait(&status);
			}
		}
		else
		{
			free_tokens_array(argv);
			if (!isatty(0))
				return (0);
		}
	}

	free(command);
	return (0);
}
