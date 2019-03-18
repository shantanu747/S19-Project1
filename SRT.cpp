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

//std::sort optional 3rd argument for sorting by remaining time in burst
bool sortHelper(Process a, Process b){
  return (a.getRemainingTimeInBurst() < b.getRemainingTimeInBurst());
}

    //Process list | num of processes | context switch time
void srt(vector<Process> p, int n, int t_cs){

    vector<Process> all_processes = p; //Copy of the passed in process vector
    vector<Process> readyQ; //Waiting processes get added here
    vector<Process> serviceQ; //Finished processes get added here
    Process currentProcess; //to hold the data for the current bursting process

    bool cpuInUse = false; //set to true only when the CPU is being used
    unsigned int funcTime = 0; //Total running time (in loop iterations)

    cout << "time " << time << "ms: Simulator started for RR ";
    printQ(readyQ);

    //Iterates until all processes have been added to serviceQ
    while(serviceQ.size() != all_processes.size()){

        //Iterates over each process to see if there should be
        //a context switch due to a new process arriving
        for(int i = 0; i < all_processes.size(); i++){
            
        }
        funcTime++;
    }

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
