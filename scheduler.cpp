#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm> 
#include <string>
#include "scheduler.h"


//For picking print statements
enum PrintState {ARRIVE, START, COMPLETED, BLOCK, IOCOMPLETED, TAU, TERMINATED, TIMESLICE, ERROR};


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
void printProcessState(PrintState p, int time, Process cur){
    if( p == ARRIVE ){
        if(0 != cur.getTau()){
            printf("time %dms: Process %c (tau %.0fms) arrived; added to ready queue ", time, cur.getId(), cur.getTau());
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
        if(0 != cur.getTau()){
            printf("time %dms: Process %c (tau %.0fms) completed I/O; added to ready queue ", time, cur.getId(), cur.getTau());
        }else{
            printf("time %dms: Process %c completed I/O; added to ready queue ", time, cur.getId());
        }
    }
    if(p == TAU){
        printf("time %dms: Recalculated tau = %.0fms for process %c ", time, cur.getTau(), cur.getId());
    }
    if(p == TERMINATED){
        printf("time %dms: Process %c terminated ", time, cur.getId());
    }
	if(p == TIMESLICE){
		printf("time %dms: Time slice expired; ", time);
	}
    fflush(stdout);
}

PrintState getPrintState(eventType evnt){
	if(evnt == burstDone) return COMPLETED;
    else if(evnt == ioDone) return IOCOMPLETED;
    else if(evnt == arrival) return ARRIVE;
    else if(evnt == tslice) return TIMESLICE;
    // else if(evnt == switchOUT) 
    // else if(evnt == switchIN)
	else{
		perror("ERROR: Not recognized state");
		return ERROR;
	}
	
}


//////////////////////////////////Start of Class definitions//////////////////////////////////

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

//Sets the Algorithm states 
void Scheduler::setAlgorithm(std::string algo){
	if(algo == "FCFS"){
		isPreemptive = false;
		hasTimeSlice = false;
	}
	else if(algo == "SJF"){
		isPreemptive = false;
		hasTimeSlice = false;
	}
	else if(algo == "SRT"){
		isPreemptive = true;
		hasTimeSlice = false;
	}
	else if(algo == "RR"){
		isPreemptive = true;
		hasTimeSlice = true;
	}
	else{
		perror("Error: Invalid Algorithm\n");
		return; 	
	}
	printf("time %dms: Simulator started for %s ", 0, algo.c_str());
	printSimQ(&(this->READY));
	
}




void Scheduler::contextSwitch() {
    // do a context switch
    //  move toIO from cpu (RUNNING) to io (BLOCKED)
    //  move toCPU from queue (READY) to cpu (RUNNING)

    // SWITCH OUT
    if (RUNNING) {
        // PRINT HERE: time 67ms: Process A (tau 100ms) completed a CPU burst; 15 bursts to go [Q B]
        // PRINT HERE: time 156ms: Process A switching out of CPU; will block on I/O until time 311ms [Q B]
        //              block on I/O until time (simulation_timer + process->I/O + tcs/2
        // PRINT HERE: time 177ms: Recalculated tau = 103ms for process B [Q A]
        // PRINT HERE: time 4770ms: Process B terminated [Q <empty>]

        this->BLOCKED.push_back(*this->RUNNING);
        RUNNING->contextSwitch(false);
        // advance timer here
        simulation_timer += tcs/2;

        // also advance io
        std::vector<Process>::iterator iobegin = BLOCKED.begin();
        std::vector<Process>::iterator ioend = BLOCKED.end();
        while (iobegin != ioend) {
            // could skip ahead by a couple ms here - need to fix
            // i.e. time is 100, arrival at 101, tcs/2 finish at 102, this will catch 102 but not 101 arrivals
            // reformat this section to call fastForward, but without context switching?
            // also need to avoid timeslices, etc.
            if (iobegin->doIO(tcs/2)) {
                // return to READY
                // PRINT HERE: time 92ms: Process A (tau 78ms) completed I/O; preempting B [Q A]
                // PRINT HERE: time 4556ms: Process B (tau 121ms) completed I/O; added to ready queue [Q B]
               this->READY.push_back(*iobegin);
               iobegin = BLOCKED.erase(iobegin);
            }
            ++iobegin;
        }
        // also advance arrivals
        std::vector<Process>::iterator arr = this->ARRIVAL.begin();
        std::vector<Process>::iterator arrend = this->ARRIVAL.end();
        while (arr != arrend) {
            if (arr->advanceArrival(tcs/2)) {
                // process arrives, add to READY
                // PRINT HERE: time 18ms: Process B (tau 100ms) arrived; added to ready queue [Q B]
                READY.push_back(*arr);
                arr = ARRIVAL.erase(arr);
            }
            ++arr;
        }
    }


    // SWITCH IN
    if (!READY.empty()) {
        std::vector<Process>::iterator bg = READY.begin();
        bg->contextSwitch(true);
        this->RUNNING = &(*bg);
		this->READY.erase(bg);
        // advance timer here
        simulation_timer += tcs/2;
        // PRINT HERE: time 160ms: Process B (tau 100ms) started using the CPU with 85ms burst remaining [Q <empty>]

        // also advance io
        std::vector<Process>::iterator iobegin = BLOCKED.begin();
        std::vector<Process>::iterator ioend = BLOCKED.end();
        while (iobegin != ioend) {
            if (iobegin->doIO(tcs/2)) {
                // return to READY
                // PRINT HERE: time 92ms: Process A (tau 78ms) completed I/O; preempting B [Q A]
                // PRINT HERE: time 4556ms: Process B (tau 121ms) completed I/O; added to ready queue [Q B]
                READY.push_back(*iobegin);
                iobegin = BLOCKED.erase(iobegin);
            }
            ++iobegin;
        }
        // also advance arrivals
        std::vector<Process>::iterator arr = this->ARRIVAL.begin();
        std::vector<Process>::iterator arrend = this->ARRIVAL.end();
        while (arr != arrend) {
            if (arr->advanceArrival(tcs/2)) {
                // process arrives, add to READY
                // PRINT HERE: time 18ms: Process B (tau 100ms) arrived; added to ready queue [Q B]
                READY.push_back(*arr);
                arr = ARRIVAL.erase(arr);
            }
            ++arr;
        }
        // if SRT and process arrives/completes I/O with lower tau than the process being switched in
        // PRINT HERE: time 405ms: Process A (tau 54ms) will preempt B [Q A]
    }

    if (hasTimeSlice) {
        remainingtimeslice = timeslice;
    }

    return;
}

void Scheduler::processArrival(Process newProcess) {
    // move to queue, preempt?
    READY.push_back(newProcess);

    return;
}

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

    std::vector<Event> nxtEvnts;
    // struct Event ne; //placeholder
    enum eventType type;

    // check burst complete
    type = burstDone;
    if (!switchIN && !switchOUT && RUNNING) {
        storeEventIfSooner(nxtEvnts, RUNNING->burstTimeLeft(), type);
    }

    // check io complete
    type = ioDone;
    std::vector<Process>::iterator iobegin = BLOCKED.begin();
    std::vector<Process>::iterator ioend = BLOCKED.end();
    while (iobegin != ioend) {
        storeEventIfSooner(nxtEvnts, iobegin->ioTimeLeft(), type);
        ++iobegin;
    }

    // check arriving processes
    type = arrival;
    std::vector<Process>::iterator arr = this->ARRIVAL.begin();
    std::vector<Process>::iterator arrend = this->ARRIVAL.end();
    while (arr != arrend) {
        // TODO: quit after arrival is no longer the same value?
        // that's mostly for efficiency, who cares atm
        storeEventIfSooner(nxtEvnts, arr->getArrival(), type);
        ++arr;
    }

    // check timeslice if algo is timeslice based
    type = tslice;
    if (this->hasTimeSlice) {
        storeEventIfSooner(nxtEvnts, remainingtimeslice, type);
    }

    // check switch out/switch in
    if (switchOUT || switchIN) {
        storeEventIfSooner(nxtEvnts, nextCS, type);
    }

    return nxtEvnts;
}

void Scheduler::fastForward(unsigned int deltaT) {
    // simulation moves forward
    simulation_timer += deltaT;
    if (remainingtimeslice) {
        // if timeslice based, less time in slice left
        remainingtimeslice -= deltaT;
        if (0 == remainingtimeslice) {
            // timeslice done, context switch
            if (READY.empty()) {
                // no context switch
                // PRINT HERE: time 4709ms: Time slice expired; no preemption because ready queue is empty [Q <empty>]
            } else {
                // PRINT HERE: time 2908ms: Time slice expired; process B preempted with 6ms to go [Q A]
                contextSwitch();
            }
            // reset timeslice
            remainingtimeslice = timeslice;
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
            // PRINT HERE: time 92ms: Process A (tau 78ms) completed I/O; preempting B [Q A]
            // PRINT HERE: time 4556ms: Process B (tau 121ms) completed I/O; added to ready queue [Q B]
            READY.push_back(*iobegin);
            iobegin = BLOCKED.erase(iobegin);
        }
        ++iobegin;
    }

    // arriving processes
    std::vector<Process>::iterator arr = this->ARRIVAL.begin();
    std::vector<Process>::iterator arrend = this->ARRIVAL.end();
    while (arr != arrend) {
        if (arr->advanceArrival(deltaT)) {
            // process arrives, add to READY
            // PRINT HERE: time 18ms: Process B (tau 100ms) arrived; added to ready queue [Q B]
            READY.push_back(*arr);
            arr = ARRIVAL.erase(arr);
        }
        ++arr;
    }

    // finished updating time vars
    return;
}

bool Scheduler::advance() {
    // return true if advanced onwards
    // return false if finished
    if (simulation_timer == 0) {
        // PRINT HERE: time 0ms: Simulator started for SJF [Q <empty>]
    }

    // advance to next event
    std::vector<Event> thingsHappening = nextEvents();
    if (thingsHappening.empty()) {
        // no next events - need to finish current running process?
        if (RUNNING) {
            unsigned int deltaT = RUNNING->burstTimeLeft();
            RUNNING->doWork(deltaT);
            // switch out to finish
            contextSwitch();
            // PRINT HERE: time 4772ms: Simulator ended for FCFS [Q <empty>]
        }
        return false;
    } else {
		//Printing statement 
		
		//printProcessState(
        fastForward(thingsHappening[0].timeToEvent);
        return true;
    }
}

unsigned long Scheduler::getTimer(){
    return this->simulation_timer;

}

void Scheduler::runSimulation(std::string algo){
	this->setAlgorithm(algo);
	while(advance()){
		#ifdef DEBUG_MODE
			printf("Advancing simulation in loop!\n");
		#endif
	}
}


