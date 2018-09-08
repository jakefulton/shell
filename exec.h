#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "parse.h"

void set_infile(full_command *cmd, int *in);
void set_ofile(full_command *cmd, int *out);
void set_child_io(full_command *cmd, int *in, int *out, int index, int pipes[]);
void check_background(background *bg, char *line);
void change_directory(full_command *cmd);
void exec(full_command *cmd, background *bg, char *line);

