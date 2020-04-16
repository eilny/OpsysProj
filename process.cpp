#include <queue>
#include <math.h>
#include "process.h"



Process::Process( char pid, unsigned int arr
        , unsigned int nbursts)
    : process_ID(pid)
    , arrival_time(arr)
    , num_bursts(nbursts)
{
    // initialize turnaround, wait, and burst/io queues
	this->remaining_time = 0;
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

void Process::addBurst(unsigned int time){
	this->burst_times->push(time);
}
void Process::addIo(unsigned int time){
	this->io_times->push(time);
}
char Process::getId(){
	return this->process_ID;
}

unsigned int Process::getArrival(){
	return this->arrival_time;
}
unsigned int Process::getNumBursts(){
	return this->num_bursts;
}



