#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int pid0 = getpid();
    int pid1 = -1, pid2 = -1, pid3 = -1;
    printf("Main process id : %d\n", pid0);
    
    pid1 = fork(); //fork1
    if(pid1 > 0) wait(NULL); //main
    else if(pid1 == 0) //child1
    {
        printf("Fork1. I'm the child %d, my parent is %d.\n", getpid(), getppid());
        pid2 = fork(); //fork2
        if(pid2 > 0) wait(NULL); //child1(now parent)
        else if(pid2 == 0) //child2
        {
            printf("Fork2. I'm the child %d, my parent is %d.\n", getpid(), getppid());
        }
    }
    
    pid3 = fork(); //fork3
    if(pid3 > 0) wait(NULL); //main, child1, child2(now parent)
    else if(pid3 == 0) //child3, child4, child5
    {
        printf("Fork3. I'm the child %d, my parent is %d.\n", getpid(), getppid());
        exit(0);
    }
    exit(0);
    return 0;
}
