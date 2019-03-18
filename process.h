//  process.h
//
//  Header file for Process class implementation

#include <string>
using namespace std;

#ifndef _process_h_
#define _process_h_

class Process
{
public:
    Process();
    Process(string gid, int arrival, int burst, int bcount, int io);

    //Accessors
    string getID() const;
    int getArrivalTime() const;
    int getBurstTime() const;
    int getNumBursts() const;
    int getIOTime() const;
    int getIOBursts() const;
    int getBlockedUntil() const;
    int getRemainingTimeInBurst() const;
    bool getServiced() const;
    int getContextSwitchCount() const;
    int getWaitTime() const;
    int getPreemptedCount() const;

    //Modifiers
    void setBlockedUntil(int b); //tells us when to bring process back from I/O
    void setRemainingTimeInBurst(int b); //required for pre-emptive algorithms
    void decreaseCPUBurst();
    void decreaseIOBursts();
    void setServiced(); //process has finished executing
    void setTurnaroundTime(int t);
    void addContextSwitch();
    void addWaitTime(int w);
    void addPreemptedCount();

private:
    string pid;
    int arrivalTime;
    int burstTime; //how long each burst lasts
    int numBursts; //number of CPU bursts
    int ioTime; //how long each IO operation takes
    int numIO;
    int timeInIO;
    int timeNeeded;
    int blockedUntil;
    bool serviced;

    int cpuTime;
    int turnaroundTime;
    int waitTime;
    int timeRemainingInBurst;
    int preemptedCount;
    int contextSwitchCount;
};

#endif /* process_h */
