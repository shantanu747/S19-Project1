#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <utility>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <deque>
#include <cassert>
#include <cmath>

#include "process.h"

using namespace std;


void printQ(vector<Process> &all)
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

//std::sort optional 3rd argument for sorting by remaining time in burst
bool sortHelper(Process a, Process b)
{
  return (a.getRemainingTimeInBurst() < b.getRemainingTimeInBurst());
}

void printQ_RR(deque<Process> &all)
{
    if (all.size() == 0)
    {
        cout << "[Q <empty>]" << endl;
        return;
    }
    string queue = "[Q ";
    for (int i = 0; i < all.size(); i++)
    {
        queue += all[i].getID();
        queue += " ";
    }
    queue += "]";
    cout << queue << endl;
}

// after each simulation algorithm ends, we regenerate vector<Process>
// with new burst times, burst count, io times
// call next simulation
vector<Process> process_helper()
{
  long int seed = 777;
  srand48(seed);
  string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  float lambda = 0.01;
  int upperBound = 3000;
  int n = 2;

  vector<Process> all_processes;

  for(int i = 0; i < n; i++)
  {
    char name = alphabet[i];

    int arrivalTime = -log(drand48())/lambda;
    while(arrivalTime > upperBound) //if above upper bound keep recalculating until it is within range
    {
      arrivalTime = -log(drand48())/lambda;
    }

    int ioTime = -log(drand48())/lambda;
    while(ioTime > upperBound) //if above upper bound keep recalculating until it is within range
    {
      ioTime = -log(drand48())/lambda;
    }

    int burstTime = -log(drand48())/lambda;
    while(burstTime > upperBound) //if above upper bound keep recalculating until it is within range
    {
      burstTime = -log(drand48())/lambda;
    }

    int bursts = floor(drand48()*100);
    bursts += 1;

    Process t(name, arrivalTime, burstTime, bursts, ioTime, lambda);
    all_processes.push_back(t);
  }
  return all_processes;
}

void SJF(vector<Process> all_p, int n, int switch_time)
{
  int t_cs = switch_time; // takes this much time to make a context switch
  int startNextIO = -1;
  int decisionTime = -1;
  int numProcesses = n;
  bool cpuInUse = false; // only set to true while process in CPU
  bool firstProcessArrived = false;
  unsigned int time = 0; // overall timer for simulation
  int startNextProcess = -1; // By default first process should not start before this time
  int burstEnd = -1;

  vector<Process> readyQ;
  vector<Process> serviceQ;
  int cp;
  int ip;

  //Initial output
  for(int i = 0; i < all_p.size(); i++)
  {
    cout << "Process " << all_p[i].getID() << " [NEW] (arrival time " << all_p[i].getArrivalTime() << " ms) " << all_p[i].getNumBursts() << " CPU bursts" << endl;
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
        if(!firstProcessArrived)
        {
          firstProcessArrived = true;
          startNextProcess = time + (t_cs/2);
          cp = i;
        }
      }
      if(time == all_p[i].getBlockedUntil())
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
      all_p[cp].decreaseCPUBursts();
      cout << "time " << time << "ms: Process " << all_p[cp].getID() << " completed a CPU burst; " << all_p[cp].getNumBursts() << " bursts to go ";
      printQ(readyQ);
      if(all_p[cp].getNumBursts()==0)
      {
        //process is finished, no need for IO
        all_p[cp].setServiced();
        serviceQ.push_back(all_p[cp]);
        cout << "time " << time << "ms: Process " << all_p[cp].getID() << " terminated ";
        if(readyQ.size() > 0)
        {
          decisionTime = time + (t_cs/2); //shortest process at this time is next one to be loaded into CPU, not shortest process at startNextProcess
          startNextProcess = time + t_cs; //next process starts at this time
        }
      }
      else if(all_p[cp].getNumBursts() > 0)
      {
        //IO needed
        startNextIO = time + (t_cs/2); //time needed to exit CPU
        if(readyQ.size() > 0)
        {
          decisionTime = time + (t_cs/2); //shortest process at this time is next one to be loaded into CPU, not shortest process at startNextProcess
          startNextProcess = time + t_cs; //next process starts at this time
        }
        ip = cp;
        all_p[ip].addContextSwitch(); //increment context switch count for this process
        cpuInUse = false; //CPU is not in use anymore
      }
    }

    else if(!cpuInUse && time == decisionTime)
    {
      for(int i = 0; i < all_p.size(); i++)
      {
        if(all_p[i].getID() == readyQ[0].getID())
        {
          cp = i;
          break;
        }
      }
      readyQ.erase(readyQ.begin()); //remove the process from the readyQ
    }

    //time to put new process into the CPU
    else if(!cpuInUse && time == startNextProcess)
    {
      burstEnd = time + all_p[cp].getBurstTime();
      cpuInUse = true; //CPU is now in use
      cout << "time " << time << "ms: Process " << all_p[cp].getID() << " started using the CPU for " << all_p[cp].getBurstTime() << "ms burst ";
      printQ(readyQ);
    }

    //output message saying process is now in IO
    if(time == startNextIO)
    {
      int returnTime = startNextIO + all_p[ip].getIOTime();
      all_p[ip].setBlockedUntil(returnTime);
      cout << "time " << time << "ms: Process " << all_p[ip].getID() << " switching out of CPU; will block on I/O until time " << all_p[ip].getBlockedUntil() << "ms ";
      printQ(readyQ);
    }

    if(!cpuInUse && startNextProcess < time && readyQ.size() != 0)
    {
      startNextProcess = time + (t_cs/2);
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

int main(int argc, char const *argv[])
{
  /*
    argv[1] - seed for random number generator
    argv[2] - lambda
    argv[3] - upper bound, if random number is generated above this, ignore and
              regenerate ot
    argv[4] - n, number of processes to simulate
    argv[5] - tcs, time it takes to perform a context switch
    argv[6] - SJF and SRT. Constant alpha value for exponential averaging
    argv[7] - timeslice for RR
    argv[8] - BEGINNING or END to figure out where to pop process into ready
              queue for RR. Optional
  */

  /*

  if(argc != 8 || argc != 9)
  {
    std::cerr << "ERROR: Incorrect number of arguments supplied!" << '\n';
    return 1;
  }

  //int seed = atoi(argv[1]);
  int lambda = atoi(argv[2]);
  int upper_bound = atoi(argv[3]);
  int n = atoi(argv[4]);

  if(n > 26) //more processes than we were told to expect
  {
    cerr << "ERROR: No more than 26 processes accepted!" << '\n';
  }

  int t_cs = atoi(argv[5]);

  if((t_cs%2) != 0) //t_cs needs to be an even number
  {
    cerr << "ERROR: t_cs needs to be an even integer!" << '\n';
  }
  int alpha = atoi(argv[6]);
  int timeslice = atoi(argv[7]);
  string rradd;
  if(argc = 9) //optional argument supplied
  {
    rradd = argv[8]
  }
  else
  {
    rradd = "END"; //default behavior
  }
  */

  //uncomment as functions are written and testable
  int n = 2;
  int t_cs = 8;
  int timeslice = 84;
  string rradd = "END";

  vector<Process> processes;
  processes = process_helper();
  //cout << processes.size() << endl;
  SJF(processes, n, t_cs);
  //processes = process_helper();
  //SRT(processes, n, t_cs);
  //processes = process_helper();
  //FCFS(processes, n, t_cs);
  //processes = process_helper();
  //RR(processes, n, t_cs, timeslice, rradd);
  return 0;
}
