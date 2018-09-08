#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 256

typedef struct
{
    //a single program along with its arguments
    char * command; //store program name here (ex ls)
    char * argv[MAX]; //store arguments to program here
    int argument_number; //keep track of how many arguments there are
    //using arg_number instead of setting "command[0] = '\0'" 
        //for easy resume after '|' or '<' parsing
} command;

typedef struct
{
    //stores input about a series of commands a user wants to execute
    command command_list[MAX]; //list of commands along with their arguments
    char *infile; //stores input redirection
    char *outfile; //stores output redirection;
    int append; //keep track of if appending or overwriting a file
    int background; //background jobs run asynchronously
    int cmd_index; //increment when a new pipe is found
} full_command;

typedef struct
{
    char *command; //what command is in background
    int pid; //pid of command in background
} bg_job;

typedef struct
{
    bg_job bg_jobs[MAX]; //array of background jobs;
    int bg_count; //number of background jobs;
} background;

void reset(full_command *cmd);
void _quit(char *error_message, char *line);
char *readline();
void parse_redirection(char **token, full_command *job, char *line);
void parse_regular(char **token, full_command *job);
void parse_pip(char **token, full_command *job, char *line);
void parse(char *line, full_command *job);
void print_command(full_command job);
int parse_command();
