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
        if(i == all.size()-1){
          q += "]";
        }
        else{
          q += " ";
        }
    }
    //q.pop_back();
    cout << q << endl;
}

//std::sort optional 3rd argument for sorting by remaining time in burst
bool sortHelper(Process a, Process b)
{
  return (a.getTau() < b.getTau());
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
        if(i == all.size()-1){
          queue += "]";
        }
        else{
          queue += " ";
        }
    }
    cout << queue << endl;
}

// after each simulation algorithm ends, we regenerate vector<Process>
// with new burst times, burst count, io times
// call next simulation
vector<Process> process_helper()
{
  long int seed = 2;
  srand48(seed);
  string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  float lambda = 0.01;
  int upperBound = 200;
  int n = 2;

  vector<Process> all_p;

  for(int i = 0; i < n; i++)
  {
    char name = alphabet[i];

    int arrivalTime = floor(-log(drand48())/lambda);
    while(arrivalTime > upperBound) //if above upper bound keep recalculating until it is within range
    {
      arrivalTime = floor(-log(drand48())/lambda);
    }

    int cpuBursts = floor(drand48()*100);
    cpuBursts += 1;
    int ioBursts = cpuBursts - 1;
    vector<int> burstTimes;
    vector<int> ioBurstTimes;
    for(int i = 0; i < cpuBursts; i++)
    {
      if(i == cpuBursts-1)
      {
        int burstTime = ceil(-log(drand48())/lambda);
        while(burstTime > upperBound) //if above upper bound keep recalculating until it is within range
        {
          burstTime = ceil(-log(drand48())/lambda);
        }
        burstTimes.push_back(burstTime);
      }
      else
      {
        int burstTime = ceil(-log(drand48())/lambda);
        while(burstTime > upperBound) //if above upper bound keep recalculating until it is within range
        {
          burstTime = ceil(-log(drand48())/lambda);
        }
        burstTimes.push_back(burstTime);

        int ioTime = ceil(-log(drand48())/lambda);
        while(ioTime > upperBound) //if above upper bound keep recalculating until it is within range
        {
          ioTime = ceil(-log(drand48())/lambda);
        }
        ioBurstTimes.push_back(ioTime);
      }
    }

    Process t(name, arrivalTime, burstTimes, cpuBursts, ioBurstTimes, ioBursts, lambda);
    all_p.push_back(t);
  }
  return all_p;
}

void SJF(vector<Process> all_p, int n, int switch_time)
{
  int t_cs = switch_time; // takes this much time to make a context switch
  int decisionTime = -1;
  int numProcesses = n;
  bool cpuInUse = false; // only set to true while process in CPU
  bool firstProcessArrived = false;
  long int time = 0; // overall timer for simulation
  int startNextProcess = -1; // By default first process should not start before this time
  int burst_end = -1;
  int endSim = -1;
  float alpha = 0.5;

  vector<Process> readyQ;
  vector<Process> serviceQ;
  int cp;

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
        cout << "time " << time << "ms: Process " << all_p[i].getID() << " (tau " << all_p[i].getTau() << "ms) arrived; added to ready queue ";
        printQ(readyQ);
        if(!firstProcessArrived)
        {
          firstProcessArrived = true;
          decisionTime = time;
          startNextProcess = time + (t_cs/2);
          cp = i;
        }
      }
      if(time == all_p[i].getBlockedUntil())
      {
        readyQ.push_back(all_p[i]);
        cout << "time " << time << "ms: Process " << all_p[i].getID() << " (tau " << all_p[i].getTau() <<"ms) completed I/O; added to ready queue ";
        printQ(readyQ);
        all_p[i].decreaseIOBursts(); //another IO burst finished, decrement counter for remaining bursts
        all_p[i].resetIOBurst();
      }
    }
    sort(readyQ.begin(), readyQ.end(), sortHelper); //sorts readyQ by shortest burst time required

    //burst ended, send to IO, reassign current process
    if(cpuInUse && time == burst_end)
    {
      all_p[cp].decreaseCPUBursts();
      if(all_p[cp].getNumBursts()==0)
      {
        //process is finished, no need for IO
        all_p[cp].setServiced();
        serviceQ.push_back(all_p[cp]);
        cout << "time " << time << "ms: Process " << all_p[cp].getID() << " terminated ";
        printQ(readyQ);
        if(readyQ.size() > 0)
        {
          decisionTime = time + (t_cs/2); //shortest process at this time is next one to be loaded into CPU, not shortest process at startNextProcess
          startNextProcess = time + t_cs; //next process starts at this time
        }
        cpuInUse = false;
      }
      else if(all_p[cp].getNumBursts() > 0)
      {
        //IO needed
        cout << "time " << time << "ms: Process " << all_p[cp].getID() << " completed a CPU burst; " << all_p[cp].getNumBursts() << " bursts to go ";
        printQ(readyQ);
        int newTau = (alpha*all_p[cp].getBurstTime()) + ((1-alpha)*all_p[cp].getTau());
        //newTau = newTau + 1;
        all_p[cp].setTau(newTau);
        cout << "time " << time << "ms: Recalculated tau = " << newTau << "ms for process " << all_p[cp].getID() << " ";
        printQ(readyQ);
        all_p[cp].resetCPUBurst();
        all_p[cp].addContextSwitch(); //increment context switch count for this process
        int returnTime = time + (t_cs/2) + all_p[cp].getIOTime();
        all_p[cp].setBlockedUntil(returnTime);
        cout << "time " << time << "ms: Process " << all_p[cp].getID() << " switching out of CPU; will block on I/O until time " << all_p[cp].getBlockedUntil() << "ms ";
        printQ(readyQ);
        if(readyQ.size() > 0)
        {
          decisionTime = time + (t_cs/2); //shortest process at this time is next one to be loaded into CPU, not shortest process at startNextProcess
          startNextProcess = time + t_cs; //next process starts at this time
        }
        cpuInUse = false; //CPU is not in use anymore
      }
    }

    if(!cpuInUse && startNextProcess < time && readyQ.size() > 0)
    {
      decisionTime = time;
      startNextProcess = time + (t_cs/2);
    }

    if(!cpuInUse && time == decisionTime && readyQ.size() > 0)
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
    if(!cpuInUse && time == startNextProcess)
    {
      burst_end = time + all_p[cp].getBurstTime();
      cpuInUse = true; //CPU is now in use
      cout << "time " << time << "ms: Process " << all_p[cp].getID() << " started using the CPU for " << all_p[cp].getBurstTime() << "ms burst ";
      printQ(readyQ);
    }

    time++;
  }
  cout << "time " << time+(t_cs/2)-1 << "ms: Simulator ended for SJF ";
  printQ(readyQ);

  // calculations for avg algorithm stats
  float total_turn_around_time = 0;
  float total_burst_times = 0;
  float total_wait_time = 0;

  int context_switches = 0;

  float avg_tat = 0.0; // average turn around time
  float avg_bt = 0.0; // average burst time
  float avg_wt = 0.0; // average wait time

}

void FCFS(vector < Process > all_p, int n, int switch_time)
{
  int t_cs = switch_time;
  unsigned int time = 0;

  vector < Process > readyQ; //Waiting processes get added here
  vector < Process > serviceQ; //Finished processes get added here

  bool cpuInUse = false; //Set to true only when the CPU is being used
  bool firstProcessArrived = false;

  //For keeping track of various important times in the CPU burst process
  int burst_end = -1;
  int startNextIO = -1;
  int startNextProcess = -1;
  int returnTime = -1;

  int cp; //Holds currently bursting process
  int ip; //Holds current process in IO

  //Initial output
  for (int i = 0; i < all_p.size(); i++)
  {
    cout << "Process " << all_p[i].getID() << " [NEW] (arrival time " << all_p[i].getArrivalTime() << " ms) " << all_p[i].getNumBursts() << " CPU bursts" << endl;
  }

  cout << "time " << time << "ms: Simulator started for FCFS ";
  printQ(readyQ);

  //Iterates until all processes have been added to serviceQ
  while (serviceQ.size() != all_p.size())
  {
    //Iterates over each process to see if anything arrives at this time
    for (int i = 0; i < all_p.size(); i++)
    {
      //Check to see if i-th process arrives *now*
      if (all_p[i].getArrivalTime() == time)
      {
        readyQ.push_back(all_p[i]);
        cout << "time " << time << "ms: Process " << all_p[i].getID() << " arrived; added to ready queue ";
        printQ(readyQ);
        if (!firstProcessArrived)
        {
          firstProcessArrived = true;
          startNextProcess = time + (t_cs / 2);
          cp = i;
        }
      }

      //Check to see if i-th process returns from I/O *now*
      if (all_p[i].getBlockedUntil() == time)
      {
        readyQ.push_back(all_p[i]);
        cout << "time " << time << "ms: Process " << all_p[i].getID() << " completed I/O; added to ready queue ";
        printQ(readyQ);
        all_p[i].decreaseIOBursts();
        all_p[i].resetIOBurst();
      }
    }

    //Current CPU Burst ended
    if (cpuInUse && burst_end == time)
    {
      all_p[cp].decreaseCPUBursts();

      //Process has no more remaining bursts, service the process
      if (all_p[cp].getNumBursts() == 0)
      {
        all_p[cp].setServiced();
        all_p[cp].addContextSwitch();
        serviceQ.push_back(all_p[cp]);
        cout << "time " << time << "ms: Process " << all_p[cp].getID() << " terminated ";
        printQ(readyQ);
        if(readyQ.size() > 0)
        {
          startNextProcess = time + t_cs; //next process starts at this time
        }
        cpuInUse = false;
      }

      //Current burst gets sent to IO, CPU use disabled
      else if (all_p[cp].getNumBursts() > 0)
      {
        cout << "time " << time << "ms: Process " << all_p[cp].getID() << " completed a CPU burst; " << all_p[cp].getNumBursts() << " bursts to go ";
        printQ(readyQ);
        all_p[cp].resetCPUBurst();
        all_p[cp].addContextSwitch(); //increment context switch count for this process
        int returnTime = time + (t_cs/2) + all_p[cp].getIOTime();
        all_p[cp].setBlockedUntil(returnTime);
        cout << "time " << time << "ms: Process " << all_p[cp].getID() << " switching out of CPU; will block on I/O until time " << all_p[cp].getBlockedUntil() << "ms ";
        printQ(readyQ);
        if(readyQ.size() > 0)
        {
          startNextProcess = time + t_cs; //next process starts at this time
        }
        cpuInUse = false; //CPU is not in use anymore
      }
    }


    if (!cpuInUse && startNextProcess < time && readyQ.size() > 0)
    {
      startNextProcess = time + (t_cs / 2);
    }

    //CPU ready to accept frontmost process from the readyQ
    if (!cpuInUse && time == startNextProcess)
    {
      for (int i = 0; i < all_p.size(); i++)
      {
        if (all_p[i].getID() == readyQ[0].getID())
        {
          cp = i;
          break;
        }
      }
      readyQ.erase(readyQ.begin()); //remove the process from the readyQ

      burst_end = time + all_p[cp].getBurstTime();
      cpuInUse = true; //CPU is now in use
      cout << "time " << time << "ms: Process " << all_p[cp].getID() << " started using the CPU for " << all_p[cp].getBurstTime() << "ms burst ";
      printQ(readyQ);
    }
    time++;
  }

  cout << "time " << time+(t_cs/2)-1 << "ms: Simulator ended for FCFS ";
  printQ(readyQ);

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
    int decisionTime = -1; //due to push-front behavior, make decision at burst_end + t_cs/2 for next process
    int burst_end = -1; // acts as marker for when next context switch/preemption should occur

    int cp; // holds ID of current process being serviced

    deque<Process> readyQ;
    deque<Process> serviceQ;

    // Begin algorithm simulation
    cout << "time " << time << "ms: Simulator started for RR ";
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
                cout << "time " << time << "ms: Process " << all_p[i].getID() << " arrived; added to ready queue ";
                if(rradd == "BEGINNING")
                {
                  readyQ.push_front(all_p[i]);
                }
                else
                {
                  readyQ.push_back(all_p[i]);
                }
                printQ_RR(readyQ);

                //only applies to the first process to arrive
                if(!firstProcessArrived)
                {
                  firstProcessArrived = true;
                  decisionTime = time;
                  startNextProcess = time + (t_cs/2); //first process just arrived, start loading into CPU and start executing at t_cs/2
                  cp = i;
                }
            }
            // Check if any processes come back from I/O at this time
            if(all_p[i].getBlockedUntil() == time)
            {
              cout << "time " << time << "ms: Process " << all_p[i].getID() <<" completed I/O; added to ready queue ";
              if(rradd == "BEGINNING")
              {
                readyQ.push_front(all_p[i]);
              }
              else
              {
                readyQ.push_back(all_p[i]);
              }
              printQ_RR(readyQ);
              all_p[i].decreaseIOBursts();//another IO bursts completed, reduce number of bursts needed
              all_p[i].resetIOBurst();
            }
        }

        //CPU currently in use but there is an event that needs handling
        if(cpuInUse && time == burst_end)
        {
          //no context switch required
          if(readyQ.size() == 0)
          {
            //CPU burst finished
            if(all_p[cp].getRemainingTimeInBurst() == 0)
            {
              all_p[cp].decreaseCPUBursts(); //another CPU burst finished, decrement the count
              if(all_p[cp].getNumBursts() == 0)
              {
                //process has finished executing, setServiced
                all_p[cp].setServiced();
                serviceQ.push_back(all_p[cp]);
                all_p[cp].addContextSwitch();
                cout << "time " << time << "ms: Process " << all_p[cp].getID() << " terminated ";
                printQ_RR(readyQ);
              }
              else //send process to IO
              {
                cout << "time " << time << "ms: Process " << all_p[cp].getID() << " completed a CPU burst; " << all_p[cp].getNumBursts() << " bursts to go ";
                printQ_RR(readyQ);
                all_p[cp].resetCPUBurst();
                all_p[cp].resetPreempted();
                all_p[cp].setRemainingTimeInBurst(all_p[cp].getBurstTime());
                all_p[cp].addContextSwitch();
                int returnTime = time + (t_cs/2) + all_p[cp].getIOTime();
                all_p[cp].setBlockedUntil(returnTime);
                cout << "time " << time << "ms: Process " << all_p[cp].getID() << " switching out of CPU; will block on I/O until time " << all_p[cp].getBlockedUntil() << "ms ";
                printQ_RR(readyQ);
              }
              cpuInUse = false; //free up cpu from use so next event can occur
            }
            //Remaining time might be less than t_slice
            else if(all_p[cp].getRemainingTimeInBurst() <= t_slice)
            {
              burst_end = time + all_p[cp].getRemainingTimeInBurst();
              all_p[cp].setRemainingTimeInBurst(0);
            }
            //extend by t_slice with no preemption
            else
            {
              burst_end = time + t_slice;
              int difference = all_p[cp].getRemainingTimeInBurst() - t_slice;
              all_p[cp].setRemainingTimeInBurst(difference);
            }
          }

          //context switch is required
          else
          {
            if(all_p[cp].getRemainingTimeInBurst()==0)//CPU burst finished
            {
              all_p[cp].decreaseCPUBursts();
              if(all_p[cp].getNumBursts()==0)
              {
                //process just finished CPU burst, has no more CPU or IO bursts remaining
                //process is serviced
                all_p[cp].setServiced();
                all_p[cp].addContextSwitch();
                serviceQ.push_back(all_p[cp]);
                cout << "time " << time << "ms: Process " << all_p[cp].getID() << " terminated ";
                printQ_RR(readyQ);
              }
              else
              {
                //send process to IO
                cout << "time " << time << "ms: Process " << all_p[cp].getID() << " completed a CPU burst; " << all_p[cp].getNumBursts() << " bursts to go ";
                printQ_RR(readyQ);
                all_p[cp].resetCPUBurst();
                all_p[cp].resetPreempted();
                all_p[cp].setRemainingTimeInBurst(all_p[cp].getBurstTime());
                all_p[cp].addContextSwitch(); //increment context switch count for this process
                int returnTime = time + (t_cs/2) + all_p[cp].getIOTime();
                all_p[cp].setBlockedUntil(returnTime);
                cout << "time " << time << "ms: Process " << all_p[cp].getID() << " switching out of CPU; will block on I/O until time " << all_p[cp].getBlockedUntil() << "ms ";
                printQ_RR(readyQ);
              }
            }
            //current process needs more CPU time before IO
            else
            {
              all_p[cp].addContextSwitch();
              all_p[cp].addPreemptedCount();
              cout << "time " << time << "ms: Time slice expired; process " << all_p[cp].getID() << " preempted with " << all_p[cp].getRemainingTimeInBurst() << "ms to go ";
              printQ_RR(readyQ);
              if(rradd == "BEGINNING")
              {
                readyQ.push_front(all_p[cp]);
              }
              else
              {
                readyQ.push_back(all_p[cp]);
              }
            }

            decisionTime = time + (t_cs/2);
            startNextProcess = time + t_cs;
            cpuInUse = false;
          }
        }

        if(!cpuInUse && startNextProcess < time && readyQ.size()>0)
        {
          //needed for when no processes waiting in readyQ for a while, all were in IO or waiting ot arrive at one point
          //startNextProcess needs to be reassigned and new current process needed to be chosen
          startNextProcess = time + (t_cs/2);
          decisionTime = time;
        }

        //cpu not in use since last burst ended, decide next process to start loading in
        if(!cpuInUse && time == decisionTime && readyQ.size() != 0)
        {
          //assign this as our current process to start loading in
          for(int i = 0; i < all_p.size(); i++)
          {
            if(all_p[i].getID() == readyQ[0].getID())
            {
              cp = i;
              break;
            }
          }
          readyQ.erase(readyQ.begin());
        }

        //CPU not in use, it's time to load in a new process, and readyQ is not empty
        if(!cpuInUse && time == startNextProcess)
        {
          if(all_p[cp].getID() == readyQ[0].getID())
          {
            readyQ.erase(readyQ.begin());
          }
          //burst will finish before or at the same time that timeslice expires
          if(all_p[cp].getRemainingTimeInBurst() <= t_slice)
          {
            burst_end = time + all_p[cp].getRemainingTimeInBurst();
            if(all_p[cp].getPreempted())
            {
              cout << "time " << time << "ms: Process " << all_p[cp].getID() << " started using the CPU with " << all_p[cp].getRemainingTimeInBurst() << "ms remaining ";
              printQ_RR(readyQ);
            }
            else
            {
              cout << "time " << time << "ms: Process " << all_p[cp].getID() << " started using the CPU for " << all_p[cp].getRemainingTimeInBurst() << "ms burst ";
              printQ_RR(readyQ);
            }
            all_p[cp].setRemainingTimeInBurst(0); //this way we know to send process to IO at burst end
          }
          //timeslice will expire first, setRemainingTimeInBurst used to save the difference
          else
          {
            burst_end = time + t_slice;
            int difference = all_p[cp].getRemainingTimeInBurst() - t_slice;
            all_p[cp].setRemainingTimeInBurst(difference);
            cout << "time " << time << "ms: Process " << all_p[cp].getID() << " started using the CPU for " << t_slice << "ms burst ";
            printQ_RR(readyQ);
          }
          all_p[cp].addContextSwitch(); //switch from waitQ to CPU happened
          cpuInUse = true;
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

    cout <<"time " << time+(t_cs/2)-1 << "ms: Simulator ended for RR ";
    printQ_RR(readyQ);

    avg_tat = total_turn_around_time / float(context_switches);
    avg_bt = total_burst_times / float(context_switches);
    avg_wt = total_wait_time / float(context_switches);
}

void SRT(vector <Process> p, int n, int t_cs)
{
  vector < Process > all_p = p; //Copy of the passed in process vector
  vector < Process > readyQ; //Waiting processes get added here
  vector < Process > serviceQ; //Finished processes get added here

  bool cpuInUse = false; //Set to true only when the CPU is being used
  bool firstProcessArrived = false;
  unsigned int time = 0; // overall timer for simulation
  int startNextProcess = -1; // By default first process should not start before this time
  int decisionTime = -1; //due to push-front behavior, make decision at burst_end + t_cs/2 for next process
  int burst_end = -1; // acts as marker for when next context switch/preemption should occur

  int cp; //To hold the data for the current bursting process
  float alpha = 0.5;

  //Initial output
  for(int i = 0; i < all_p.size(); i++)
  {
    cout << "Process " << all_p[i].getID() << " [NEW] (arrival time " << all_p[i].getArrivalTime() << " ms) " << all_p[i].getNumBursts() << " CPU bursts" << endl;
  }

  cout << "time " << time << "ms: Simulator started for SRT ";
  printQ(readyQ);

  //Iterates until all processes have been added to serviceQ
  while(serviceQ.size() != all_p.size())
  {
    // Starting CPU
    if(!cpuInUse && time == startNextProcess)
    {
      cpuInUse = true;
      if(all_p[cp].getPreempted())
      {
        cout << "time " << time << "ms: Process " << all_p[cp].getID() << " started using CPU with " << all_p[cp].getRemainingTimeInBurst() << "ms remaining ";
        printQ(readyQ);
      }
      else
      {
        cout << "time " << time << "ms: Process " << all_p[cp].getID() << " started using CPU for " << all_p[cp].getRemainingTimeInBurst() << "ms burst ";
        printQ(readyQ);
      }
      all_p[cp].addContextSwitch();
    }

    //Iterates over each process to see if anything arrives at this time
    for(int i = 0; i < all_p.size(); i++)
    {
      //Check to see if i-th process arrives *now*
      if(all_p[i].getArrivalTime() == time)
      {
        readyQ.push_back(all_p[i]);
        sort(readyQ.begin(), readyQ.end(), sortHelper);
        cout << "time " << time << "ms: Process " << all_p[i].getID() << " (tau " << all_p[i].getTau() << "ms) arrived; added to ready queue ";
        printQ(readyQ);

        //Signifies the first process has arrived
        //Necessary because nothing preempts the first process
        //only applies to the first process to arrive
        if (!firstProcessArrived)
        {
          firstProcessArrived = true;
          decisionTime = time;
          startNextProcess = time + (t_cs / 2); //first process just arrived, start loading into CPU and start executing at t_cs/2
          cp = i;
        }
      }

      //Check to see if i-th process returns from I/O *now*
      if(all_p[i].getBlockedUntil() == time)
      {
        readyQ.push_back(all_p[i]);
        sort(readyQ.begin(), readyQ.end(), sortHelper);
        all_p[i].decreaseIOBursts(); //another IO bursts completed, reduce number of bursts needed
        all_p[i].resetIOBurst();
        if(cpuInUse && all_p[i].getRemainingTimeInBurst() < all_p[cp].getRemainingTimeInBurst())
        {
          decisionTime = time + t_cs/2;
          cpuInUse = false;
          all_p[cp].addPreemptedCount();
          all_p[cp].addContextSwitch();
          startNextProcess = time + t_cs;
          cout << "time " << time << "ms: Process " << all_p[i].getID() << " (tau " << all_p[i].getTau() << "ms) completed I/O and will preempt " << all_p[cp].getID() <<" ";
          printQ(readyQ);
          readyQ.push_back(all_p[cp]);
        }
        else
        {
          cout << "time " << time << "ms: Process " << all_p[i].getID() << " (tau " << readyQ[0].getTau() << "ms) completed I/O; added to ready queue ";
          printQ(readyQ);
        }
      }
    }

    if(!cpuInUse && startNextProcess < time && readyQ.size() > 0)
    {
      //needed for when no processes waiting in readyQ for a while, all were in IO or waiting ot arrive at one point
      //startNextProcess needs to be reassigned and new current process needed to be chosen
      startNextProcess = time + (t_cs/2);
      decisionTime = time;
    }

    if(!cpuInUse && time == decisionTime && readyQ.size() != 0)
    {
      //assign this as our current process to start loading in
      for(int i = 0; i < all_p.size(); i++)
      {
        if(all_p[i].getID() == readyQ[0].getID())
        {
          cp = i;
          break;
        }
      }
      readyQ.erase(readyQ.begin());
    }

    if(cpuInUse)
    {
      //If readyQ front will preempt the current process
      if(readyQ[0].getRemainingTimeInBurst() < all_p[cp].getRemainingTimeInBurst())
      {
        decisionTime = time + t_cs/2;
        startNextProcess = time + t_cs;
        cpuInUse = false;
        cout << "time " << time << "ms: Process " << readyQ[0].getID() << " (tau " << readyQ[0].getTau() << "ms) will preempt " << all_p[cp].getID() << " ";
        printQ(readyQ);
        readyQ.push_back(all_p[cp]);
      }
      else
      {
        if(time != startNextProcess)
        {
          int newTime = all_p[cp].getRemainingTimeInBurst() - 1;
          all_p[cp].setRemainingTimeInBurst(newTime);
        }
        if(all_p[cp].getRemainingTimeInBurst() == 0)
        {
          all_p[cp].decreaseCPUBursts();

          if(readyQ.size() > 0)
          {
            decisionTime = time + (t_cs/2);
            startNextProcess = time + t_cs;
          }
          if(all_p[cp].getNumBursts() == 0)
          {
            //process has finished executing, setServiced
            all_p[cp].setServiced();
            serviceQ.push_back(all_p[cp]);
            all_p[cp].addContextSwitch();
            cout << "time " << time << "ms: Process " << all_p[cp].getID() << " terminated ";
            printQ(readyQ);
          }
          else
          {
            cout << "time " << time << "ms: Process " << all_p[cp].getID() << " completed a CPU burst; " << all_p[cp].getNumBursts() << " bursts to go ";
            printQ(readyQ);
            int newTau = (alpha*all_p[cp].getBurstTime()) + ((1-alpha)*all_p[cp].getTau());
            //newTau = newTau + 1;
            all_p[cp].setTau(newTau);
            cout << "time " << time << "ms: Recalculated tau = " << newTau << "ms for process " << all_p[cp].getID() << " ";
            printQ(readyQ);
            all_p[cp].resetCPUBurst();
            all_p[cp].resetPreempted();
            all_p[cp].setRemainingTimeInBurst(all_p[cp].getBurstTime());
            all_p[cp].addContextSwitch();
            int returnTime = time + (t_cs/2) + all_p[cp].getIOTime();
            all_p[cp].setBlockedUntil(returnTime);
            cout << "time " << time << "ms: Process " << all_p[cp].getID() << " switching out of CPU; will block on I/O until time " << all_p[cp].getBlockedUntil() << "ms ";
            printQ(readyQ);
          }
          cpuInUse = false;
        }
      }
    }
    time++;
  }
  cout << "time " << time+(t_cs/2)-1 << "ms: Simulator ended for SRT ";
  printQ(readyQ);

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
  int t_cs = 4;
  int timeslice = 120;
  string rradd = "END";

  vector<Process> processes;
  processes = process_helper();
  SJF(processes, n, t_cs);
  cout << endl;
  //processes = process_helper();
  //SRT(processes, n, t_cs);
  processes = process_helper();
  FCFS(processes, n, t_cs);
  cout << endl;
  processes = process_helper();
  RR(processes, n, t_cs, timeslice, rradd);
  cout << endl;
  return 0;
}
