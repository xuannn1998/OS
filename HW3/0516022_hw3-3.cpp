#include <iostream>
#include <fstream>
#include <queue>
#include <stdio.h>
using namespace std;

int main(int argc, char *argv[])
{
    ifstream workfile;
    workfile.open(argv[1]);
    int num, time_quantum;
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
    workfile >> time_quantum;
    
    int time = 0;
    int flag = num;
    int done = 1; // process is finished or not
    int now;
    int newtime = 0;
    queue<int> q;

    while(flag)
    {
        //cout << "flag : " << flag << endl;
        while(time <= newtime)
        {
            for(int i = 0; i<num; i++)
            {
                if(time == arrival[i])
                {
                    run[i] = burst[i];
                    q.push(i);
                }
            }
            time++;
        }
        //cout << "time : " << newtime << endl;
        if(done == 0) q.push(now);

        now = q.front();
        //cout << " process: "<< now+1 << endl;
        if(run[now] > 0)
        {
            q.pop();
            if(run[now] > time_quantum)
            {
                run[now] -= time_quantum;
                newtime += time_quantum;
                done = 0;
            }
            else //run[now] <= time_quantum
            {
                newtime += run[now];
                wait[now] = newtime - burst[now] - arrival[now];
                done = 1;
                run[now] = 0;
                flag--;
            }
        }
    }

    for(int i = 0; i<num; i++) turnaround[i] = burst[i] + wait[i];

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

    ofstream myfile ("ans3.txt");
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
