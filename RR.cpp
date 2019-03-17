#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <dequeue>
#include <iomanip>
#include "process.h"
#include "functions.h"

using namespace std;

void RR(dequeue<Process> all_p, int n, int switch_time, int tslice, string behavior)
{
    int total_p = all_p.size(); // total processes received at the beginning of config
    int t_cs = switch_time; // takes this much time to make a context switch
    int t_slice = tslice; // time slice for RR algorithm
    bool cpu_in_use = false; // only set to true while process in CPU
    string rradd = behavior;
    int time = 0; // overall timer for simulation
    int preemptions = 0;
    int context_switches = 0;
    int start_next_process = t_cs/2; // By default first process should not start before this time
    int burst_start = 0;
    int burst_end = 0; // acts as marker for when next context switch/preemption should occur

    float total_turn_around_time = 0;
    float total_burst_times = 0;
    float total_wait_time = 0;

    float avg_tat = 0.0; // average turn around time
    float avg_bt = 0.0; // average burst time
    float avg_wt = 0.0; // average wait time

    dequeue<Process> readyQ;
    dequeue<Process> serviced_q;

    string curent_process; // holds ID of current process being serviced
    int current_process_index = 0;

    for(int i = 0; i < total_p; i++)
    {
        total_burst_times += (all_p[i].get_burst_count() * all_p[i].get_burst_time());
        all_p[i].set_time_needed();
        all_p[i].set_remaining_time_in_current_burst(all_p[i].get_burst_time());
    }

    // Begin algorithm simulation
    cout << "time " << time << "ms: Simulator started for RR ";
    printQ(readyQ);

    // while not all processes have been serviced
    while (serviced_q.size() < total_p)
    {
        // Check if any processes arrive at this time
        // If so: add to readyQ
        for (int i=0; i < total_p; i++)
        {
            if (all_p[i].get_arrival_time() == time )
            {
                cout << "time " << time << "ms: Process " << all_p[i].get_id() << " arrived and added to ready queue ";
                if(rradd == "BEGINNING")
                {
                  readyQ.insert(readyQ.begin(), all_p[i]);
                }
                else
                {
                  readyQ.push_back(all_p[i]);
                }
                printQ(readyQ);
            }
        }

        // check current conditions and service next process if everything checks out
        if  ((!cpu_in_use)
             && time == start_next_process
             && (readyQ.size() != 0))
        {
            curent_process = readyQ[0].get_id();
            for (int i =0; i< total_p ; i++ )
            {
                if (all_p[i].get_id() == curent_process){
                    current_process_index = i;
                }
            }

            cout << "time " << time << "ms: Process " << curent_process << " started using the CPU ";
            if (all_p[current_process_index].getRemainingTimeInBurst() != all_p[current_process_index].get_burst_time()){
                cout << "with " << all_p[current_process_index].getRemainingTimeInBurst() <<"ms remaining ";

            }
            cpu_in_use = true;
            burst_start = time  ;

            if (all_p[current_process_index].getRemainingTimeInBurst() <= t_slice)
                // process will end before t_slice expires
            {

                burst_end = burst_start + all_p[current_process_index].getRemainingTimeInBurst();
                all_p[current_process_index].set_remaining_time_in_current_burst(0);
            }
            else {
                burst_end = burst_start + t_slice ;

                int tmp = (all_p[current_process_index].getRemainingTimeInBurstgetRemainingTimeInBurst()) - t_slice;
                all_p[current_process_index].set_remaining_time_in_current_burst(tmp);
            }
            readyQ.erase(readyQ.begin());
            if (readyQ.size() != 0){
                cout << " ";
            }
            printQ(readyQ);
        }

        // Check if any processes finished an I/0 brust, return it to ready
        for (int i =0; i < all_p.size() ; i++ ){
            if (all_p[i].get_blocked_until() != 0 && all_p[i].get_blocked_until() == time && time != burst_end){
                if (all_p[i].get_burst_count() == 0 ) // process finish
                {
                   // cout << "time " << time << "ms: Process " << all_p[i].get_id() << " terminated ";
                   /* all_p[i].set_serviced();
                    int turnaround = time -all_p[i].get_arrival_time();
                    all_p[i].set_turn_around_time(turnaround);
                    serviced_q.push_back(all_p[i]);*/
                }
                else { // more bursts needed

                    cout << "time " << time << "ms: Process " << all_p[i].get_id() << " completed I/O; added to ready queue " ;
                    start_next_process = time + 3;
                    readyQ.push_back(all_p[i]);
                    all_p[i].set_remaining_time_in_current_burst(all_p[i].get_burst_time());
                    printQ(readyQ);
                }

            }
        }



        // burst_end reached
        if(time == burst_end && (burst_end != 0))
        {
            context_switches++;
            if(all_p[current_process_index].getRemainingTimeInBurst() == 0 && all_p[current_process_index].get_burst_count() != 0) // send to I/O and bring back to readyQ
            {
                int temp = 0;
                if (all_p[current_process_index].getRemainingTimeInBurst() <= t_slice && all_p[current_process_index].get_burst_count() <2)
                {
                    temp = time + all_p[current_process_index].get_io_time();
                }
                else
                {
                    temp = time + all_p[current_process_index].get_io_time()+3;
                }

                all_p[current_process_index].set_blocked_until(temp);
                cpu_in_use = false;
                start_next_process = time + t_cs*2;
                all_p[current_process_index].decrease_bursts();
                all_p[current_process_index].set_remaining_time_in_current_burst(all_p[current_process_index].get_burst_time());
                if (all_p[current_process_index].get_burst_count() == 0){
                    cout <<"time " << time << "ms: Process "<< curent_process << " terminated ";
                    all_p[current_process_index].set_serviced();
                    int turnaround = time -all_p[current_process_index].get_arrival_time();
                    all_p[current_process_index].set_turn_around_time(turnaround);
                    serviced_q.push_back(all_p[current_process_index]);

                }
                else
                {
                    string b;
                    if (all_p[current_process_index].get_burst_count() == 1)
                    {
                        b = "burst";
                    }else {b= "bursts";}
                    cout << "time " << time << "ms: Process "<< curent_process << " completed a CPU burst; "<< all_p[current_process_index].get_burst_count() << " " << b <<" to go " ;
                    printQ(readyQ) ;
                    cout <<  "time " <<time << "ms: Process " << all_p[current_process_index].get_id()<< " switching out of CPU; will block on I/O until time ";
                    cout << all_p[current_process_index].get_blocked_until() << "ms ";
                }
                preemptions++; // may or may not belong here


                printQ(readyQ);
            }
            else if (all_p[current_process_index].getRemainingTimeInBurst() == 0 && all_p[current_process_index].get_burst_count() == 0){
                cout << "time " << time << "ms: Process " << curent_process << " completed a CPU burst ";
                cpu_in_use = false;
                start_next_process = time + 2* t_cs;

            }
            else if (readyQ.size() == 0) // no context switch
            {
                cout << "time " << time << "ms: Time slice expired; no preemption because ready queue is empty ";
                printQ(readyQ);

                burst_start = time;

                if (all_p[current_process_index].getRemainingTimeInBurst() <= t_slice)
                {
                    burst_end =  time + all_p[current_process_index].getRemainingTimeInBurst();
                    all_p[current_process_index].set_remaining_time_in_current_burst(0);
                }
                else
                {

                    burst_end = time + t_slice;
                    all_p[current_process_index].set_remaining_time_in_current_burst(all_p[current_process_index].getRemainingTimeInBurst()-t_slice);
                }
            }
            else
            {

                cout << "time " << time << "ms: Time slice expired; process " << all_p[current_process_index].get_id() << " preempted with " << all_p[current_process_index].getRemainingTimeInBurst() << "ms to go ";
                start_next_process = time + t_cs*2; // do not let next process use CPU until this time
                cpu_in_use = false; // CPU is currently not in use anymore
                preemptions++;
                printQ(readyQ);
                readyQ.push_back(all_p[current_process_index]);
            }
        }
        time ++ ;


    }
    for(int i = 0; i < all_p.size(); i++)
    {
        total_turn_around_time += all_p[i].get_turn_time();
        total_wait_time += all_p[i].get_wait_time();
    }
    cout <<"time " << time+2 << "ms: Simulator ended for RR" ;

    avg_tat = total_turn_around_time / float(context_switches);
    avg_bt = total_burst_times / float(context_switches);
    avg_wt = total_wait_time / float(context_switches);
    file_writer(avg_bt, avg_wt, avg_tat, context_switches, preemptions, fname, "RR");













}
