#include <stdlib.h>
#include <string.h>

/**
 * tokenize - a
 * @command: b
 * @delim: c
 *
 * Return: 0
 */
char **tokenize(char *command, char *delim)
{
	int i;
	int num_tokens = 0;
	char **argv;
	char *cpyCommand = strdup(command);
	char *cpyCommand2 = strdup(command);
	char *token = strtok(cpyCommand, delim);

	while (token != NULL)
	{
		num_tokens++;
		token = strtok(NULL, delim);
	}

	argv = malloc((num_tokens + 1) * sizeof(char *));
	token = strtok(cpyCommand2, delim);

	i = 0;
	while (token != NULL)
	{
		argv[i] = malloc((strlen(token) + 1) * sizeof(char));
		strcpy(argv[i], token);
		token = strtok(NULL, delim);
		i++;
	}

	argv[i] = NULL;
	free(cpyCommand);
	free(cpyCommand2);
	cpyCommand = NULL;
	cpyCommand2 = NULL;
	return (argv);
}
