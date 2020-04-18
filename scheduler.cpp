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
	switchIN = false;
	switchOUT = false;
	
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

    // TODO: fix in/out switch flags
    switchIN = true;
    switchOUT = true;
    return;
}

void Scheduler::processArrival(Process newProcess) {
    // move to queue, preempt?
    READY.push_back(newProcess);

    return;
}

enum eventType {
    burstDone
    , ioDone
    , arrival
    , timeslice
    , switchOUT
    , switchIN
};

struct Event {
    unsigned int timeToEvent;
    eventType type;
};

void Scheduler::storeEventIfSooner(std::vector<Event> & events
        , unsigned int time, enum eventType type) {
    Event next;
    next.timeToEvent = time;
    next.type = type;

    if (events.empty() || time == events[0].timeToEvent) {
        events.push_back(next);
    } else if (time < events[0].timeToEvent) {
        events.clear();
        events.push_back(next);
    }
    return;
}

std::vector<Event> Scheduler::nextEvents() {

    std::vector<unsigned int> nextEvents;
    struct Event ne; //placeholder
    enum eventType type;

    // check burst complete
    type = burstDone;
    if (!switchIN && !switchOUT && RUNNING) {
        storeEventIfSooner(nextEvents, RUNNING->burstTimeLeft(), type);
    }

    // check io complete
    type = ioDone;
    std::vector<Process>::iterator iobegin = BLOCKED.begin();
    std::vector<Process>::iterator ioend = BLOCKED.end();
    while (iobegin != ioend) {
        storeEventIfSooner(nextEvents, iobegin->ioTimeLeft(), type);
        ++iobegin;
    }

    // check arriving processes
    type = arrival;
    std::vector<Process>::iterator arr = this->ARRIVAL.begin();
    std::vector<Process>::iterator arrend = this->ARRIVAL.end();
    while (arr != arrend) {
        // TODO: quit after arrival is no longer the same value?
        // that's mostly for efficiency, who cares atm
        storeEventIfSooner(nextEvents, arr->getArrival(), type);
        ++arr;
    }

    // check timeslice if algo is timeslice based
    type = timeslice;
    if (this->hasTimeSlice) {
        storeEventIfSooner(nextEvents, remainingtimeslice, type);
    }

    // check switch out/switch in
    if (switchOUT || switchIN) {
        storeEventIfSooner(nextEvents, nextCS, type);
    }

    return nextEvents;
}

void Scheduler::fastForward(deltaT) {
    // simulation moves forward
    simulation_timer += deltaT;
    if (remainingtimeslice) {
        // if timeslice based, less time in slice left
        remainingtimeslice -= deltaT;
        if (0 == remainingtimeslice) {
            // timeslice done, context switch
            // return to READY queue
        }
    }

    // cpu burst
    if (RUNNING) {
        if (RUNNING->doWork(deltaT)) {
            // burst finisheds - context switch
            contextSwitch();
        }
    }

    // io block
    std::vector<Process>::iterator iobegin = BLOCKED.begin();
    std::vector<Process>::iterator ioend = BLOCKED.end();
    while (iobegin != ioend) {
        if (iobegin->doIO(deltaT)) {
            // return to READY
            READY.push_back(iobegin);
            BLOCKED.erase(iobegin);
        }
        ++iobegin;
    }

    // arriving processes
    std::vector<Process>::iterator arr = this->ARRIVAL.begin();
    std::vector<Process>::iterator arrend = this->ARRIVAL.end();
    while (arr != arrend) {
        if (arr->advanceArrival(deltaT)) {
            // process arrives, add to READY
            READY.push_back(arr);
            ARRIVAL.erase(arr);
        }
        ++arr;
    }

    // finished updating time vars
    return;
}

bool Scheduler::advance() {
    // return true if advanced onwards
    // return false if finished

    // advance to next event
    std::vector<Event> thingsHappening = nextEvents();
    if (thingsHappening.empty()) {
        // no next events - need to finish current running process?
        if (RUNNING) {
            unsigned int deltaT = RUNNING->burstTimeLeft();
            RUNNING->doWork(deltaT);
            // switch out to finish
            contextSwitch();
        }
        return flase;
    } else {
        fastForward(thingsHappening[0].timeToEvent);
        return true;
    }
}
		
unsigned long Scheduler::getTimer(){
	return this->simulation_timer;
	
}

