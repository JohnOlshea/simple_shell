#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024

/**
 * _putchar - writes the character c to stdout
 * @c: The character to print
 *
 * Return: On success 1.
 * On error, -1 is returned, and errno is set appropriately.
 */
int _putchar(char c)
{
	return (write(1, &c, 1));
}

/**
 * print_string - writes string to stdout
 * @str: The string to print
 *
 * Return: On success 1.
 * On error, -1 is returned, and errno is set appropriately.
 */
int print_string(char *str)
{
	int i = 0, count = 0;

	while (str[i])
	{
		count += _putchar(str[i++]);
	}
	return (count);
}

/**
 * execute_command - x
 * @args: x
 *
 */
void execute_command(char *args[])
{
	int status;
	pid_t pid = fork();

	if (pid < 0)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		if (execve(args[0], args, NULL) == -1)
		{
			perror("execve");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		if (wait(&status) == -1)
			exit(EXIT_FAILURE);
	}
}

/**
 * tokenize_input - x
 * @input: x
 * @args: x
 *
 * Return: Always 0.
 */
void tokenize_input(char *input, char *args[])
{
	int arg_count = 0;
	char *token = strtok(input, " \n");

	while (token != NULL && arg_count < MAX_INPUT_SIZE - 1)
	{
		args[arg_count] = token;
		arg_count++;
		token = strtok(NULL, " \n");
	}

	args[arg_count] = NULL;
}

/**
 * main - Entry point
 *
 * Return: Always 0.
 */
int main(void)
{
	char *input = NULL;
	size_t input_size = 0;
	ssize_t chars_read;
	char *args[MAX_INPUT_SIZE];

	while (1)
	{
		if (isatty(0))
		print_string("$ ");

		chars_read = getline(&input, &input_size, stdin);
		if (chars_read < 0)
		{
			if (isatty(0))
			print_string("\n");
			free(input);
			exit(EXIT_FAILURE);
		}

		if (input[chars_read - 1] == '\n')
			input[chars_read - 1] = '\0';

		tokenize_input(input, args);

		if (args[0] != NULL)
			execute_command(args);
	}

	return (0);
}
