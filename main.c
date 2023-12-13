#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
			write(STDOUT_FILENO, "($) ", 4);
			fflush(stdout);
		}

		if (getline(&input, &input_size, stdin) == -1)
		{
			if (isatty(0))
				write(STDOUT_FILENO, "\n", 1);
			perror("getline");
			exit(EXIT_FAILURE);
		}

		/* Remove trailing newline */
		newline = strchr(input, '\n');

		if (newline != NULL)
		{
			*newline = '\0';
		}

		/* Ignore comments (lines starting with '#') */

		/* Tokenize the input */

		/* Check for the exit built-in command */

		/* Check for the env built-in command */

		printf("Shelly");

		/* Free memory for tokens */
		for (f_i = 0; args[f_i] != NULL; f_i++)
		{
			free(args[f_i]);
		}
	}

	free(input);

	return (0);
}
