#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <limits.h>
#include <iomanip>
#include <stdio.h>
#include <string.h>

using namespace std;

int main(int argc, char *argv[])
{
    ifstream workfile;
    workfile.open(argv[1]);

    int num, time_q1, time_q2;
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
    workfile >> time_q1 >> time_q2;

    queue<int > first_layer;
    queue<int > second_layer;
    vector<int > third_layer;

    int time = 0, index = -1, now_layer = -1, tq = -1;
    int flag = num;
    //now_layer:0,1,2
    while(flag)
    {
        for(int i = 0; i<num; i++)
        {
            if(time == arrival[i])
            {
                run[i] = burst[i];
                first_layer.push(i);
            }
        }

        if(index == -1) {
            if(!first_layer.empty())
            {
                index = first_layer.front();
                now_layer = 0;
                tq = time_q1;
            }
            else if(!second_layer.empty())
            {
                index = second_layer.front();
                now_layer = 1;
                tq = time_q2;
            }
            else if(!third_layer.empty())
            {
                int temp, min = INT_MAX;
                vector<int >::iterator it;
                for(it = third_layer.begin(); it != third_layer.end(); it++)
                {
                    if(run[*it] < min && run[*it] != 0)
                    {
                        min = run[*it];
                        temp = *it;
                    }
                }
                index = temp;
                now_layer = 2;
                //SJF
            }
        }

        int id = index; //id:now using CPU

        if(id != -1 && (now_layer==0 || now_layer==1)) // RR
        {
            run[id]--;
            tq--;

            if(run[id] == 0)
            {
                turnaround[id] = time - arrival[id] + 1;
                if(now_layer == 0) first_layer.pop();
                else if(now_layer==1) second_layer.pop();
                index = now_layer = tq = -1;   //reset
                flag--;
            }
            else if(tq == 0)
            { //unfinish but tq over
                if(now_layer == 0)
                {
                    first_layer.pop();
                    second_layer.push(id);
                }
                else if(now_layer == 1)
                {
                    second_layer.pop();
                    third_layer.push_back(id);
                }
                index = now_layer = tq = -1;   //reset
            }
        }

        if(id != -1 && now_layer == 2)  //SJF
        {
            run[id]--;

            if(run[id] == 0)
            {
                flag--;
                turnaround[id] = time - arrival[id] + 1;
                vector<int >::iterator it;
                for(it = third_layer.begin(); it != third_layer.end(); it++)
                {
                    if(*it == id)
                    {
                        third_layer.erase(it);
                        break;
                    }
                }
            }
            index = now_layer = tq = -1;   //reset
        }

        for(int i = 0; i<num; i++)
        {
            if(id != i && run[i] > 0) wait[i]++;
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

    ofstream myfile ("ans4.txt");
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



