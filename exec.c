#include "exec.h"

void set_infile(full_command *cmd, int *in)
{
    //Setup input file (only needs to be done once)
    if (cmd->infile){
        *in = open(cmd->infile, O_RDONLY);
    }
    else
        *in = 0;
    if (*in < 0)
        perror("Something went wrong setting ifile");
}

void set_ofile(full_command *cmd, int *out)
{
    if (cmd->outfile && !cmd->append){
        *out = open(cmd->outfile, O_WRONLY | O_CREAT, 0666);
    }
    else if (cmd->outfile && cmd->append){
        *out = open(cmd->outfile, O_APPEND | O_RDWR | O_CREAT, 0666);
    }
    else
        *out = 1;
    if (*out < 0)
        perror("Something went wrong setting ofile");
}

void set_child_io(full_command *cmd, int *in, int *out, int index, int pipes[])
{
    /** if first command, set infile */
    if( index == 0 )
        set_infile(cmd, in);
    /** If last command, set outfile */
    if (index == cmd->cmd_index)
        set_ofile(cmd, out);
    else
    {
        /** child output goes to pipe, unless last argument */
        *out = pipes[1];
        close(pipes[0]);
    }
}

void check_background(background *bg, char *line)
{
    int status, pid, i;
    //check if a background child has finished executing
    pid = waitpid(-1, &status, WNOHANG);
    //iterate through background jobs to find index of finished job
    for (i = 0; i < bg->bg_count; i++)
        if ( bg->bg_jobs[i].pid = pid)
        {
            //print out the finished job
            printf("[%d] Process done executing\n", i+1);
            bg->bg_count--;
            break;
        }
}

void change_directory(full_command *cmd)
{
    //Accepts one argument for which dir to change to
    if (cmd->command_list[0].argument_number != 2)
        perror("Expected 1 argument to cd");
    else
    {
        if (chdir(cmd->command_list[0].argv[1]) != 0)
            perror("cd error");
    }
        
}

void exec(full_command *cmd, background *bg, char *line)
{
    int in, out, pid, inbuf, outbuf;
    int pipes[2];
    
    //In and out initially set to stdin and stdout
    in = 0;
    out = 1;
    //Make a copy of stdin and stdout
    inbuf = dup(0);
    outbuf = dup(1);
    
    
    //Check to see if any background job has finished executing
    check_background(bg, line);
    
    char *first_command = cmd->command_list[0].command;
    //Change directory if first command is "cd"
    if ( !strcmp(first_command, "cd") )
    {
        change_directory(cmd);
        return;
    }
    if ( !strcmp(first_command, "exit") )
    {
        //free(line);
        exit(0);
    }
    if ( !strcmp(first_command, "print") )
    {
       print_command(*cmd);
        return;
    }

    //iterate over commands 
    int i;
    for (i=0; i <= cmd->cmd_index; i++)
    {
        pipe(pipes);
        //Create child process
        pid = fork();
        //Child work
        if (pid == 0)
        {
            //Set child input and output
            set_child_io(cmd, &in, &out, i, pipes);
            //Redirect child input and output
            if (dup2(out, 1) < 0) { perror("error copying out to stdout"); }
            if ( dup2(in, 0) < 0) { perror("error copying in to stdin"); }
            //close(in);
            //close(out);
            //Execute command
            execvp(cmd->command_list[i].command, cmd->command_list[i].argv);
            exit(0);
        }
        else if (pid < 0)
            perror("Error during fork");
        //Parent work
        else
        {
            //Next command in pipe will receive input from current command
            if (i != cmd->cmd_index)
                in = pipes[0];
            else
                close(pipes[0]);
            close(pipes[1]);

            int status;
            if (!cmd->background)
                //Wait for child processes to return
                waitpid(pid, &status, WUNTRACED);
            else
            {
                //Fill out background info, do not wait on child
                bg->bg_jobs[bg->bg_count].command = line;
                bg->bg_jobs[bg->bg_count].pid = pid;
                bg->bg_count++;
            }
        }
    }
        //Restore stdin and stdout
        dup2(inbuf, 0);
        dup2(outbuf, 1);
        close(inbuf);
        close(outbuf);
}
