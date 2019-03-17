//  process.cpp
//
//  Process class implementation

#include "process.h"
using namespace std;

Process::Process() // default constructor
{
    pid = "";
    arrivalTime = 0;
    burstTime = 0;
    numBursts =  0;
    ioTime = 0;
    blockedUntil = 0;
    serviced = false;
    turnaroundTime = 0;
    waitTime = 0;
}

Process::Process(string gid, int arrival, int burst, int bcount, int io)
{
    pid = gid;
    arrivalTime = arrival;
    burstTime = burst;
    numBursts =  bcount;
    ioTime = io;
    blockedUntil = 0;
    serviced = false;
    turnaroundTime = 0;
    waitTime = 0;
    timeRemainingInBurst = burst;
    setTimeNeeded();
    totalRemainingTime = timeNeeded;
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

int Process::getBlockedUntil() const
{
    return blockedUntil;
}

int Process::getRemainingTimeInBurst() const
{
    return timeRemainingInBurst;
}

int Process::getTotalRemainingTime() const
{
    return totalRemainingTime;
}

bool Process::getServiced() const
{
    return serviced;
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

void Process::decreaseBurst()
{
    numBursts--;
    totalRemainingTime -= burstTime;
}

void Process::setServiced()
{
    serviced = true;
}

void Process::setTurnaroundTime(int t)
{
    turnaroundTime = t;
}

/* DEPRECATED
void Process::setTimeNeeded()
{
    timeNeeded = (burstTime * numBursts);
}
*/
