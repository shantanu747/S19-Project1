#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <iomanip>
#include "process.h"
#include "functions.h"
using namespace std;

void FCFS(vector<Process> all_p, int n, int switch_time)
{
    int t_cs = 6;
    unsigned int total_p = all_p.size();

    int time = 0;
    int burst_end = 0;
    int burst_start = 0;
    int currentProcess_index = 0;
    string currentProcess;

    bool cpu_in_use = false;
    vector<Process> readyQ;
    vector<Process> serviceQ;

    int context_switches = 0;
    float total_turn_around_time = 0;
    float total_burst_times = 0;
    float total_wait_time = 0;

    cout << "time " << time << "ms: Simulator started for FCFS ";
    printQ(readyQ);

    while (serviceQ.size() < total_p)
    {
        for (unsigned int i=0; i<total_p; i++)
        {
            if (all_p[i].get_arrival_time() == time
                && all_p[i].get_blocked_until() == 0)
            {
                //print out that a process arrived
                cout << "time " << time << "ms: Process " << all_p[i].get_id() << " arrived and added to ready queue ";

                readyQ.push_back(all_p[i]);
                printQ(readyQ);
            }
            else if (all_p[i].get_blocked_until() == time && all_p[i].get_arrival_time() < time)
            {
                readyQ.push_back(all_p[i]);
                cout << "time " << time << "ms: Process " << all_p[i].get_id() << " completed I/O; added to ready queue ";
                printQ(readyQ);
            }
        }

        // service the process
        if (readyQ.size() > 0
            && ((!cpu_in_use && time >= burst_end + 6) || time == t_cs*0.5)
            && time >= readyQ[0].get_blocked_until() + 3){

            currentProcess = readyQ[0].get_id();
            context_switches++;

            for (unsigned int i=0; i<all_p.size(); i++){
                if (currentProcess == all_p[i].get_id()){
                    currentProcess_index = i;
                }
            }

            cout << "time " << time << "ms: Process " << readyQ[0].get_id() << " started using the CPU ";

            burst_start = time;
            burst_end = time + readyQ[0].get_burst_time();

            readyQ.erase(readyQ.begin());
            printQ(readyQ);

            cpu_in_use = true;
        }
        else if (time == burst_end && burst_end != 0)
        {
            if (!all_p[currentProcess_index].get_serviced()){
                total_turn_around_time += (burst_end - all_p[currentProcess_index].get_arrival_time());
            }
            else{
                total_turn_around_time += (burst_end - all_p[currentProcess_index].get_blocked_until());
            }
            total_burst_times += (burst_end - burst_start);

            cpu_in_use = false;
            all_p[currentProcess_index].set_serviced();
            //decrease the burst count
            //if burst count > 0 throw back in q and update blocked until
            //else put it in the serviced q and remove from all_p
            if (all_p[currentProcess_index].get_burst_count() > 1){
                string b ;

                cout << "time " << time << "ms: Process " << all_p[currentProcess_index].get_id() << " completed a CPU burst; ";
                all_p[currentProcess_index].decrease_bursts();

                all_p[currentProcess_index].set_blocked_until(time +
                                                               all_p[currentProcess_index].get_io_time()+3);
                if (all_p[currentProcess_index].get_burst_count() == 1){
                    b = "burst";
                }else {b = "bursts";}
                cout << all_p[currentProcess_index].get_burst_count() << " " << b <<" to go ";
                printQ(readyQ);
                cout << "time " << time << "ms: Process " << all_p[currentProcess_index].get_id()<< " switching out of CPU; will block on I/O until time ";
                cout << all_p[currentProcess_index].get_blocked_until() << "ms ";
                printQ(readyQ);

            }
            else
            {
                all_p[currentProcess_index].decrease_bursts();
                all_p[currentProcess_index].set_blocked_until(time +
                                                               all_p[currentProcess_index].get_io_time());

                cout << "time " << time << "ms: Process " << all_p[currentProcess_index].get_id() << " terminated ";
                printQ(readyQ);

                serviceQ.push_back(all_p[currentProcess_index]);
                all_p.erase(all_p.begin() + currentProcess_index);
            }
        }

        if (readyQ.size() > 0 && cpu_in_use){
            total_wait_time += readyQ.size();
        }
        else if (readyQ.size() && !cpu_in_use){
            total_wait_time += (readyQ.size()-1);
        }
        //go again!
        time++;
    }
    time += (t_cs/2)-1;
    cout << "time " << time << "ms: Simulator ended for FCFS" << endl;

    float avg_tat = total_turn_around_time / float(context_switches);

    float avg_bt = total_burst_times / float(context_switches);

    float avg_wt = total_wait_time / float(context_switches);

    file_writer(avg_bt, avg_wt, avg_tat, context_switches, 0, fname, "FCFS");
}
