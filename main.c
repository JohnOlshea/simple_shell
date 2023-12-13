#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

#define MAX_INPUT_SIZE 1024

/**
 * Set environment variable
 */
void set_environment_variable(char *variable, char *value) {
    if (setenv(variable, value, 1) == -1) {
        perror("setenv");
        fprintf(stderr, "Failed to set environment variable %s\n", variable);
    }
}

/**
 * Unset environment variable
 */
void unset_environment_variable(char *variable) {
    if (unsetenv(variable) == -1) {
        perror("unsetenv");
        fprintf(stderr, "Failed to unset environment variable %s\n", variable);
    }
}

/**
 * Execute a command with fork, execve, and wait
 */
void execute_with_fork(char *command, char *args[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror(command);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        if (execve(command, args, environ) == -1) {
	    fprintf(stderr, "%s: %d: %s: not found\n", command, getpid(), args[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        int status;
        if (wait(&status) == -1) {
            perror(command);
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * Execute a command directly using its full path
 */
void execute_directly(char *command, char *args[]) {
    execute_with_fork(command, args);
}

/**
 * Search for the command in PATH and execute if found
 */
void execute_in_path(char *command, char *args[]) {
    char *path_env = getenv("PATH");

    if (path_env == NULL) {
	fprintf(stderr, "%s: %s: not found\n", command, args[0]);
        exit(EXIT_FAILURE);
    }

    char *path_copy = strdup(path_env);

    if (path_copy == NULL) {
        perror(command);
        exit(EXIT_FAILURE);
    }

    char *token = strtok(path_copy, ":");

    while (token != NULL) {
        char full_path[MAX_INPUT_SIZE];
        snprintf(full_path, sizeof(full_path), "%s/%s", token, command);

        if (access(full_path, X_OK) == 0) {
            execute_with_fork(full_path, args);
            free(path_copy);
            return;
        }

        token = strtok(NULL, ":");
    }

    // If the loop completes, the command was not found
    fprintf(stderr, "%s: %s: not found\n", command, args[0]);
    free(path_copy);
    exit(EXIT_FAILURE);
}

/**
 * Execute a command, either directly or by searching in PATH
 */
void execute_command(char *args[]) {
    if (args[0][0] == '/' || args[0][0] == '.') {
        execute_directly(args[0], args);
    } else {
        execute_in_path(args[0], args);
    }
}

/**
 * custom_tokenize - custom strtok
 *
 * @input: string to tokenize
 * @args: tokenized array
 *
 * Return: Always 0.
 */
void custom_tokenize(char *input, char *args[]) {
    int arg_count = 0;
    char *token = strtok(input, " \n");

    while (token != NULL && arg_count < MAX_INPUT_SIZE - 1) {
        args[arg_count] = strdup(token);
        if (args[arg_count] == NULL) {
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
 * print_environment - print environment variable
 *
 * Return: Always 0.
 */
void print_environment(void) {
    char **env_var = environ;
    while (*env_var != NULL) {
        printf("%s\n", *env_var);
        env_var++;
    }
}

/**
 * change_directory - change directory
 *
 * @directory: directory to change into
 *
 * Return: Always 0.
 */
void change_directory(char *directory) {
    if (directory == NULL || strcmp(directory, "-") == 0) {
        directory = getenv("HOME");

        if (directory == NULL) {
            fprintf(stderr, "Error: HOME environment variable not set\n");
            return;
        }
    }

    if (chdir(directory) == -1) {
        perror("chdir");
        fprintf(stderr, "Failed to change directory to %s\n", directory);
    }
}

/**
 * main - Simple shell
 *
 * Return: Always 0.
 */
int main(void) {
    char *input = NULL;
    size_t input_size = 0;

    while (1) {
	if (isatty(0))
	{
	    write(STDOUT_FILENO, "($)  ", 4);
            fflush(stdout);
	}

        if (getline(&input, &input_size, stdin) == -1) {
	    if (isatty(0))
		    write(STDOUT_FILENO, "\n", 1);
            perror("getline");
            exit(EXIT_FAILURE);
        }

        /* Remove trailing newline */
        char *newline = strchr(input, '\n');

        if (newline != NULL) {
            *newline = '\0';
        }

        /* Ignore comments (lines starting with '#') */
        if (input[0] == '#') {
            continue;
        }

        /* Tokenize the input */
        char *args[MAX_INPUT_SIZE];

        custom_tokenize(input, args);
        if (args[0] != NULL) {
            /* Check for the exit built-in command */
            if (strcmp(args[0], "exit") == 0) {
                /* Free allocated memory before exiting */
                free(input);

                /* Check for exit status argument */
                if (args[1] != NULL) {
                    int exit_status = atoi(args[1]);

                    exit(exit_status);
                } else {
                    exit(EXIT_SUCCESS);
                }
            }

            /* Check for the env built-in command */
            if (strcmp(args[0], "env") == 0) {
                print_environment();
            }
            /* Check for the setenv built-in command */
            else if (strcmp(args[0], "setenv") == 0) {
                if (args[1] != NULL && args[2] != NULL) {
                    set_environment_variable(args[1], args[2]);
                } else {
		    char error_message[] = "Usage: setenv VARIABLE VALUE\n";
                    write(STDERR_FILENO, error_message, sizeof(error_message) - 1);
                }
            }
            /* Check for the unsetenv built-in command */
            else if (strcmp(args[0], "unsetenv") == 0) {
                if (args[1] != NULL) {
                    unset_environment_variable(args[1]);
                } else {
		    char error_message[] = "Usage: unsetenv VARIABLE\n";

                    write(STDERR_FILENO, error_message, sizeof(error_message) - 1);
                }
            }
            /* Check for the cd built-in command */
            else if (strcmp(args[0], "cd") == 0) {
                change_directory(args[1]);
            } else {
                execute_command(args);
            }
        }

        /* Free memory for tokens */
        for (int i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
    }

    return (0);
}
