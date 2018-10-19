#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define TOKEN_SEP " \t\r\n\a"
#define MAX_LINE 80

int main(void)
{
    int should_run = 1; //flag to determine when to exit program
    int wait;
    while(should_run){
        if(wait == 0) printf("osh>");
        fflush(stdout);
        wait = 0;
        char *arg[MAX_LINE/2+1] = {NULL}; //command line arguments
        char inputBuffer[MAX_LINE] = {0};
        read(STDIN_FILENO, inputBuffer, MAX_LINE);
        int num = 0;
        int bg = 1;
        char *cmd;
        int pid = -1;
        cmd = strtok(inputBuffer, TOKEN_SEP);
        while(cmd != NULL)
        {
            arg[num] = cmd;
            num++;
            cmd = strtok(NULL, TOKEN_SEP);
        }
        if(strcmp(arg[0], "exit") == 0)
        {
            should_run = 0;
        }
        if(arg[num-1][0] == '&')
        {
            arg[num-1] = NULL;
            bg = 0;
            wait = 1;
        }
        pid = fork();
        if(pid == 0)
        {
            execvp(arg[0], arg);
        }
        else
        {
            int status;
            if(bg)
                waitpid(pid, &status, 0);
        }
    }    
    return 0;
}

