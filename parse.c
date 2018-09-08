#include "parse.h"

void reset(full_command *cmd)
{
    //used to reset full_command struct each time
    //user enters a new command
    for ( int i = 0; i <= cmd->cmd_index; i++){
        for (int k = 0; k < cmd->command_list[i].argument_number; k++){
            cmd->command_list[i].argv[k] = '\0';
        }
        cmd->command_list[i].argument_number = '\0';
        cmd->command_list[i].command = '\0';
    }
    cmd->cmd_index = 0; 
    cmd->infile = NULL;
    cmd->outfile = NULL;
    cmd->background = 0;
}

void _quit(char * error_message, char *line)
{
    //exit program with error message
    printf("ERROR: %s\n", error_message);
    free(line);
    exit(1);
}

void _parse_redirection(char **token, full_command *job, char *line)
{
    //Modifies infile, outfile, and append values in full_command
    char *temp = *token;
    *token = strtok(NULL, " ");
    if ( !(*token) || strstr("<<>|&", *token) )
        _quit("Must specify file to redirect to/from", line);
    if( !strcmp("<", temp))
    {
    /*
        if (!job->infile)  //uncommment to prevent overwriting infile
                           //unsure which way to implement
    */
            job->infile = *token;
    }
    else if ( !strcmp(">", temp) )
    {
        job->outfile = *token;
        job->append = 0;
    }
    else if ( !strcmp(">>", temp) )
    {
        job->outfile = *token;
        job->append = 1;
    }
    else
        _quit("Invalid redirection token", line);
    *token = strtok(NULL, " ");

}

int check_for_comments(char **token)
{
    if ( strstr(*token, "#") != NULL ){
        *token = NULL;
        return 1;
    }
    return 0;
}

void _parse_regular(char **token, full_command *job)
{ 
    //fills out a new command for full_command
    command *cmd = &job->command_list[job->cmd_index];
    int i;
    for(i = 0; *token && !strstr("<>>|&", *token); i++)
    {
        if (check_for_comments(token))
            break;
        if (i == 0)
            cmd->command = *token;
        cmd->argv[cmd->argument_number] = *token;
        cmd->argument_number++;
        *token = strtok(NULL, " ");
    }
}

void _parse_background(char **token, full_command *job)
{
    job->background = 1;
    *token = strtok(NULL, " ");
}

void _parse_pipe(char **token, full_command *job, char *line)
{
    //prepares new pipeline
    //each pipe is has its own index into full_command's list of commands
    job->cmd_index++;
    job->command_list[job->cmd_index].argument_number = 0;
    *token = strtok(NULL, " ");
    if ( !(*token) || strstr("<<>|&", *token) )
        _quit("Must specify where to pipe to", line);
    _parse_regular(token, job);
}

void parse(char *line, full_command *job)
{
    //seperates line by spaces and stores
    //information into full_command struct
    char *token;
    token = strtok(line, " ");
    if ( !strcmp("|", token) )
        _quit("First token may not be a pipe", line);
    while(token)
    {
        if ( strstr("<>>", token) )
            _parse_redirection(&token, job, line);
        else if ( !strcmp("|", token) )
            _parse_pipe(&token, job, line);
        else if ( !strcmp("&", token) )
            _parse_background(&token, job);
        else 
            _parse_regular(&token, job);
    }
}

void print_command(full_command job)
{
    //Print # of commands
    //printf("%d: ", (job.command_list[0].argument_number == 0)? 0 : job.cmd_index + 1);
    //Print infile
    if (job.infile)
        printf("< %s ", job.infile);
    //Print commands sepearated by '|'
    for (int i = 0; i <= job.cmd_index; i++)
    {
        command *cmd = &job.command_list[i];
        for (int k = 0; k < cmd->argument_number; ++k)
        {
            if (!(i == 0 && k == 0))
                printf("%s ", cmd->argv[k]);
        }
        if (i != job.cmd_index)
            printf("| ");
    }
    //Print outfile
    if (job.outfile)
    {
        if (job.append)
            printf(">> %s", job.outfile);
        else
            printf("> %s", job.outfile);
    }
    printf("\n");
}
