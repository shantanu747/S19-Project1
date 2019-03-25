#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <deque>
#include <iomanip>
#include "process.h"
#include "functions.h"

using namespace std;

void RR(vector<Process> p, int n, int switch_time, int tslice, string behavior)
{
    deque<Process> all_p;
    for(int i = 0; i < p.size(); i++) //convert to deque for push_front capabilities
    {
      cout << "Process " << p[i].getID() << " [NEW] (arrival time " << p[i].getArrivalTime() << " ms) " << p[i].getNumBursts() << " CPU bursts" << endl;
      all_p.push_back(p[i]);
    }

    int t_cs = switch_time; // takes this much time to make a context switch
    int t_slice = tslice; // time slice for RR algorithm
    bool cpuInUse = false; // only set to true while process in CPU
    bool firstProcessArrived = false;
    string rradd = behavior; //BEGINNING OR END
    unsigned int time = 0; // overall timer for simulation
    int startNextProcess = -1; // By default first process should not start before this time
    int decisionTime = -1; //due to push-front behavior, make decision at burstEnd + t_cs/2 for next process
    int burst_end = -1; // acts as marker for when next context switch/preemption should occur
    int ioTime = -1;

    Process currentProcess; // holds ID of current process being serviced
    Process IOProcess; //hold ID of process to send to IO

    deque<Process> readyQ;
    deque<Process> serviceQ;

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
            if (all_p[i].getArrivalTime() == time )
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

                //only applies to the first process to arrive
                if(!firstProcessArrived)
                {
                  firstProcessArrived = true;
                  startNextProcess = time + (t_cs/2); //first process just arrived, start loading into CPU and start executing at t_cs/2
                }
                printQ(readyQ);
            }
            // Check if any processes come back from I/O at this time
            else if(all_p[i].getBlockedUntil() == time)
            {
              cout << "time " << time << "ms: Process " << all_p[i].getID() << " finished I/O and added to ready queue ";
              all_p[i].decreaseIOBursts();//another IO bursts completed, reduce number of bursts needed
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
        if(cpuInUse && time == burst_end)
        {
          //no context switch required
          if(readyQ.size() == 0)
          {
            //CPU burst finished
            if(currentProcess.getRemainingTimeInBurst() == 0)
            {
              currentProcess.decreaseCPUBursts(); //another CPU burst finished, decrement the count
              if(currentProcess.getNumBursts() == 0)
              {
                //process has finished executing, setServiced
                currentProcess.setServiced();
                serviceQ.push_back(currentProcess);
                decisionTime = time + (t_cs/2); //chose next process at this time
                startNextProcess = time + t_cs;
              }
              else //send process to IO
              {
                IOProcess = currentProcess;
                ioTime = time + (t_cs/2);
              }
              cpuInUse = false; //free up cpu from use so next event can occur
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
              int difference = currentProcess.getRemainingTimeInBurst() - t_slice;
              currentProcess.setRemainingTimeInBurst(difference);
            }
          }

          //context switch is required
          else
          {
            if(currentProcess.getRemainingTimeInBurst()==0)//CPU burst finished
            {
              currentProcess.decreaseCPUBursts();
              if(currentProcess.getNumBursts()==0)
              {
                //process just finished CPU burst, has no more CPU or IO bursts remaining
                //process is serviced
                currentProcess.setServiced();
                currentProcess.addContextSwitch();
                serviceQ.push_back(currentProcess);
              }
              else
              {
                //send process to IO
                currentProcess.addContextSwitch();
                IOProcess = currentProcess;
                ioTime = time + (t_cs/2);
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
                readyQ.push_back(currentProcess);
              }
            }
            decisionTime = time + (t_cs/2);
            startNextProcess = time + t_cs;
            cpuInUse = false;
          }
        }

        //cpu not in use since last burst ended, decide next process to start loading in
        else if(!cpuInUse && time == decisionTime && readyQ.size() != 0)
        {
          //assign this as our current process to start loading in
          currentProcess = readyQ[0];
          readyQ.erase(readyQ.begin());
        }

        //CPU not in use, it's time to load in a new process, and readyQ is not empty
        else if(!cpuInUse && time == startNextProcess)
        {
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
          currentProcess.addContextSwitch(); //switch from waitQ to CPU happened
          cpuInUse = true;
        }

        if(time == ioTime) //time to send IOProcess to IO
        {
          int returnTime = time + IOProcess.getIOTime();
          IOProcess.setBlockedUntil(returnTime);
          cout << "time " << time << "ms: Process " << IOProcess.getID() << " sent for IO burst ";
          printQ(readyQ);
        }

        if(!cpuInUse && startNextProcess < time && readyQ.size()>0)
        {
          //needed for when no processes waiting in readyQ for a while, all were in IO or waiting ot arrive at one point
          //startNextProcess needs to be reassigned and new current process needed to be chosen
          startNextProcess = time + (t_cs/2);
          currentProcess = readyQ[0];
          readyQ.erase(readyQ.begin());
        }
        time++;
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

    /*
    for(int i = 0; i < all_p.size(); i++)
    {

    }
    */
    cout <<"time " << time+2 << "ms: Simulator ended for RR" ;

    avg_tat = total_turn_around_time / float(context_switches);
    avg_bt = total_burst_times / float(context_switches);
    avg_wt = total_wait_time / float(context_switches);

}
