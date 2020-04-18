#include <stdio.h>
#include <queue>
#include <math.h>
#include "process.h"



Process::Process( char pid, unsigned int arr
        , unsigned int nbursts, float lambda, float alp)
    : process_ID(pid)
    , arrival_time(arr)
    , num_bursts(nbursts)
	, alpha(alp)
{
    // initialize turnaround, wait, and burst/io queues
    this->turnaround_time = 0;
    this->wait_time = 0;
	this->tau = 1/lambda;
    this->burst_times = new std::vector<unsigned int>;
    this->io_times = new std::vector<unsigned int>;
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

void Process::contextSwitch(bool switch_in) {
    if (switch_in) {
        this->state = RUN;
    }
    else {
        this->state = BLK;
        // remove the cpu burst time from vector
        std::vector<unsigned int>::iterator finished = burst_times->begin();
        this->burst_times->erase(finished);
    }
}

void Process::addBurst(unsigned int time){
	this->burst_times->push_back(time);
}
void Process::addIo(unsigned int time){
	this->io_times->push_back(time);
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

float Process::getTau(){
	return this->tau;
}

State Process::setState(State newstate) {
    this->state = newstate;
    return this->state;
}

unsigned int Process::burstTimeLeft() {
    return *this->burst_times->begin();
}

unsigned int Process::ioTimeLeft() {
    return *this->io_times->begin();
}

bool Process::doWork(unsigned int deltaT) {
    std::vector<unsigned int>::iterator burst = this->burst_times->begin();
    *burst -= deltaT;
    if (*burst == 0) {
        // finished CPU burst
        return true;
    }
    return false;
}

bool Process::doIO(unsigned int deltaT) {
    std::vector<unsigned int>::iterator io = this->io_times->begin();
    *io -= deltaT;
    if (*io == 0) {
        // finished IO block
        return true;
    }
    return false;
}

void Process::recalculateTau(int burstTime){
	this->tau = (this->alpha)*(burstTime + (this->tau));
	
}

bool Process::advanceArrival(unsigned int deltaT) {
    arrival_time -= deltaT;
    if (arrival_time == 0) {
        return true;
    }
    return false;
}

void Process::setTau(bool useTau){
	if(useTau) return;
	tau = 0;
}
