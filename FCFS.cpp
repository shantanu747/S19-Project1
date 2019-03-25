#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <deque>
#include <iomanip>
#include <algorithm>

#include "process.h"

using namespace std;

void printQ_FCFS(vector<Process> &all)
{
    if (all.size() == 0)
    {
        cout << "[Q <empty>]" << endl;
        return;
    }
    string q;
    q += "[Q ";
    for (int i = 0; i < all.size(); i++)
    {
        q += all[i].getID();
        q += " ";
    }
    //q.pop_back();
    q += "]";
    cout << q << endl;
}

void FCFS(vector<Process> all_p, int n, int switch_time)
{
    int t_cs = switch_time;
    unsigned int time = 0;

    vector<Process> all_processes = all_p; //Copy of the passed in process vector
    vector<Process> readyQ; //Waiting processes get added here
    vector<Process> serviceQ; //Finished processes get added here

    bool cpuInUse = false; //Set to true only when the CPU is being used

    //For keeping track of various important times in the CPU burst process
    int burstEnd, startNextIO, startNextProcess, returnTime;

    Process currentProcess; //Holds currently bursting process
    Process IOProcess; //Holds current process in IO

    //Initial output
    for(int i = 0; i < all_processes.size(); i++)
    {
      cout << "Process " << all_processes[i].getID() << " [NEW] (arrival time " << all_processes[i].getArrivalTime() << " ms) " << all_processes[i].getNumBursts() << " CPU bursts" << endl;
    }

    cout << "time " << time << "ms: Simulator started for FCFS ";
    /*
    printQ_FCFS(readyQ);

    //Iterates until all processes have been added to serviceQ
    while(serviceQ.size() != all_processes.size()){

        //Iterates over each process to see if anything arrives at this time
        for(int i = 0; i < all_processes.size(); i++){

            //Check to see if i-th process arrives *now*
            if(all_processes[i].getArrivalTime() == time){
                cout << "time " << time << "ms: Process " << all_processes[i].getID() << " arrived; added to ready queue ";
                readyQ.push_back(all_processes[i]);
                printQ_FCFS(readyQ);
            }

            //Check to see if i-th process returns from I/O *now*
            else if(all_processes[i].getBlockedUntil() == time){
                // Check if any processes come back from I/O at this time
                cout << "time " << time << "ms: Process " << all_processes[i].getID() << " completed I/O; added to ready queue ";
                readyQ.push_back(all_processes[i]);
                printQ_FCFS(readyQ);
            }

            //Nothing arrives at 'time', continue the loop
            else{
                continue;
            }
        }

        //Current CPU Burst ended, send process to IO
        if(cpuInUse && burstEnd == time){
            cout << "time " << time << "ms: Process " << currentProcess.getID() << " completed a CPU burst ";
            printQ_FCFS(readyQ);
            currentProcess.decreaseCPUBursts();

            //Process has no more remaining bursts, service the process
            if(currentProcess.getNumBursts()==0){
                currentProcess.setServiced();
                serviceQ.push_back(currentProcess);
            }

            //Current burst gets sent to IO, CPU use disabled
            else if(currentProcess.getNumBursts() > 0){
                startNextIO = time + (t_cs/2); //time needed to exit CPU
                startNextProcess = time + t_cs; //next process starts at this time
                IOProcess = currentProcess;
                returnTime = startNextIO + IOProcess.getIOTime();
                IOProcess.setBlockedUntil(returnTime);
                IOProcess.addContextSwitch(); //increment context switch count for this process
                cpuInUse = false; //CPU is not in use anymore
            }
        }

        //CPU ready to accept frontmost process from the readyQ
        else if(!cpuInUse && time == startNextProcess){
            currentProcess = readyQ[0];
            readyQ.erase(readyQ.begin()); //remove the process from the readyQ

            burstEnd = time + currentProcess.getBurstTime();
            cpuInUse = true; //CPU is now in use
            cout << "time " << time << "ms: Process " << currentProcess.getID() << " started using the CPU for " << currentProcess.getBurstTime() << "ms burst ";
            printQ_FCFS(readyQ);
        }

        //output message saying process is now in IO
        if(time == startNextIO){
            cout << "time " << time << "ms: Process " << IOProcess.getID() << " switching out of CPU; will block on I/O until time " << IOProcess.getBlockedUntil() << "ms ";
            printQ_FCFS(readyQ);
        }
        time++;
    }

    */

    //******************************//
    // END OF FUNCTION CALCULATIONS //
    //******************************//

    // calculations for avg algorithm stats
    float total_turn_around_time = 0;
    float total_burst_times = 0;
    float total_wait_time = 0;

    int context_switches = 0;

    float avg_tat = 0.0; // average turn around time
    float avg_bt = 0.0; // average burst time
    float avg_wt = 0.0; // average wait time
}
