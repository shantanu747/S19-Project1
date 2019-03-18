#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <deque>
#include <iomanip>
#include <algorithm>
#include "process.h"
#include "main.cpp"

using namespace std;
      // process vector | num inputs

bool sortHelper(Process a, Process b)
{
  return (a.getRemainingTimeInBurst() < b.getRemainingTimeInBurst());
}

    //Process list | num of processes | context switch time
void srt(vector<Process> p, int n, int t_cs){

    vector<Process> all_processes = p; //Copy of the passed in process vector
    vector<Process> readyQ; //Waiting processes get added here
    vector<Process> serviceQ; //Finished processes get added here
    Process currentProcess; //to hold the data for the current bursting process

    bool cpuInUse = false; //set to true only when the CPU is being used
    unsigned int time = 0; //Total running time (in loop iterations)

    cout << "time " << time << "ms: Simulator started for RR ";
    printQ(readyQ);


}
