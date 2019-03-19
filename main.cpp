#include <iostream>
#include <stdio.h>
#include <stdlib.h>
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
#include "functions.h"

using namespace std;

// after each simulation algorithm ends, we regenerate vector<Process>
// with new burst times, burst count, io times
// call next simulation
vector<Process> process_helper()
{
  long int seed = 777;
  srand(seed);
  string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  float lambda = 0.01;
  int upperBound = 3000;
  int n = 26;
  vector<Process> all_processes;
  /* Code to create processes and assign:
      - arrival time
      - cpu time per busrt
      - number of bursts
      - io burst time
      Goes here
  */

  for(int i = 0; i < n; i++)
  {
    string pid = to_string(alphabet[i]);

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

    Process t = new Process(pid, arrivalTime, burstTime, bursts, ioTime);
    all_processes.push_back(t);
  }
  return all_processes;
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

  vector<Process> processes;
  processes = process_helper();
  for(int i = 0; i < processes.size(); i++)
  {
    string pid = processes[i].getID();
    int burstTime = processes[i].getBurstTime();
    int numBursts = processes[i].getNumBursts();
    int ioTime = processes[i].getIOTime();
    int ioBursts = processes[i].getIOBursts();
    cout << pid << " " << burstTime << " " << numBursts << " " << ioTime << " " << ioBursts << endl;
  }
  /*
  SJF(processes, n, t_cs);
  processes = process_helper();
  SRT(processes, n, t_cs);
  processes = process_helper();
  FCFS(processes, n, t_cs);
  processes = process_helper();
  RR(processes, n, t_cs, timeslice, rradd);
  */
  return 0;
}
