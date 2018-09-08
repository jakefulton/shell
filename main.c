//#include "parse.h"
#include "exec.c"

char *readline()
{
    //Read input until user presses enter
    //Exit program if EOF
    int c;
    char *line = malloc( sizeof(char) * 512 );
    int index = 0;
    do
    {
        c = getchar();
        if( c == EOF )
            exit(0);
        line[index] = c;
        index++;
    } while ( c != '\n' );
    line[index - 1] = '\0'; //Null terminate & get rid of '\n'
    return line;
}

void scripting(full_command *cmd, background *bg, char *file)
{
    FILE *fp;
    char *line = NULL;
    char *copy;
    size_t bufsize = 0;
    ssize_t read;
    fp = fopen(file, "r");
    if (fp == NULL)
        perror("Could not open file");
    while((read = getline(&line, &bufsize, fp)) != -1)
    {
        line[strlen(line)-1] = '\0';
        parse(line, cmd);
        exec(cmd, bg, line);
        reset(cmd);
    }
    fclose(fp);
    exit(0);
}

int main(int argc, char *argv[], char *envp[])
{
    char *line;
    full_command job;
    background bg;

    bg.bg_count = 0;
    job.cmd_index = 0;
    job.infile = NULL;
    job.outfile = NULL;

    if (argc == 2)
        scripting(&job, &bg, argv[1]);

    while(1)
    {
        printf("? ");
        line = readline();
        parse(line, &job);
        //print_command(job);
        exec(&job, &bg, line);
        reset(&job);
    }
    free(line);
}
