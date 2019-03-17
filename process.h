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
    int getBlockedUntil() const;
    int getRemainingTimeInBurst() const;
    int getTotalRemainingTime() const;
    bool getServiced() const;

    //Modifiers
    void setBlockedUntil(int b); //tells us when to bring process back from I/O
    void setRemainingTimeInBurst(int b); //required for pre-emptive algorithms
    void decreaseBurst();
    void setServiced(); //process has finished executing
    void setTurnaroundTime(int t);
    //void setTimeNeeded(); /* deprectaed */

private:
    string pid;
    int arrivalTime;
    int burstTime;
    int numBursts;
    int ioTime;
    int timeNeeded;
    int blockedUntil;
    int totalRemainingTime;
    bool serviced;

    int turnaroundTime;
    int waitTime;
    int timeRemainingInBurst;
    int preemptedTime;
};

#endif /* process_h */
