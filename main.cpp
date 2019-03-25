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

#include "RR.cpp"
#include "SJF.cpp"
#include "SRT.cpp"
#include "FCFS.cpp"


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
  int n = 6;
  double container;
  vector<Process> all_processes;

  for(int i = 0; i < n; i++)
  {
    string name = to_string(alphabet[i]);

    container = -log(drand48())/lambda;
    while(container > upperBound) //if above upper bound keep recalculating until it is within range
    {
      container = -log(drand48())/lambda;
    }
    int arrivalTime = container;

    container = -log(drand48())/lambda;
    while(container > upperBound) //if above upper bound keep recalculating until it is within range
    {
      container = -log(drand48())/lambda;
    }
    int ioTime = container;

    container = -log(drand48())/lambda;
    while(container > upperBound) //if above upper bound keep recalculating until it is within range
    {
      container = -log(drand48())/lambda;
    }
    int burstTime = container;

    int bursts = floor(drand48()*100);
    bursts += 1;

    Process t(name, arrivalTime, burstTime, bursts, ioTime, lambda);
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
  /*

  int n = 6;
  int t_cs = 8;
  int timeslice = 84;
  string rradd = "END";

  vector<Process> processes;
  processes = process_helper();
  cout << processes.size() << endl;
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
