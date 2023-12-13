#ifndef HEADER_H
#define HEADER_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>

#define MAX_INPUT_SIZE 1024

void tokenize_input(char *input, char *args[]);
void execute_command(char *args[]);

#endif
