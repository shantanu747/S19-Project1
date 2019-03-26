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
                  decisionTime = time;
                  startNextProcess = time + (t_cs/2); //first process just arrived, start loading into CPU and start executing at t_cs/2
                  cp = i;
                }
                printQ_RR(readyQ);
            }
            // Check if any processes come back from I/O at this time
            else if(all_p[i].getBlockedUntil() == time)
            {
              cout << "time " << time << "ms: Process " << all_p[i].getID() << " finished I/O and added to ready queue ";
              all_p[i].decreaseIOBursts();//another IO bursts completed, reduce number of bursts needed
              all_p[i].resetIOBurst();
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
                printQ(readyQ);
              }
              else //send process to IO
              {
                cout << "time " << time << "ms: Process " << all_p[cp].getID() << " completed a CPU burst; " << all_p[cp].getNumBursts() << " bursts to go ";
                printQ(readyQ);
                all_p[cp].resetCPUBurst();
                all_p[cp].addContextSwitch();
                int returnTime = time + all_p[cp].getIOTime();
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

        if(!cpuInUse && startNextProcess < time && readyQ.size()>0)
        {
          //needed for when no processes waiting in readyQ for a while, all were in IO or waiting ot arrive at one point
          //startNextProcess needs to be reassigned and new current process needed to be chosen
          startNextProcess = time + (t_cs/2);
          decisionTime = time;
        }

        //cpu not in use since last burst ended, decide next process to start loading in
        else if(!cpuInUse && time == decisionTime && readyQ.size() != 0)
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

void FCFS(vector<Process> all_p, int n, int switch_time)
{
    int t_cs = switch_time;
    unsigned int time = 0;

    vector<Process> all_processes = all_p; //Copy of the passed in process vector
    vector<Process> readyQ; //Waiting processes get added here
    //readyQ.size();
    vector<Process> serviceQ; //Finished processes get added here
    //serviceQ.size();

    bool cpuInUse = false; //Set to true only when the CPU is being used
    bool firstProcessArrived = false;

    //For keeping track of various important times in the CPU burst process
    int burstEnd, startNextIO, startNextProcess, returnTime;

    int currentProcess; //Holds currently bursting process
    int IOProcess; //Holds current process in IO

    //Initial output
    for(int i = 0; i < all_processes.size(); i++)
    {
      cout << "Process " << all_processes[i].getID() << " [NEW] (arrival time " << all_processes[i].getArrivalTime() << " ms) " << all_processes[i].getNumBursts() << " CPU bursts" << endl;
    }
    cout << "\n";

    cout << "time " << time << "ms: Simulator started for FCFS ";
    printQ(readyQ);

    //Iterates until all processes have been added to serviceQ
    while(serviceQ.size() != all_processes.size())
    {
      cout << "Penetraded the while loop" << endl;
        //Iterates over each process to see if anything arrives at this time
        for(int i = 0; i < all_processes.size(); i++)
        {
            //Check to see if i-th process arrives *now*
            if(all_processes[i].getArrivalTime() == time)
            {
                readyQ.push_back(all_processes[i]);
                cout << "time " << time << "ms: Process " << all_processes[i].getID() << " arrived; added to ready queue ";
                printQ(readyQ);
                if(!firstProcessArrived)
                {
                  firstProcessArrived = true;
                  startNextProcess = time + (t_cs/2);
                }
            }

            //Check to see if i-th process returns from I/O *now*
            if(all_processes[i].getBlockedUntil() == time)
            {
                // Check if any processes come back from I/O at this time
                readyQ.push_back(all_processes[i]);
                cout << "time " << time << "ms: Process " << all_processes[i].getID() << " completed I/O; added to ready queue ";
                printQ(readyQ);
            }
        }

        //Current CPU Burst ended, send process to IO
        if(cpuInUse && burstEnd == time)
        {
            all_processes[currentProcess].decreaseCPUBursts();
            cout << "time " << time << "ms: Process " << all_processes[currentProcess].getID() << " completed a CPU burst; " << all_processes[currentProcess].getNumBursts() << " bursts to go ";
            printQ(readyQ);

            //Process has no more remaining bursts, service the process
            if(all_processes[currentProcess].getNumBursts()==0)
            {
                all_processes[currentProcess].setServiced();
                serviceQ.push_back(all_processes[currentProcess]);
                cout << "time " << time << "ms: Process " << all_processes[currentProcess].getID() << " terminated ";
                printQ(readyQ);
            }

            //Current burst gets sent to IO, CPU use disabled
            else if(all_processes[currentProcess].getNumBursts() > 0)
            {
                startNextIO = time + (t_cs/2); //time needed to exit CPU
                startNextProcess = time + t_cs; //next process starts at this time
                IOProcess = currentProcess;
                returnTime = startNextIO + all_processes[IOProcess].getIOTime();
                all_processes[IOProcess].setBlockedUntil(returnTime);
                all_processes[IOProcess].addContextSwitch(); //increment context switch count for this process
                cpuInUse = false; //CPU is not in use anymore
            }
        }

        //CPU ready to accept frontmost process from the readyQ
        else if(!cpuInUse && time == startNextProcess)
        {
            for(int i = 0; i < all_processes.size(); i++)
            {
              if(all_processes[i].getID() == readyQ[0].getID())
              {
                currentProcess = i;
                break;
              }
            }
            readyQ.erase(readyQ.begin()); //remove the process from the readyQ

            burstEnd = time + all_processes[currentProcess].getBurstTime();
            cpuInUse = true; //CPU is now in use
            cout << "time " << time << "ms: Process " << all_processes[currentProcess].getID() << " started using the CPU for " << all_processes[currentProcess].getBurstTime() << "ms burst ";
            printQ(readyQ);
        }

        //output message saying process is now in IO
        if(time == startNextIO)
        {
            cout << "time " << time << "ms: Process " << all_processes[IOProcess].getID() << " switching out of CPU; will block on I/O until time " << all_processes[IOProcess].getBlockedUntil() << "ms ";
            printQ(readyQ);
        }

        if(!cpuInUse && startNextProcess < time && readyQ.size() != 0)
        {
          startNextProcess = time + (t_cs/2);
        }

        time++;
    }



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
