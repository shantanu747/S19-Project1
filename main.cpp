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
#include <dequeue>
#include <cassert>

//#include "process.h"

using namespace std;

// after each simulation algorithm ends, we regenerate vector<Process>
// with new burst times, burst count, io times
// call next simulation
dequeue<Process> process_helper()
{
  dequeue<Process> all_processes;
  /* Code to create processes and assign:
      - arrival time
      - cpu time per busrt
      - number of bursts
      - io burst time
      Goes here
  */
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

  if(argc != 8 || argc != 9)
  {
    std::cerr << "ERROR: Incorrect number of arguments supplied!" << '\n';
    return 1;
  }

  int seed = atoi(argv[1]);
  int lambda = atoi(argv[2]);
  int upper_bound = atoi(argv[3]);
  int n = atoi(argv[4]);
  int t_cs = atoi(argv[5]);
  itn alpha = atoi(argv[6]);
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

  //uncomment as functions are written and testable
  /*
  dequeue<Process> processes;
  processes = process_helper();
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
