#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMBER_OF_THREADS 11 /* Hint */
#define PUZZLE_SIZE 9

/* example puzzle */
int puzzle[PUZZLE_SIZE+1][PUZZLE_SIZE+1] = {
			{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0}
		};

/* data structure for passing data to threads */
typedef struct
{
	int thread_number;
	int x;
	int y;
} parameters;

/* print puzzle */ 
#define LINE "====================== \n"
#define COLLINE "\n___________________________________ \n"
#define NAME "||  SUM   CHECKER  || \n"
void print_grid(int grid[10][10])
{
    int i,j;
    printf(LINE);
    printf(NAME);
    printf(LINE);

	for (i = 1; i < 10; i++)
	{
		for (j = 1; j < 10; j++)
		{
	        printf("|%1d |",grid[i][j]);
		}
        printf(COLLINE);
	}
    printf("\n");
}
// read file to check sudoku
void SetPuzzle(char filename[]){
    FILE *file = fopen(filename,"r");
    int i,j,ch,num;
    for (i=0;i<=9;i++){
        for(j=0;j<=9;j++){
            while(((ch = getc(file)) != EOF)){
                if(ch == EOF) break;
                if(ch == ',') break;
                if(ch == '\n') break;
                ungetc(ch,file);
                fscanf(file,"%d",&num);
                if(num!=-1)
                    puzzle[i][j] = num;            
            } 
        }
    }
    print_grid(puzzle);
    return;
}

int puzzle_sum = 0;

void* subgrid_checker(void *in)
{
    int subgrid = (int*) in;
    int sum = 0;
    for(int i=(subgrid%3)*3+1; i<(subgrid%3)*3+4; i++){
        for(int j=(subgrid/3)*3+1; j<(subgrid/3)*3+4; j++){
            sum = sum + puzzle[i][j];
        }
    }
    //sleep(10);
    if(sum != puzzle_sum) pthread_exit((void*)0);
    else pthread_exit((void*)1);
}

void* row_checker()
{
    
    for(int i=1; i<PUZZLE_SIZE+1; i++){
        int sum = 0;
        for(int j=1; j<PUZZLE_SIZE+1; j++){
            sum = sum + puzzle[i][j];
        }
        if(i == 1) puzzle_sum = sum;
        if(sum != puzzle_sum) pthread_exit((void*)0);
    }
    //sleep(10);
    pthread_exit((void*)1);
}

void* column_checker()
{
    
    for(int i=1; i<PUZZLE_SIZE+1; i++){
        int sum = 0;
        for(int j=1; j<PUZZLE_SIZE+1; j++){
            sum = sum + puzzle[j][i];
        }
        if(sum != puzzle_sum) pthread_exit((void*)0);
    }
    //sleep(10);
    pthread_exit((void*)1);
}

int main(int argc, char* argv[])
{
	int rv = 1; // flag to check answer
    // input the sudoku file
    SetPuzzle("test.txt");
    //my code
    pthread_t row, column, sub[9]; // thread variable
    pthread_create(&row, NULL, row_checker, NULL); // create row thread
    pthread_create(&column, NULL, column_checker, NULL); // create column thread
    for (int i=0; i<PUZZLE_SIZE; i++) // create subgrid thread
    {
        pthread_create(&sub[i], NULL, subgrid_checker, (void*)i);
    }
    void *r;
    pthread_join(row, &r);
    rv = rv && (int*)r;
    //printf("%d\n", rv);
    pthread_join(column, &r);
    rv = rv && (int*)r;
    //printf("%d\n", rv);
    for (int i=0; i<PUZZLE_SIZE; i++)
    {
        pthread_join(sub[i], &r);
        rv = rv && (int*)r;
    }
    //printf("%d\n", rv);

	if (rv == 1)
		printf("Successful :) \n");
	else
		printf("Must check again! :( \n");

	return 0;
}
