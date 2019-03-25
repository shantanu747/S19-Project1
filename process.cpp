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
using namespace std;

Process::Process() // default constructor
{
    pid = "";
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

Process::Process(string gid, int arrival, int burst, int bcount, int io, float lambda)
{
    pid = gid;
    arrivalTime = arrival;
    burstTime = burst;
    numBursts =  bcount;
    ioTime = io;
    numIO = numBursts - 1; //1 less I/O burst needed than CPU bursts
    cpuTime = burstTime*numBursts;
    timeInIO = ioTime*numIO; //total time that will be spent in IO
    blockedUntil = -1; //doesn't trigger at time 0
    serviced = false;
    turnaroundTime = 0;
    waitTime = 0;
    timeRemainingInBurst = burst;
    preemptedCount = 0;
    contextSwitchCount = 0;
    tau = 1/lambda;
}

// ACCESSORS
string Process::getID() const
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
}

void Process::setTau(int t)
{
  tau = t;
}

void Process::printQ(vector<Process> &all)
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
