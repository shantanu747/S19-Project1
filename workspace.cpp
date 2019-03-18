// check current conditions and service next process if everything checks out
if  ((!cpu_in_use)
     && time == startNextProcess
     && (readyQ.size() != 0))
{

    cout << "time " << time << "ms: Process " << curent_process << " started using the CPU ";
    if (all_p[current_process_index].getRemainingTimeInBurst() != all_p[current_process_index].get_burst_time()){
        cout << "with " << all_p[current_process_index].getRemainingTimeInBurst() <<"ms remaining ";

    }
    cpu_in_use = true;
    burst_start = time  ;

    if (all_p[current_process_index].getRemainingTimeInBurst() <= t_slice)
        // process will end before t_slice expires
    {

        burst_end = burst_start + all_p[current_process_index].getRemainingTimeInBurst();
        all_p[current_process_index].set_remaining_time_in_current_burst(0);
    }
    else {
        burst_end = burst_start + t_slice ;

        int tmp = (all_p[current_process_index].getRemainingTimeInBurstgetRemainingTimeInBurst()) - t_slice;
        all_p[current_process_index].set_remaining_time_in_current_burst(tmp);
    }
    readyQ.erase(readyQ.begin());
    if (readyQ.size() != 0){
        cout << " ";
    }
    printQ(readyQ);
}

// Check if any processes finished an I/0 brust, return it to ready
for (int i =0; i < all_p.size() ; i++ )
{
    if (all_p[i].get_blocked_until() != 0 && all_p[i].get_blocked_until() == time && time != burst_end){
        if (all_p[i].get_burst_count() == 0 ) // process finish
        {
           // cout << "time " << time << "ms: Process " << all_p[i].get_id() << " terminated ";
           /* all_p[i].set_serviced();
            int turnaround = time -all_p[i].get_arrival_time();
            all_p[i].set_turn_around_time(turnaround);
            serviceQ.push_back(all_p[i]);*/
        }
        else { // more bursts needed

            cout << "time " << time << "ms: Process " << all_p[i].get_id() << " completed I/O; added to ready queue " ;
            startNextProcess = time + 3;
            readyQ.push_back(all_p[i]);
            all_p[i].set_remaining_time_in_current_burst(all_p[i].get_burst_time());
            printQ(readyQ);
        }

    }
}

// burst_end reached
if(time == burst_end && (burst_end != 0))
{
    context_switches++;
    if(all_p[current_process_index].getRemainingTimeInBurst() == 0 && all_p[current_process_index].get_burst_count() != 0) // send to I/O and bring back to readyQ
    {
        int temp = 0;
        if (all_p[current_process_index].getRemainingTimeInBurst() <= t_slice && all_p[current_process_index].get_burst_count() <2)
        {
            temp = time + all_p[current_process_index].get_io_time();
        }
        else
        {
            temp = time + all_p[current_process_index].get_io_time()+3;
        }

        all_p[current_process_index].set_blocked_until(temp);
        cpu_in_use = false;
        startNextProcess = time + t_cs*2;
        all_p[current_process_index].decrease_bursts();
        all_p[current_process_index].set_remaining_time_in_current_burst(all_p[current_process_index].get_burst_time());
        if (all_p[current_process_index].get_burst_count() == 0){
            cout <<"time " << time << "ms: Process "<< curent_process << " terminated ";
            all_p[current_process_index].set_serviced();
            int turnaround = time -all_p[current_process_index].get_arrival_time();
            all_p[current_process_index].set_turn_around_time(turnaround);
            serviceQ.push_back(all_p[current_process_index]);

        }
        else
        {
            string b;
            if (all_p[current_process_index].get_burst_count() == 1)
            {
                b = "burst";
            }else {b= "bursts";}
            cout << "time " << time << "ms: Process "<< curent_process << " completed a CPU burst; "<< all_p[current_process_index].get_burst_count() << " " << b <<" to go " ;
            printQ(readyQ) ;
            cout <<  "time " <<time << "ms: Process " << all_p[current_process_index].get_id()<< " switching out of CPU; will block on I/O until time ";
            cout << all_p[current_process_index].get_blocked_until() << "ms ";
        }
        preemptions++; // may or may not belong here


        printQ(readyQ);
    }
    else if (all_p[current_process_index].getRemainingTimeInBurst() == 0 && all_p[current_process_index].get_burst_count() == 0){
        cout << "time " << time << "ms: Process " << curent_process << " completed a CPU burst ";
        cpu_in_use = false;
        startNextProcess = time + 2* t_cs;

    }
    else if (readyQ.size() == 0) // no context switch
    {
        cout << "time " << time << "ms: Time slice expired; no preemption because ready queue is empty ";
        printQ(readyQ);

        burst_start = time;

        if (all_p[current_process_index].getRemainingTimeInBurst() <= t_slice)
        {
            burst_end =  time + all_p[current_process_index].getRemainingTimeInBurst();
            all_p[current_process_index].set_remaining_time_in_current_burst(0);
        }
        else
        {

            burst_end = time + t_slice;
            all_p[current_process_index].set_remaining_time_in_current_burst(all_p[current_process_index].getRemainingTimeInBurst()-t_slice);
        }
    }
    else
    {

        cout << "time " << time << "ms: Time slice expired; process " << all_p[current_process_index].get_id() << " preempted with " << all_p[current_process_index].getRemainingTimeInBurst() << "ms to go ";
        startNextProcess = time + t_cs*2; // do not let next process use CPU until this time
        cpu_in_use = false; // CPU is currently not in use anymore
        preemptions++;
        printQ(readyQ);
        readyQ.push_back(all_p[current_process_index]);
    }
}
