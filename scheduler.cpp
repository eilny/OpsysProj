#include <vector>
#include "scheduler.h"

//Constructor
Scheduler::Scheduler(std::vector<Process> processList,
			unsigned int tcontext,
			unsigned int tmslice, 
			unsigned int rr)
		:	tcs(tcontext)
		,	timeslice(tmslice)
		,	rraddbgn(rr)
{
	avgwait = 0;
	simulation_timer = 0;
	hasTimeSlice = false;
	
	//Sort the incoming list 
	for(unsigned int i = 0; i < processList.size(); ++i){
		Process temp = processList[i];
		if(i != 0){
			std::vector<Process>::iterator bg = Incoming.begin();
			std::vector<Process>::iterator ed = Incoming.end();
			while(bg != ed){
				Process comp = *bg;
				if(temp.getArrival() < comp.getArrival()){
					Incoming.insert(bg, temp);
					break;
				} 
				else if(temp.getArrival() == comp.getArrival()){
					if(temp.getId() < comp.getId()){
						Incoming.insert(bg, temp);
						break;
					}
				}
				bg++;
				if(bg == ed){
					Incoming.push_back(temp);
				}
			}
		}
		else{
			Incoming.push_back(temp);
		}
		
	}

}





void Scheduler::contextSwitch(Process toIO, Process toCPU) {
    // do a context switch
    //  move toIO from cpu (RUNNING) to io (BLOCKED)
    //  move toCPU from queue (READY) to cpu (RUNNING)

    // update timing? deal with context switch times
    //this->simulation_timer += this->tcs/2;
    this->BLOCKED.push_back(*this->RUNNING);
    State temp = BLK;
    this->RUNNING->setState(temp);
    //this->simulation_timer += this->tcs/2;
    std::vector<Process>::iterator bg = READY.begin();
    this->RUNNING = this->READY.erase(bg);
    temp = RUN;
    this->RUNNING->setState(temp);
    return;
}

void Scheduler::processArrival(Process newProcess) {
    // move to queue, preempt?
    READY.push_back(newProcess);

    return;
}

unsigned int Scheduler::timeToNextEvent() {
    // check arriving processes
    // check timeslice
    // check burst complete
    // check io complete
    unsigned int deltaT = 0;
    --deltaT; // largest possible unsigned int value

    if (this->hasTimeSlice) {
        if (this->timeslice < deltaT) {
            deltaT = this->timeslice;
        }
    }

    // check  next arrival time (first element should be fine)
    std::vector<Process>::iterator arr = this->Incoming.begin();
    if (arr->getArrival() < deltaT) {
        deltaT = arr->getArrival();
    }

    if (this->RUNNING->burstTimeLeft() < deltaT) {
        deltaT = this->RUNNING->burstTimeLeft();
    }

    if (this->IO->ioTimeLeft() < deltaT) {
        deltaT = this->IO->ioTimeLeft();
    }

    return deltaT;
}

void Scheduler::advance() {
    // can maybe do smarter - advance to next event?

    // simulate next step
    ++this->simulation_timer;
    // replace with a deltaT, update all vals based on that

    /*
	std::vector<unsigned int>::iterator burst = RUNNING->burst_times.begin();
    --burst;
	std::vector<unsigned int>::iterator io = RUNNING->io_times.begin();
    --io;

    // increment wait times
	std::vector<Process>::iterator waiting = READY.begin();
	std::vector<Process>::iterator ed = READY.end();
    while (waiting != ed) {
        ++waiting.wait_time;
        ++waiting;
    }
    */
}
		
unsigned long Scheduler::getTimer(){
	return this->simulation_timer;
	
}

