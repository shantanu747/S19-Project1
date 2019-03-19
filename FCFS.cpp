#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <deque>
#include <iomanip>
#include <algorithm>
#include "process.h"
#include "functions.h"

using namespace std;

void FCFS(vector<Process> all_p, int n, int switch_time){
    int t_cs = switch_time;
    unsigned int time = 0;

    vector<Process> all_processes = all_p; //Copy of the passed in process vector
    vector<Process> readyQ; //Waiting processes get added here
    vector<Process> serviceQ; //Finished processes get added here

    bool cpuInUse = false; //Set to true only when the CPU is being used
    bool firstProcessArrived = false;

    int burstEnd = -1;

    Process currentProcess;
    Process IOProcess;

    cout << "time " << time << "ms: Simulator started for FCFS ";
    printQ(readyQ);

    //Iterates until all processes have been added to serviceQ
    while(serviceQ.size() != all_processes.size()){

        //Iterates over each process to see if anything arrives at this time
        for(int i = 0; i < all_processes.size(); i++){

            //Check to see if i-th process arrives *now*
            if(all_processes[i].getArrivalTime() == time){
                cout << "time " << time << "ms: Process " << all_processes[i].getID() << " arrived and added to ready queue ";
                readyQ.push_back(all_processes[i]);
            }

            //Check to see if i-th process returns from I/O *now*
            else if(all_processes[i].getBlockedUntil() == time){
                // Check if any processes come back from I/O at this time
                cout << "time " << time << "ms: Process " << all_processes[i].getID() << " finished I/O and added to ready queue ";
                readyQ.push_back(all_processes[i]);
                printQ(readyQ);
            }
            
            //Nothing arrives at 'time', continue the loop
            else{
                continue;
            }
        }

        //Current CPU Burst ended, send process to IO
        if(cpuInUse && burstEnd == time){
            cout << "time " << time << "ms: Process " << currentProcess.getID() << " completed a CPU burst ";
            printQ(readyQ);
            currentProcess.decreaseCPUBursts();

            //Process has no more remaining bursts, service the process
            if(currentProcess.getNumBursts()==0){
                currentProcess.setServiced();

            }
        }

}