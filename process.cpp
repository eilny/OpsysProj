#include <stdio.h>
#include <queue>
#include <math.h>
#include "process.h"


Process::Process( char pid, unsigned int arr
        , unsigned int nbursts, float lambda, float alp)
    : process_ID(pid)
    , arrival_time(arr)
	, alpha(alp)
{
    // initialize turnaround, wait, and burst/io queues
    this->turnaround_time = 0;
    this->wait_time = 0;
	this->tau = 1/lambda;
	this->prevBurst = 0;

    this->burst_times = new std::vector<unsigned int>;
    this->io_times = new std::vector<unsigned int>;
    this->pristine_burst_times = new std::vector<unsigned int>;
    this->pristine_io_times = new std::vector<unsigned int>;

    pristine_burst_times->reserve(nbursts);
    burst_times->reserve(nbursts);
    io_times->reserve(nbursts-1);
    pristine_io_times->reserve(nbursts-1);
}

//Copy Constructor 
Process::Process(const Process &p){
	process_ID = p.process_ID;
	arrival_time = p.arrival_time;
    turnaround_time = p.turnaround_time;
	wait_time = p.wait_time;
	
	tau = p.tau; 
	alpha = p.alpha;
	prevBurst = p.prevBurst;
	
	burst_times = new std::vector<unsigned int>;
	io_times = new std::vector<unsigned int>;
	
	pristine_arrival_time = p.pristine_arrival_time;
	
	pristine_burst_times =  new std::vector<unsigned int>;
	pristine_io_times =  new std::vector<unsigned int>;
	
	//Copy Vectors over 
	*burst_times = *(p.burst_times);
	*io_times = *(p.io_times);
	*pristine_burst_times = *(p.pristine_burst_times);
	*pristine_io_times = *(p.pristine_io_times);
}



Process::~Process() {
    // delete burst/io time queues
	if(this->burst_times !=NULL){
		delete this->burst_times;
	}
	if(this->io_times != NULL){
		delete this->io_times;
	}
	if(this->pristine_burst_times !=NULL){
		delete this->pristine_burst_times;
	}
	if(this->pristine_io_times != NULL){
		delete this->pristine_io_times;
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
    pristine_burst_times->push_back(time);
	this->burst_times->push_back(time);
}

void Process::addIo(unsigned int time){
    pristine_io_times->push_back(time);
	this->io_times->push_back(time);
}

char Process::getId(){
	return this->process_ID;
}

unsigned int Process::getArrival(){
	return this->arrival_time;
}

unsigned int Process::getNumBursts(){
	return pristine_burst_times->size();
}

unsigned int Process::getNumBurstsLeft() {
    return burst_times->size();
}

unsigned int Process::getNumIOLeft() {
    return io_times->size();
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

void Process::recalculateTau(){
	this->tau = std::ceil((this->alpha*this->prevBurst) +  ((1-this->alpha)*(this->tau)));
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

float Process::tauEffective() {
    if (burst_times->size() == 0) {
        // can't give tau for something that is completed
        // don't want anything to preempt something that is terminating
        return 0;
    }
	unsigned int proctime = pristine_burst_times->at(pristine_burst_times->size() - burst_times->size());
	proctime -= burstTimeLeft();
	return (this->tau - proctime);
}

void Process::waitTime(unsigned int deltaT) {
    wait_time += deltaT;
}
void Process::resetWait(){
	wait_time = 0;
	
}

void Process::turnA(unsigned int deltaT) {
    turnaround_time += deltaT;
}

void Process::turnAReset() {
    turnaround_time = 0;
}

void Process::finishedCPUBurst() {
	prevBurst = pristine_burst_times->at(pristine_burst_times->size() - burst_times->size());
    burst_times->erase(burst_times->begin());
}

void Process::finishedIOBlock() {
    io_times->erase(io_times->begin());
}

bool Process::isMidBurst() {
	return ( pristine_burst_times->at(pristine_burst_times->size() - burst_times->size()) != burstTimeLeft() );
}
