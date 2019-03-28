//  process.cpp
//
//  Process class implementation
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <string>
#include <deque>
#include <iomanip>
#include <algorithm>
#include "process.h"
#include <cmath>
using namespace std;

Process::Process() // default constructor
{
    arrivalTime = 0;
    burstTime = 0;
    numBursts =  0;
    ioTime = 0;
    numIO = 0;
    cpuTime = 0;
    timeInIO = 0;
    blockedUntil = -1;
    serviced = false;
    turnaroundTime = 0;
    waitTime = 0;
    timeRemainingInBurst = 0;
    preemptedCount = 0;
    contextSwitchCount = 0;
}

Process::Process(char gid, int arrival, vector<int> burstTimes, int bcount, vector<int> ioTimer, int ioCount, float lambda)
{
    pid = gid;
    arrivalTime = arrival;
    burstTime = burstTimes[0];
    burstTimes.erase(burstTimes.begin());
    numBursts = bcount;
    if(ioTimer.size() > 0)
    {
      ioTime = ioTimer[0];
      ioTimer.erase(ioTimer.begin());
    }
    else
    {
      ioTime = 0;
    }
    numIO = ioCount; //1 less I/O burst needed than CPU bursts
    cpuTime = burstTime*numBursts;
    timeInIO = ioTime*numIO; //total time that will be spent in IO
    blockedUntil = -1; //doesn't trigger at time 0
    serviced = false;
    turnaroundTime = 0;
    waitTime = 0;
    timeRemainingInBurst = burstTime;
    preemptedCount = 0;
    contextSwitchCount = 0;
    int iTau = ceil(1/lambda);
    tau = int(iTau);
    gotPreempted = false;

    ioTimes = ioTimer;
    cpuTimes = burstTimes;

    cpuTime = 0;
    for(unsigned int i = 0; i < burstTimes.size(); i++)
    {
      cpuTime += burstTimes[i];
    }
}

// ACCESSORS
char Process::getID() const
{
    return pid;
}

int Process::getArrivalTime() const
{
    return arrivalTime;
}

int Process::getBurstTime() const
{
    return burstTime;
}

int Process::getNumBursts() const
{
    return numBursts;
}

int Process::getIOTime() const
{
    return ioTime;
}

int Process::getIOBursts() const
{
  return numIO;
}

int Process::getBlockedUntil() const
{
    return blockedUntil;
}

int Process::getRemainingTimeInBurst() const
{
    return timeRemainingInBurst;
}

bool Process::getServiced() const
{
    return serviced;
}

int Process::getContextSwitchCount() const
{
  return contextSwitchCount;
}

int Process::getWaitTime() const
{
  return waitTime;
}

int Process::getPreemptedCount() const
{
  return preemptedCount;
}

int Process::getTau() const
{
  return tau;
}

bool Process::getPreempted() const
{
  return gotPreempted;
}

int Process::getTurnaroundTime() const
{
  return turnaroundTime;
}

int Process::getCPUTime() const
{
  return cpuTime;
}


//MODIFIERS
void Process::setBlockedUntil(int b)
{
    blockedUntil = b;
}

void Process::setRemainingTimeInBurst(int b)
{
    timeRemainingInBurst = b;
}

void Process::decreaseCPUBursts()
{
    numBursts -= 1;
}

void Process::decreaseIOBursts()
{
  numIO -= 1;
}

void Process::setServiced()
{
    serviced = true;
}

void Process::setTurnaroundTime(int t)
{
    turnaroundTime = t;
}

void Process::addContextSwitch()
{
  contextSwitchCount += 1;
}

void Process::setWaitTime(int w)
{
  waitTime += w;
}

void Process::addPreemptedCount()
{
  preemptedCount += 1;
  gotPreempted = true;
}

void Process::setTau(int t)
{
  tau = t;
}

void Process::resetCPUBurst()
{
  if(cpuTimes.size()>0){
    burstTime = cpuTimes[0];
    cpuTimes.erase(cpuTimes.begin());
  }
  else{
    burstTime = 0;
  }
}

void Process::resetIOBurst()
{
  if(ioTimes.size()>0){
    ioTime = ioTimes[0];
    ioTimes.erase(ioTimes.begin());
  }
  else{
    ioTime = 0;
  }
}

void Process::resetPreempted()
{
  gotPreempted = false;
}
