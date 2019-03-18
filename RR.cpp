#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <deque>
#include <iomanip>
#include "process.h"

using namespace std;

void RR(vector<Process> p, int n, int switch_time, int tslice, string behavior)
{
    deque<Process> all_p;
    for(int i = 0; i < p.size(); i++) //convert to deque for push_front capabilities
    {
      all_p.push_back(p[i]);
    }

    int t_cs = switch_time; // takes this much time to make a context switch
    int t_slice = tslice; // time slice for RR algorithm
    bool cpu_in_use = false; // only set to true while process in CPU
    //bool cpu_blocked = false; //block during context switch
    string rradd = behavior; //BEGINNING OR END
    unsigned int time = 0; // overall timer for simulation
    int startNextProcess = -1; // By default first process should not start before this time
    int burst_end = -1; // acts as marker for when next context switch/preemption should occur

    deque<Process> readyQ;
    deque<Process> serviceQ;

    Process currentProcess; // holds ID of current process being serviced

    // Begin algorithm simulation
    cout << "time " << time << "ms: Simulator started for RR ";
    printQ(readyQ);

    // while not all processes have been serviced
    while (serviceQ.size() != all_p.size())
    {
        //before adding/removing any process from CPU, make sure we have
        //the proper readyQ needed
        for (int i = 0; i < all_p.size(); i++)
        {
            // Check if any processes arrive at this time
            if (all_p[i].get_arrival_time() == time )
            {
                cout << "time " << time << "ms: Process " << all_p[i].getID() << " arrived and added to ready queue ";
                if(rradd == "BEGINNING")
                {
                  readyQ.push_front(all_p[i]);
                }
                else
                {
                  readyQ.push_back(all_p[i]);
                }
                printQ(readyQ);
                if(readyQ.size()==1)
                {
                  //first process arrived at whatever time it did
                  //set startNextProcess to arrival time plus t_cs/2
                  startNextProcess = readyQ[0].getArrivalTime() + (t_cs/2);
                }
            }
            // Check if any processes come back from I/O at this time
            else if(all_p[i].getBlockedUntil() == time)
            {
              cout << "time " << time << "ms: Process " << all_p[i].getID() << " finished I/O and added to ready queue ";
              if(rradd == "BEGINNING")
              {
                readyQ.push_front(all_p[i]);
              }
              else
              {
                readyQ.push_back(all_p[i]);
              }
              printQ(readyQ);
            }
        }

        //CPU currently in use but there is an event that needs handling
        if(cpu_in_use && time == burst_end)
        {
          //no context switch required
          if(readyQ.size() == 0)
          {
            //CPU burst finished
            if(currentProcess.getRemainingTimeInBurst() == 0)
            {

            }
            //Remaining time might be less than t_slice
            else if(currentProcess.getRemainingTimeInBurst() <= t_slice)
            {
              burst_end = time + currentProcess.getRemainingTimeInBurst();
            }
            //extend by t_slice with no preemption
            else
            {
              burst_end = time + t_slice;
              int difference
            }
          }
          //context switch is required
          else
          {
            if(currentProcess.getRemainingTimeInBurst()==0)//CPU burst finished
            {
              currentProcess.decreaseCPUBursts();
              if(currentProcess.getNumBursts()==0 && currentProcess.getIOBursts()==0)
              {
                //process just finished CPU burst, has no more CPU or IO bursts remaining
                //process is serviced
                currentProcess.setServiced();
                serviceQ.push_back(currentProcess);
                startNextProcess = time + t_cs;
                cpu_in_use = false;
              }
            }
            //current process needs more CPU time before IO
            else
            {
              currentProcess.addContextSwitch();
              currentProcess.addPreemptedCount();
              if(rradd == "BEGINNING")
              {
                readyQ.push_front(currentProcess);
              }
              else
              {
                readyQ.push_back(currentProcess)
              }
              cpu_in_use = false;
              startNextProcess = time + t_cs;
            }
          }
          //if timeslice still not expired no need to do anything else
          //let current CPU burst continue
        }

        //CPU not in use, it's time to load in a new process, and readyQ is not empty
        else if(cpu_in_use == false && time == startNextProcess && readyQ.size() != 0)
        {
          currentProcess = readyQ[0];
          readyQ.erase(readyQ.begin());
          //burst will finish before or at the same time that timeslice expires
          if(currentProcess.getRemainingTimeInBurst() <= t_slice)
          {
            burst_end = time + currentProcess.getRemainingTimeInBurst();
            currentProcess.setRemainingTimeInBurst(0); //this way we know to send process to IO at burst end
          }
          //timeslice will expire first, setRemainingTimeInBurst used to save the difference
          else
          {
            burst_end = time + t_slice;
            int difference = currentProcess.getRemainingTimeInBurst() - t_slice;
            currentProcess.setRemainingTimeInBurst(difference);
          }
          cpu_in_use = true;
        }
        time ++ ;
    }

    // calculations for avg algorithm stats
    float total_turn_around_time = 0;
    float total_burst_times = 0;
    float total_wait_time = 0;

    int preemptions = 0;
    int context_switches = 0;

    float avg_tat = 0.0; // average turn around time
    float avg_bt = 0.0; // average burst time
    float avg_wt = 0.0; // average wait time

    for(int i = 0; i < all_p.size(); i++)
    {

    }
    cout <<"time " << time+2 << "ms: Simulator ended for RR" ;

    avg_tat = total_turn_around_time / float(context_switches);
    avg_bt = total_burst_times / float(context_switches);
    avg_wt = total_wait_time / float(context_switches);
    file_writer(avg_bt, avg_wt, avg_tat, context_switches, preemptions, fname, "RR");

}
