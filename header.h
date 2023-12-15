#ifndef HEADER_H
#define HEADER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

#define MAX_INPUT_SIZE 1024

/**
int _putchar_err(char c);
int print_string_err(char *str);
void set_environment_variable(char *variable, char *value);
void unset_environment_variable(char *variable);
int execute_with_fork(char *command, char *args[]);
int execute_in_path(char *command, char *args[]);
int execute_command(char *args[]);
int print_environment(void);
void change_directory(char *directory);
*/


char **tokenize(char *command, char *delim);

#endif
