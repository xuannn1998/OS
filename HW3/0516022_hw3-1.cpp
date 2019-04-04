#include <iostream>
#include <fstream>
#include <limits.h>
#include <stdio.h>
using namespace std;

int main(int argc, char *argv[])
{
    ifstream workfile;
    workfile.open(argv[1]);
    int num;
    workfile >> num;
    int burst[num], arrival[num], wait[num], turnaround[num], run[num];
    float total_wait = 0, total_turnaround = 0, avg_wait = 0, avg_turnaround = 0;
    
    for(int i = 0; i<num; i++) workfile >> arrival[i];
    for(int i = 0; i<num; i++)
    {
        workfile >> burst[i];
        wait[i] = 0;
        turnaround[i] = 0;
        run[i] = -1;
    }
    
    int time = 0, change = 1;
    int flag = num;
    while(flag)
    {
        //cout << flag << endl;
        int min = INT_MAX, index;
        for(int i = 0; i<num; i++)
        {
            if(time == arrival[i]) run[i] = burst[i];
            //cout << run[i] <<" ";
        }
        //cout << endl;
        for(int i = 0; i<num; i++)
        {
            if(run[i] != -1 && run[i] < min && run[i] != 0 && change == 1)
            {
                min = run[i];
                index = i;
                //cout << index << endl;
            }
        }
        change = 0;
        if(run[index]>0) run[index]--;
        if(run[index] == 0)
        {
            turnaround[index] = time - arrival[index] + 1;
            flag--;
            change = 1;
        }
        for(int i = 0; i<num; i++)
        {
            if(index != i && run[i]>0) wait[i]++;
        }
        time++;
    }

    for(int i = 0; i<num; i++)
    {
        total_wait += wait[i];
        total_turnaround += turnaround[i];
    }

    avg_wait = total_wait/num;
    avg_turnaround = total_turnaround/num;

    char a_wait[100], a_turnaround[100];
    
    sprintf(a_wait, "%.5f", avg_wait);

    for(int i = strlen(a_wait)-1; a_wait[i] != '.'; i--)
    {
        if(a_wait[i] == '0')
        {
            a_wait[i] = ' ';
            if(a_wait[i-1] == '.') a_wait[i-1] = ' ';
        }
        else break;
    }
    sprintf(a_turnaround, "%.5f", avg_turnaround);

    for(int i = strlen(a_turnaround)-1; a_turnaround[i] != '.'; i--)
    {
        if(a_turnaround[i] == '0')
        {
            a_turnaround[i] = ' ';
            if(a_turnaround[i-1] == '.') a_turnaround[i-1] = ' ';
        }
        else break;
    }
    
    ofstream myfile ("ans1.txt");
    if (myfile.is_open())
    {
        for(int i = 0; i<num; i++)
        {
            myfile << wait[i] << " " << turnaround[i] << endl;
        }
        for(int i = 0; i<strlen(a_wait); i++)
        {
            myfile << a_wait[i] ;
        }
        myfile << endl;
        for(int i = 0; i<strlen(a_turnaround); i++)
        {
            myfile << a_turnaround[i] ;
        }
        myfile << endl;
        myfile.close();
    }
    
    return 0;
}
