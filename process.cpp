#include "process.h"


Process::Process(unsigned int pid, unsigned int arr
        , unsigned int processed)
    : process_ID(pid)
    , arrival_time(arr)
    , processed_time(processed)
{
    // initialize turnaround, wait, and burst/io queues
    this->turnaround_time = 0;
    this->wait_time = 0;
    this->burst_times = new std::queue<unsigned int>;
    this->io_times = new std::queue<unsigned int>;
}

Process::~Process() {
    // delete burst/io time queues
    delete this->burst_times;
    delete this->io_times;
}

void Process::contextSwitch(bool switch_in, unsigned int tcshalf) {
    if (switch_in) {
        this->wait_time += tcshalf;
        this->state = RUNNING;
    }
    else {
        this->state = BLOCKED;
    }
}

