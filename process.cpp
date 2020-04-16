#include <stdio.h>
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
	this->remaining_burst = 0;
	this->remaining_io = 0;
    this->turnaround_time = 0;
    this->wait_time = 0;
    this->burst_times = new std::queue<unsigned int>;
    this->io_times = new std::queue<unsigned int>;
}

Process::~Process() {
    // delete burst/io time queues
	if(this->burst_times !=NULL){
		delete this->burst_times;
	}
	if(this->io_times != NULL){
		delete this->io_times;
	}
}

void Process::contextSwitch(bool switch_in, unsigned int tcshalf) {
    if (switch_in) {
        this->wait_time += tcshalf;
        this->state = RUN;
    }
    else {
        this->state = BLK;
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

State Process::setState(State newstate) {
    this->state = newstate;
    return this->state;
}

unsigned int Process::burstTimeLeft() {
    return this->remaining_burst;
}

unsigned int Process::ioTimeLeft() {
    return this->remaining_io;
}

