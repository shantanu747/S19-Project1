#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

#include <string>
#include <deque>
#include <iomanip>
#include <algorithm>

#include "process.h"

using namespace std;

//std::sort optional 3rd argument for sorting by remaining time in burst

    //Process list | num of processes | context switch time
void SRT(vector<Process> p, int n, int t_cs){

    vector<Process> all_processes = p; //Copy of the passed in process vector
    vector<Process> readyQ; //Waiting processes get added here
    vector<Process> serviceQ; //Finished processes get added here
    Process currentProcess; //To hold the data for the current bursting process

    bool cpuInUse = false; //Set to true only when the CPU is being used
    bool firstProcessArrived = false;
    unsigned int time = 0; //Total running time (in loop iterations)
    int startNextProcess = -1; //Next process should start at this time iff there are no preemptions
    int makeDecisionTime = -1; //Chooses next process at makeDecisionTime

    //Initial output
    for(int i = 0; i < all_processes.size(); i++)
    {
      cout << "Process " << p[i].getID() << " [NEW] (arrival time " << p[i].getArrivalTime() << " ms) " << p[i].getNumBursts() << " CPU bursts" << endl;
    }

    cout << "time " << time << "ms: Simulator started for SRT ";
    /*
    printQ(readyQ);

    //Iterates until all processes have been added to serviceQ
    while(serviceQ.size() != all_processes.size()){

        //Iterates over each process to see if anything arrives at this time
        for(int i = 0; i < all_processes.size(); i++){

            //Check to see if i-th process arrives *now*
            if(all_processes[i].getArrivalTime() == time){
                cout << "time " << time << "ms: Process " << all_processes[i].getID() << " arrived and added to ready queue ";
                readyQ.push_back(all_processes[i]);

                //Signifies the first process has arrived
                //Necessary because nothing preempts the first process
                if(!firstProcessArrived){
                    firstProcessArrived = true;
                    startNextProcess = readyQ[0].getArrivalTime() + t_cs/2;
                }
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

        //Sort the readyQ
        sort(readyQ.begin(), readyQ.end(), sortHelper);



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

    int preemptions = 0;
    int context_switches = 0;

    float avg_tat = 0.0; // average turn around time
    float avg_bt = 0.0; // average burst time
    float avg_wt = 0.0; // average wait time
}
