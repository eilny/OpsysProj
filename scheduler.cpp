#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm> 
#include "scheduler.h"


//For picking print statements
enum PrintState {ARRIVE, START, COMPLETED, BLOCK, IOCOMPLETED, TAU, TERMINATED};


bool sortByArrvial(Process a, Process b){
	if(a.getArrival() == b.getArrival()){
		return (a.getId() < b.getId());
	}
	return (a.getArrival() < b.getArrival());
	
}

//Print Simulation Queue
void printSimQ(std::vector<Process> *queue){
	printf("[Q");
	if(queue->empty()){
		printf(" <empty>]\n");
		return;
	}
	std::vector<Process>::iterator bg = queue->begin();
	std::vector<Process>::iterator ed = queue->end();
	
	while(bg != ed){
		Process p = *bg;
		printf(" %c", p.getId());
		bg++;
	}
	printf("]\n");
	
}


// Printing statements 
// Needs to be modified for process class
void printProcessState(PrintState p, int time, Process cur, float tau){
	if( p == ARRIVE ){
		if(0 != tau){
			printf("time %dms: Process %c (tau %.0fms) arrived; added to ready queue ", time, cur.getId(), tau);
		}else{
			printf("time %dms: Process %c arrived; added to ready queue ", time, cur.getId());
		}
	}
	if(p == START){
		// printf("time %dms: Process %c started using the CPU for %dms burst ", time, cur.getId(), cur.getBurst());
	}
	if(p == COMPLETED){
		// printf("time %dms: Process %c completed a CPU burst; %d bursts to go ", time, cur.getId(), cur.getRemainBurst());
	}
	if(p == BLOCK){
		// printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms", time, cur.getId(), time+cur.getIo());
	}
	if(p == IOCOMPLETED){
		if(0 != tau){
			printf("time %dms: Process %c (tau %.0fms) completed I/O; added to ready queue ", time, cur.getId(), tau);
		}else{
			printf("time %dms: Process %c completed I/O; added to ready queue ", time, cur.getId());
		}
	}
	if(p == TAU){
		printf("time %dms: Recalculated tau = %.0fms for process %c ", time, tau, cur.getId());
	}
	if(p == TERMINATED){
		printf("time %dms: Process %c terminated ", time, cur.getId());
	}
	fflush(stdout);
}


//Constructor
Scheduler::Scheduler(std::vector<Process> *processList,
			unsigned int tcontext,
			unsigned int tmslice, 
			unsigned int rr)
		:	tcs(tcontext)
		,	timeslice(tmslice)
		,	rraddbgn(rr)
{
	avgwait = 0;
	avgburst = 0;
	avgturnaround = 0;
	preemptions = 0;
	remainingtimeslice = 0;
	simulation_timer = 0;
	numCS = 0;
	nextCS = 0;
	hasTimeSlice = false;
	isPreemptive = false;
	switching = false;
	
	this->ARRIVAL = *processList;
	std::sort (this->ARRIVAL.begin(), this->ARRIVAL.end(), sortByArrvial);

}






void Scheduler::contextSwitch(Process toIO, Process toCPU) {
    // do a context switch
    //  move toIO from cpu (RUNNING) to io (BLOCKED)
    //  move toCPU from queue (READY) to cpu (RUNNING)

    // update timing? deal with context switch times
    this->BLOCKED.push_back(*this->RUNNING);
    // should call process contextSwitch here
    State temp = BLK;
    this->RUNNING->setState(temp);

    std::vector<Process>::iterator bg = READY.begin();
    this->RUNNING = this->READY.erase(bg);

    // should call process contextSwitch here
    temp = RUN;
    this->RUNNING->setState(temp);

    switching = true;
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

    if (this->hasTimeSlice) { //if there's a timeslice 
        if (this->remainingtimeslice < deltaT) {
            deltaT = this->remainingtimeslice;
        }
    }

    if (this->switching) { //Context switching 
        if (this->nextCS < deltaT) {
            deltaT = this->nextCS;
        }
        /*
    } else {
        this->nextCS = 0;
        */
    }

    if (this->RUNNING->burstTimeLeft() < deltaT) { //Get the remaing burst time 
        deltaT = this->RUNNING->burstTimeLeft();
    }

    std::vector<Process>::iterator bg = BLOCKED.begin();
    std::vector<Process>::iterator ed = BLOCKED.end();
    while (bg != ed) {
        if (bg->ioTimeLeft() < deltaT) {
            deltaT = bg->ioTimeLeft();
        }
    }

    // check  next arrival time (first element should be fine)
    std::vector<Process>::iterator arr = this->ARRIVAL.begin();
    if (arr->getArrival() < deltaT) {
        deltaT = arr->getArrival();
    }

    return deltaT;
}

void Scheduler::advance() {
    // advance to next event
    unsigned int deltaT = this->timeToNextEvent();

    // advance timer
    this->simulation_timer += deltaT;
    if (this->remainingtimeslice) {
        this->remainingtimeslice -= deltaT;
    }
    if (nextCS) {
        nextCS -= deltaT;
    }

    // ties order: CPU burst, IO burst, arrival - with process ID as backup
    // check if process is done running/blocking
    if ( !this->switching && this->RUNNING->doWork(deltaT) ) {
        // will trigger when should be switching in/out
        // finished, context switch
        this->RUNNING->contextSwitch(false);
        // half to start switching in next process
        this->nextCS = tcs/2;
    }

    if (switching && 0 == nextCS) {
        this->RUNNING = READY.begin();
        this->RUNNING->contextSwitch(true);
        // if switching in
        nextCS = tcs/2;
    }

    std::vector<Process>::iterator bg = BLOCKED.begin();
    std::vector<Process>::iterator ed = BLOCKED.end();
    while (bg != ed) {
        if (bg->doIO(deltaT) ) {
            // finished, put back in READY, swap in next IO
            if (this->isPreemptive) {
                // handle preemption?
            } else {
                // add to READY?
                this->READY.push_back(*bg);
            }
        }
    }

    // try to add new processes to queues
    std::vector<Process>::iterator proc = this->ARRIVAL.begin();
    while(proc->getArrival() == this->simulation_timer) {
        // add to READY? RUNNING?
        if (this->isPreemptive) {
            // handle preemption?
        } else {
            // add to READY?
            this->READY.push_back(*proc);
        }
    }

    // end of timeslice/preemption?

    /*
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

