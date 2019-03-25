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

bool sortHelper(Process a, Process b)
{
  return (a.getBurstTime() < b.getBurstTime());
}

void SFJ(vector<Process> all_p, int n, int switch_time)
{
  int t_cs = switch_time; // takes this much time to make a context switch
  int startNextIO = -1;
  int decisionTime = -1;
  int numProcesses = n;
  bool cpu_in_use = false; // only set to true while process in CPU
  unsigned int time = 0; // overall timer for simulation
  int startNextProcess = -1; // By default first process should not start before this time
  int burstEnd = -1;

  vector<Process> readyQ;
  vector<Process> serviceQ;
  Process currentProcess;
  Process IOProcess;

  //Initial output
  for(int i = 0; i < all_p.size(); i++)
  {
    cout << "Process " << p[i].getID() << " [NEW] (arrival time " << p[i].getArrivalTime() << " ms) " << p[i].getNumBursts() << " CPU bursts" << endl;
  }

  // Begin algorithm simulation
  cout << "time " << time << "ms: Simulator started for SJF ";
  printQ(readyQ);

  while(serviceQ.size() != numProcesses)
  {
    //Beginning of each iteration check for processes arriving
    //Also check for processes coming back from IO
    for(int i = 0; i < numProcesses; i++)
    {
      if(time == all_p[i].getArrivalTime())
      {
        readyQ.push_back(all_p[i]);
        cout << "time " << time << "ms: Process " << all_p[i].getID() << " arrived and added to ready queue ";
        printQ(readyQ);
      }
      else if(time == all_p[i].getBlockedUntil())
      {
        readyQ.push_back(all_p[i]);
        cout << "time " << time << "ms: Process " << all_p[i].getID() << " finished I/O and added to ready queue ";
        all_p[i].decreaseIOBursts(); //another IO burst finished, decrement counter for remaining bursts
        printQ(readyQ);
      }
    }
    sort(readyQ.begin(), readyQ.end(), sortHelper); //sorts readyQ by shortest burst time required

    //burst ended, send to IO, reassign current process
    if(cpuInUse && time == burstEnd)
    {
      cout << "time " << time << "ms: Process " << currentProcess.getID() << " completed a CPU burst ";
      printQ(readyQ);
      currentProcess.decreaseCPUBursts();
      if(currentProcess.getNumBursts()==0)
      {
        //process is finished, no need for IO
        currentProcess.setServiced();
        serviceQ.push_back(currentProcess);
        startNextProcess = time + t_cs;
      }
      else if(currentProcess.getNumBursts() > 0)
      {
        //IO needed
        startNextIO = time + (t_cs/2); //time needed to exit CPU
        decisionTime = time + (t_cs/2); //shortest process at this time is next one to be loaded into CPU, not shortest process at startNextProcess
        startNextProcess = time + t_cs; //next process starts at this time
        IOProcess = currentProcess;
        IOProcess.addContextSwitch(); //increment context switch count for this process
        cpuInUse = false; //CPU is not in use anymore
      }
    }

    else if(!cpuInUse && time == decisionTime)
    {
      currentProcess = readyQ[0]; //shortest process at decisionTime is the one we load into the CPU
      readyQ.erase(readyQ.begin()); //remove the process from the readyQ
    }

    //time to put new process into the CPU
    else if(!cpuInUse && time == startNextProcess)
    {
      burstEnd = time + currentProcess.getBurstTime();
      cpuInUse = true; //CPU is now in use
      cout << "time " << time << "ms: Process " << currentProcess.getID() << " started using the CPU ";
      printQ(readyQ);
    }

    //output message saying process is now in IO
    if(time == startNextIO)
    {
      int returnTime = startNextIO + IOProcess.getIOTime();
      IOProcess.setBlockedUntil(returnTime);
      cout << "time " << time << "ms: Process " << IOProcess.getID() << " sent for IO burst ";
      printQ(readyQ);
    }
    time++;
  }

  // calculations for avg algorithm stats
  float total_turn_around_time = 0;
  float total_burst_times = 0;
  float total_wait_time = 0;

  int context_switches = 0;

  float avg_tat = 0.0; // average turn around time
  float avg_bt = 0.0; // average burst time
  float avg_wt = 0.0; // average wait time

  for(int i = 0; i < all_p.size(); i++)
  {

  }
}
