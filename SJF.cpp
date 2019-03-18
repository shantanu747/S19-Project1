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
  int numProcesses = n;
  bool cpu_in_use = false; // only set to true while process in CPU
  unsigned int time = 0; // overall timer for simulation
  int startNextProcess = -1; // By default first process should not start before this time
  int burstEnd = -1;

  vector<Process> readyQ;
  vector<Process> serviceQ;
  Process currentProcess;
  Process nextProcess;

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
        printQ(readyQ);
      }
    }
    sort(readyQ.begin(), readyQ.end(), sortHelper); //sorts readyQ by shortest burst time required

    //burst ended, send to IO, reassign current process
    if(cpuInUse && time == burstEnd)
    {
      cout << "time " << time << "ms: Process " << currentProcess.getID() << " finished I/O and added to ready queue ";
    }
    //time to put new process into the CPU
    else if(!cpuInUse && time == startNextProcess)
    {
      currentProcess = readyQ[0];
      startNextProcess = readyQ[0].getArrivalTime()
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
