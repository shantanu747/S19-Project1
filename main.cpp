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
  int n = 6;

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

void RR(vector<Process> p, int n, int switch_time, int tslice, string behavior)
{
    deque<Process> all_p;
    for(int i = 0; i < p.size(); i++) //convert to deque for push_front capabilities
    {
      cout << "Process " << p[i].getID() << " [NEW] (arrival time " << p[i].getArrivalTime() << " ms) " << p[i].getNumBursts() << " CPU bursts" << endl;
      all_p.push_back(p[i]);
    }

    int t_cs = switch_time; // takes this much time to make a context switch
    int t_slice = tslice; // time slice for RR algorithm
    bool cpuInUse = false; // only set to true while process in CPU
    bool firstProcessArrived = false;
    string rradd = behavior; //BEGINNING OR END
    unsigned int time = 0; // overall timer for simulation
    int startNextProcess = -1; // By default first process should not start before this time
    int decisionTime = -1; //due to push-front behavior, make decision at burstEnd + t_cs/2 for next process
    int burst_end = -1; // acts as marker for when next context switch/preemption should occur
    int ioTime = -1;

    Process currentProcess; // holds ID of current process being serviced
    Process IOProcess; //hold ID of process to send to IO

    deque<Process> readyQ;
    deque<Process> serviceQ;

    // Begin algorithm simulation
    cout << "time " << time << "ms: Simulator started for RR " << endl;
    printQ_RR(readyQ);

    // while not all processes have been serviced
    while (serviceQ.size() != all_p.size())
    {
        //before adding/removing any process from CPU, make sure we have
        //the proper readyQ needed
        for (int i = 0; i < all_p.size(); i++)
        {
            // Check if any processes arrive at this time
            if (all_p[i].getArrivalTime() == time )
            {
                cout << "time " << time << "ms: Process " << all_p[i].getID() << " arrived and added to ready queue ";
                if(rradd == "BEGINNING")
                {
                  readyQ.push_front(all_p[i]);
                }
                else
                {
                  readyQ.push_back(all_p[i]);
                }

                //only applies to the first process to arrive
                if(!firstProcessArrived)
                {
                  firstProcessArrived = true;
                  startNextProcess = time + (t_cs/2); //first process just arrived, start loading into CPU and start executing at t_cs/2
                }
                printQ_RR(readyQ);
            }
            // Check if any processes come back from I/O at this time
            else if(all_p[i].getBlockedUntil() == time)
            {
              cout << "time " << time << "ms: Process " << all_p[i].getID() << " finished I/O and added to ready queue ";
              all_p[i].decreaseIOBursts();//another IO bursts completed, reduce number of bursts needed
              if(rradd == "BEGINNING")
              {
                readyQ.push_front(all_p[i]);
              }
              else
              {
                readyQ.push_back(all_p[i]);
              }
              printQ_RR(readyQ);
            }
        }

        //CPU currently in use but there is an event that needs handling
        if(cpuInUse && time == burst_end)
        {
          //no context switch required
          if(readyQ.size() == 0)
          {
            //CPU burst finished
            if(currentProcess.getRemainingTimeInBurst() == 0)
            {
              currentProcess.decreaseCPUBursts(); //another CPU burst finished, decrement the count
              if(currentProcess.getNumBursts() == 0)
              {
                //process has finished executing, setServiced
                currentProcess.setServiced();
                serviceQ.push_back(currentProcess);
                currentProcess.addContextSwitch();
                decisionTime = time + (t_cs/2); //chose next process at this time
                startNextProcess = time + t_cs;
              }
              else //send process to IO
              {
                IOProcess = currentProcess;
                ioTime = time + (t_cs/2);
              }
              cpuInUse = false; //free up cpu from use so next event can occur
            }
            //Remaining time might be less than t_slice
            else if(currentProcess.getRemainingTimeInBurst() <= t_slice)
            {
              burst_end = time + currentProcess.getRemainingTimeInBurst();
            }
            //extend by t_slice with no preemption
            else
            {
              burst_end = time + t_slice;
              int difference = currentProcess.getRemainingTimeInBurst() - t_slice;
              currentProcess.setRemainingTimeInBurst(difference);
            }
          }

          //context switch is required
          else
          {
            if(currentProcess.getRemainingTimeInBurst()==0)//CPU burst finished
            {
              currentProcess.decreaseCPUBursts();
              if(currentProcess.getNumBursts()==0)
              {
                //process just finished CPU burst, has no more CPU or IO bursts remaining
                //process is serviced
                currentProcess.setServiced();
                currentProcess.addContextSwitch();
                serviceQ.push_back(currentProcess);
              }
              else
              {
                //send process to IO
                currentProcess.addContextSwitch();
                IOProcess = currentProcess;
                ioTime = time + (t_cs/2);
              }
            }
            //current process needs more CPU time before IO
            else
            {
              currentProcess.addContextSwitch();
              currentProcess.addPreemptedCount();
              if(rradd == "BEGINNING")
              {
                readyQ.push_front(currentProcess);
              }
              else
              {
                readyQ.push_back(currentProcess);
              }
            }
            decisionTime = time + (t_cs/2);
            startNextProcess = time + t_cs;
            cpuInUse = false;
          }
        }

        //cpu not in use since last burst ended, decide next process to start loading in
        else if(!cpuInUse && time == decisionTime && readyQ.size() != 0)
        {
          //assign this as our current process to start loading in
          currentProcess = readyQ[0];
          readyQ.erase(readyQ.begin());
        }

        //CPU not in use, it's time to load in a new process, and readyQ is not empty
        else if(!cpuInUse && time == startNextProcess)
        {
          //burst will finish before or at the same time that timeslice expires
          if(currentProcess.getRemainingTimeInBurst() <= t_slice)
          {
            burst_end = time + currentProcess.getRemainingTimeInBurst();
            currentProcess.setRemainingTimeInBurst(0); //this way we know to send process to IO at burst end
          }
          //timeslice will expire first, setRemainingTimeInBurst used to save the difference
          else
          {
            burst_end = time + t_slice;
            int difference = currentProcess.getRemainingTimeInBurst() - t_slice;
            currentProcess.setRemainingTimeInBurst(difference);
          }
          currentProcess.addContextSwitch(); //switch from waitQ to CPU happened
          cpuInUse = true;
        }

        if(time == ioTime) //time to send IOProcess to IO
        {
          int returnTime = time + IOProcess.getIOTime();
          IOProcess.setBlockedUntil(returnTime);
          cout << "time " << time << "ms: Process " << IOProcess.getID() << " sent for IO burst ";
          printQ_RR(readyQ);
        }

        if(!cpuInUse && startNextProcess < time && readyQ.size()>0)
        {
          //needed for when no processes waiting in readyQ for a while, all were in IO or waiting ot arrive at one point
          //startNextProcess needs to be reassigned and new current process needed to be chosen
          startNextProcess = time + (t_cs/2);
          currentProcess = readyQ[0];
          readyQ.erase(readyQ.begin());
        }
        time++;
    }

    // calculations for avg algorithm stats
    float total_turn_around_time = 0;
    float total_burst_times = 0;
    float total_wait_time = 0;

    int preemptions = 0;
    int context_switches = 0;

    float avg_tat = 0.0; // average turn around time
    float avg_bt = 0.0; // average burst time
    float avg_wt = 0.0; // average wait time


    for(int i = 0; i < all_p.size(); i++)
    {

    }

    cout <<"time " << time+2 << "ms: Simulator ended for RR" ;

    avg_tat = total_turn_around_time / float(context_switches);
    avg_bt = total_burst_times / float(context_switches);
    avg_wt = total_wait_time / float(context_switches);
  
}

void SJF(vector<Process> all_p, int n, int switch_time)
{
  int t_cs = switch_time; // takes this much time to make a context switch
  int startNextIO = -1;
  int decisionTime = -1;
  int numProcesses = n;
  bool cpuInUse = false; // only set to true while process in CPU
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
    cout << "Process " << all_p[i].getID() << " [NEW] (arrival time " << all_p[i].getArrivalTime() << " ms) " << all_p[i].getNumBursts() << " CPU bursts" << endl;
  }

  // Begin algorithm simulation
  cout << "time " << time << "ms: Simulator started for SJF " << endl;
  /*
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
      cout << "time " << time << "ms: Process " << currentProcess.getID() << " started using the CPU for " << currentProcess.getBurstTime() << "ms burst ";
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
  */
}

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

    cout << "time " << time << "ms: Simulator started for SRT " << endl;
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

void FCFS(vector<Process> all_p, int n, int switch_time)
{
    int t_cs = switch_time;
    unsigned int time = 0;

    vector<Process> all_processes = all_p; //Copy of the passed in process vector
    vector<Process> readyQ; //Waiting processes get added here
    vector<Process> serviceQ; //Finished processes get added here

    bool cpuInUse = false; //Set to true only when the CPU is being used

    //For keeping track of various important times in the CPU burst process
    int burstEnd, startNextIO, startNextProcess, returnTime;

    Process currentProcess; //Holds currently bursting process
    Process IOProcess; //Holds current process in IO

    //Initial output
    for(int i = 0; i < all_processes.size(); i++)
    {
      cout << "Process " << all_processes[i].getID() << " [NEW] (arrival time " << all_processes[i].getArrivalTime() << " ms) " << all_processes[i].getNumBursts() << " CPU bursts" << endl;
    }

    cout << "time " << time << "ms: Simulator started for FCFS " << endl;
    /*
    printQ(readyQ);

    //Iterates until all processes have been added to serviceQ
    while(serviceQ.size() != all_processes.size()){

        //Iterates over each process to see if anything arrives at this time
        for(int i = 0; i < all_processes.size(); i++){

            //Check to see if i-th process arrives *now*
            if(all_processes[i].getArrivalTime() == time){
                cout << "time " << time << "ms: Process " << all_processes[i].getID() << " arrived; added to ready queue ";
                readyQ.push_back(all_processes[i]);
                printQ(readyQ);
            }

            //Check to see if i-th process returns from I/O *now*
            else if(all_processes[i].getBlockedUntil() == time){
                // Check if any processes come back from I/O at this time
                cout << "time " << time << "ms: Process " << all_processes[i].getID() << " completed I/O; added to ready queue ";
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
                serviceQ.push_back(currentProcess);
            }

            //Current burst gets sent to IO, CPU use disabled
            else if(currentProcess.getNumBursts() > 0){
                startNextIO = time + (t_cs/2); //time needed to exit CPU
                startNextProcess = time + t_cs; //next process starts at this time
                IOProcess = currentProcess;
                returnTime = startNextIO + IOProcess.getIOTime();
                IOProcess.setBlockedUntil(returnTime);
                IOProcess.addContextSwitch(); //increment context switch count for this process
                cpuInUse = false; //CPU is not in use anymore
            }
        }

        //CPU ready to accept frontmost process from the readyQ
        else if(!cpuInUse && time == startNextProcess){
            currentProcess = readyQ[0];
            readyQ.erase(readyQ.begin()); //remove the process from the readyQ

            burstEnd = time + currentProcess.getBurstTime();
            cpuInUse = true; //CPU is now in use
            cout << "time " << time << "ms: Process " << currentProcess.getID() << " started using the CPU for " << currentProcess.getBurstTime() << "ms burst ";
            printQ(readyQ);
        }

        //output message saying process is now in IO
        if(time == startNextIO){
            cout << "time " << time << "ms: Process " << IOProcess.getID() << " switching out of CPU; will block on I/O until time " << IOProcess.getBlockedUntil() << "ms ";
            printQ(readyQ);
        }
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

    int context_switches = 0;

    float avg_tat = 0.0; // average turn around time
    float avg_bt = 0.0; // average burst time
    float avg_wt = 0.0; // average wait time
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
  int n = 6;
  int t_cs = 8;
  int timeslice = 84;
  string rradd = "END";

  vector<Process> processes;
  processes = process_helper();
  /*
  SJF(processes, n, t_cs);
  processes = process_helper();
  SRT(processes, n, t_cs);
  processes = process_helper();
  FCFS(processes, n, t_cs);
  processes = process_helper();
  */
  RR(processes, n, t_cs, timeslice, rradd);
  return 0;
}
