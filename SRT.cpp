#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <deque>
#include <iomanip>
#include "process.h"

using namespace std;
      // process vector | num inputs
void srt(vector<Process> p, int n){

    //Convert input process vector into a deque
    deque<Process> all_processes;
    for(int i =0; i < p.size(); i++){
        all_processes.push_back(p[i]);
    }

}