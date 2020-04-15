#include "process.h"


void Process::Process() :
    process_ID(pid)
    , arrival_time(arr)
    , processed_time(processed)
{
    // initialize turnaround, wait, and burst/io queues
    turnaround_time = 0;
    wait_time = 0;
    burst_times = new std::queue<unsigned int>;
    io_times = new std::queue<unsigned int>;
}

void Process::~Process() {
    // delete burst/io time queues
    delete burst_times;
    delete io_times;
}

void Process::contextSwitch(bool switch_in, unsigned int tcshalf) {
    if (switch_in) {
        wait_time += tcshalf;
        State = RUNNING;
    }
    else {
        State = BLOCKED;
    }
}

