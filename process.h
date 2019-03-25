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
    Process(char gid, int arrival, int burst, int bcount, int io, float lambda);

    //Accessors
    char getID() const; 
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
    int getTau() const;

    //Modifiers
    void setBlockedUntil(int b); //tells us when to bring process back from I/O
    void setRemainingTimeInBurst(int b); //required for pre-emptive algorithms
    void decreaseCPUBursts(); //reduce number of CPU bursts needed by 1
    void decreaseIOBursts(); //reduce number of IO bursts needed by 1
    void setServiced(); //process has finished executing
    void setTurnaroundTime(int t);
    void addContextSwitch();
    void setWaitTime(int w);
    void addPreemptedCount();
    void setTau(int t);

private:
    char pid;
    int arrivalTime;
    int burstTime; //how long each burst lasts
    int numBursts; //number of CPU bursts
    int ioTime; //how long each IO operation takes
    int numIO; //number of IO bursts needed
    int timeInIO; // total time spent in IO
    int cpuTime; // total time in CPU
    int blockedUntil;
    int tau;
    bool serviced;

    int turnaroundTime;
    int waitTime;
    int timeRemainingInBurst;
    int preemptedCount;
    int contextSwitchCount;
};

#endif /* process_h */
