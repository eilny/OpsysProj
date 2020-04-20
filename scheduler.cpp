#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <deque>
#include <algorithm> 
#include <string>

#include "scheduler.h"



bool sortByArrvial(Process a, Process b){
    if(a.getArrival() == b.getArrival()){
        return (a.getId() < b.getId());
    }
    return (a.getArrival() < b.getArrival());
}

bool sortByTau(Process a, Process b){
	if(a.getTau() == b.getTau()){
		return (a.getId() < b.getId());
	}
	return (a.getTau() < b.getTau());
}

bool sortByIOTimeLeft(Process a, Process b){
	if(a.ioTimeLeft() == b.ioTimeLeft()){
		return (a.getId() < b.getId());
	}
	return (a.ioTimeLeft() < b.ioTimeLeft());
}

//Print Simulation Queue
void printSimQ(std::deque<Process> *queue){
    printf("[Q");
    if(queue->empty()){
        printf(" <empty>]\n");
        return;
    }
    for(unsigned int i = 0; i < queue->size(); ++i){
        printf(" %c", (*queue)[i].getId());
    }
    printf("]\n");
	fflush(stdout);

}


// Printing statements 
// Needs to be modified for process class
void printProcessState(PrintState p, int time, Process *cur, unsigned int tcs = 0){
    if( p == ARRIVE ){
        if(0 != cur->getTau()){
            printf("time %dms: Process %c (tau %.0fms) arrived; added to ready queue ", time, cur->getId(), cur->getTau());
        }else{
            printf("time %dms: Process %c arrived; added to ready queue ", time, cur->getId());
        }
    }
    if(p == START){
        printf("time %dms: Process %c started using the CPU for %dms burst ", time, cur->getId(), cur->burstTimeLeft());
    }
    if(p == COMPLETED){
        printf("time %dms: Process %c completed a CPU burst; %d bursts to go ", time, cur->getId(), cur->getNumBurstsLeft());
    }
    if(p == BLOCK){
        printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms ", time, cur->getId(), time+cur->ioTimeLeft()+(tcs/2));
    }
    if(p == IOCOMPLETED){
        if(0 != cur->getTau()){
            printf("time %dms: Process %c (tau %.0fms) completed I/O; ", time, cur->getId(), cur->getTau());
        }else{
            printf("time %dms: Process %c completed I/O; added to ready queue ", time, cur->getId());
        }
    }
    if(p == TAU){
        printf("time %dms: Recalculated tau = %.0fms for process %c ", time, cur->getTau(), cur->getId());
    }
    if(p == TERMINATED){
        printf("time %dms: Process %c terminated ", time, cur->getId());
    }
	if(p == TIMESLICE){
		printf("time %dms: Time slice expired; ", time);
	}
	if(p == PREEMPT){
		printf("time %dms: ", time);
	}
    fflush(stdout);
}

void printPreemptState(std::deque<Process> *queue, Process* cur, PrintState pState){
	if(pState == TIMESLICE){
		if(queue->empty()){
			printf(" no preemption because ready queue is empty ");
		}else{
			printf(" process %c preempted with %dms to go ", cur->getId(), cur->burstTimeLeft());
		}
	}
	if(pState == PREEMPT){
		printf("Process %c (tau %.0fms) will preempt %c ", (queue->front()).getId(), (queue->front()).getTau(), cur->getId());
	}
	if(pState == IOCOMPLETED){
		if(queue->empty()){
			printf("added to ready queue ");
		}
		else{
			printf("preempting %c ", cur->getId());
		}
	}
	
}


void setTauForAll(std::deque<Process> *queue, bool isUsingTau){
	if(isUsingTau) return;
	for( unsigned int i = 0; i < queue->size(); ++i){
		(*queue)[i].setTau(isUsingTau);		
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
    useTau = false;

    for (const Process& p: *processList) {
        ARRIVAL.push_back(p);
    }
    std::sort (ARRIVAL.begin(), ARRIVAL.end(), sortByArrvial);

}

Scheduler::~Scheduler() {
    // no dynamic allocs
}

//Sets the Algorithm states 
void Scheduler::setAlgorithm(std::string algo){
	if(algo == "FCFS"){
		isPreemptive = false;
		hasTimeSlice = false;
		useTau = false;
	}
	else if(algo == "SJF"){
		isPreemptive = false;
		hasTimeSlice = false;
		useTau = true;
	}
	else if(algo == "SRT"){
		isPreemptive = true;
		hasTimeSlice = false;
		useTau = true;
	}
	else if(algo == "RR"){
		isPreemptive = true;
		hasTimeSlice = true;
		useTau = false;
	}
	else{
		perror("Error: Invalid Algorithm\n");
		return; 	
	}
	setTauForAll(&ARRIVAL, useTau);
	printf("time %dms: Simulator started for %s ", 0, algo.c_str());
	printSimQ((&READY));
	
}

bool Scheduler::switchOUT() {
    // return true if actually switched something out
    if (RUNNING->getNumBurstsLeft() && RUNNING->burstTimeLeft() == 0) {
        RUNNING->finishedCPUBurst();
		pState = COMPLETED; 
		printProcessState(pState , simulation_timer, RUNNING);
		printSimQ(&READY);
        // PRINT HERE: time 67ms: Process A (tau 100ms) completed a CPU burst; 15 bursts to go [Q B]

        if (RUNNING->getNumBurstsLeft()) {
            // more bursts, not complete yet
            if (useTau) {
                // recalculate tau before switching to i/o
                RUNNING->recalculateTau(RUNNING->burstTimeLeft());
				pState = TAU;
				printProcessState(pState, simulation_timer, RUNNING);
				printSimQ(&READY);
                // PRINT HERE: time 177ms: Recalculated tau = 103ms for process B [Q A]
            }

            // PRINT HERE: time 156ms: Process A switching out of CPU; will block on I/O until time 311ms [Q B]
            //              block on I/O until time (simulation_timer + process->I/O + tcs/2
			pState = BLOCK;
			printProcessState(pState, simulation_timer, RUNNING, tcs);
			printSimQ(&READY);

			// increment I/O and handle arrivals before switching this onto the I/O queue
			contextSwitchTime(false);

			RUNNING->contextSwitch(false);
            RUNNING->setState(BLK);
            BLOCKED.push_back(*RUNNING);
            std::sort(BLOCKED.begin(), BLOCKED.end(), sortByIOTimeLeft);
            RUNNING = NULL;
            return true;

        } else {
            // no more bursts, terminate process
			pState = TERMINATED;
			printProcessState(pState, simulation_timer, RUNNING);
			printSimQ(&READY);
            // PRINT HERE: time 4770ms: Process B terminated [Q <empty>]
            RUNNING->setTurnAround(simulation_timer);
            RUNNING->setState(CMP);
            COMPLETE.push_back(*RUNNING);

            // after turnaround is done, increment timer by tcs/2
			contextSwitchTime(false);
        }
        RUNNING = NULL;
        return true;

    } else {
        // not finished, timeslice or preemption
        if (hasTimeSlice) {
            // check timeslice end
            if (remainingtimeslice) {
                // not end of timeslice
            	// what are we doing here then?
            	// have been preempted by something else?
            } else {
				pState = TIMESLICE;
				printProcessState(pState, simulation_timer, RUNNING);
                if (READY.empty()) {
                    // don't bother switching out
					printPreemptState(&READY, RUNNING, pState);
					printSimQ(&READY);
                    // PRINT HERE: time 585ms: Time slice expired; no preemption because ready queue is empty [Q <empty>]
                    return false;
                } else {
                    // do timeslice preemption
                    // preempt and put back on ready queue
                    ++preemptions;
					printPreemptState(&READY, RUNNING, pState);
					printSimQ(&READY);
                    // PRINT HERE: time 465ms: Time slice expired; process B preempted with 70ms to go [Q A]

					contextSwitchTime(false);

                    RUNNING->contextSwitch(false);
                    RUNNING->setState(RDY);
                    READY.push_back(*RUNNING);
                    RUNNING = NULL;
                    return true;
                }
            }
        }
        if (isPreemptive) {
            // not finished and not empty READY timeslice, process has been preempted
            // preempt and put back on ready queue
            ++preemptions;

            contextSwitchTime(false);

            RUNNING->contextSwitch(false);
            RUNNING->setState(RDY);
            READY.push_back(*RUNNING);
            RUNNING = NULL;
            return true;
        }
    }

    RUNNING = NULL;
    return true;
}

bool Scheduler::switchIN() {
    // return true if actually switched something in
	
    // PRINT HERE: time 160ms: Process B (tau 100ms) started using the CPU with 85ms burst remaining [Q <empty>]
    if (!RUNNING) {

        RUNNING = new Process(READY.front());
        RUNNING->setState(RUN);
        RUNNING->contextSwitch(true);
        READY.pop_front();
    } else {
        // do nothing - preempts also set NULL
    }
    contextSwitchTime(true);
	pState = START;
	printProcessState(pState, simulation_timer, RUNNING);
	printSimQ(&READY);
    return true;
}

void Scheduler::contextSwitchTime(bool swtIN) {
    // advance timer here
    simulation_timer += tcs/2;

    if (swtIN) {
        if (isPreemptive) {
            pState = PREEMPT;
            printPreemptState(&READY, RUNNING, pState);
            printSimQ(&READY);
            // PRINT HERE: time 405ms: Process A (tau 54ms) will preempt B [Q A]
        }
    }

    // also advance io
    std::deque<Process>::iterator iobegin = BLOCKED.begin();
    std::deque<Process>::iterator ioend = BLOCKED.end();
    while (iobegin != ioend) {
        for (unsigned int i = 0; i < tcs/2; ++i) {
            // jump by 1 ms until we hit half of context switch time
            if (iobegin->doIO(1)) {
                // return to READY
				pState = IOCOMPLETED;
				printProcessState(pState, simulation_timer, &(*iobegin));
				printSimQ(&READY);
                // PRINT HERE: time 92ms: Process A (tau 78ms) completed I/O; preempting B [Q A]
                // PRINT HERE: time 4556ms: Process B (tau 121ms) completed I/O; added to ready queue [Q B]

				iobegin->finishedIOBlock();
                READY.push_back(*iobegin);
                iobegin = BLOCKED.erase(iobegin);
                continue;
            }
        }
        ++iobegin;
    }

    // also advance arrivals
    std::deque<Process>::iterator arr = this->ARRIVAL.begin();
    std::deque<Process>::iterator arrend = this->ARRIVAL.end();
    while (arr != arrend) {
        for(unsigned int i = 0; i < tcs/2; ++i) {
            if (arr->advanceArrival(1)) {
                // process arrives, add to READY
				pState = ARRIVE;
				printProcessState(pState, simulation_timer, &(*arr));
				printSimQ(&READY);
                // PRINT HERE: time 18ms: Process B (tau 100ms) arrived; added to ready queue [Q B]
                READY.push_back(*arr);
                arr = ARRIVAL.erase(arr);
            }
        }
        ++arr;
    }
}

void Scheduler::contextSwitch() {
    ++numCS;
    // do a context switch
    //  move toIO from cpu (RUNNING) to io (BLOCKED)
    //  move toCPU from queue (READY) to cpu (RUNNING)

    // SWITCH OUT
    if (RUNNING) {
        if (switchOUT()) {
            // contextSwitchTime(false);
        	// call this in switchOUT
        }
    }

    // SWITCH IN
    if (!READY.empty()) {
        switchIN(); // calls contextSwitchTime inside to print 'started using CPU' after C/S in
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
    if (RUNNING && RUNNING->getNumBurstsLeft()) {
        storeEventIfSooner(nxtEvnts, RUNNING->burstTimeLeft(), type);
    }

    // check io complete
    type = ioDone;
    std::deque<Process>::iterator iobegin = BLOCKED.begin();
    std::deque<Process>::iterator ioend = BLOCKED.end();
    while (iobegin != ioend) {
        if (iobegin->getNumIOLeft()) {
            storeEventIfSooner(nxtEvnts, iobegin->ioTimeLeft(), type);
        }
        ++iobegin;
    }

    // check arriving processes
    type = arrival;
    std::deque<Process>::iterator arr = this->ARRIVAL.begin();
    std::deque<Process>::iterator arrend = this->ARRIVAL.end();
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

    if (!RUNNING && !READY.empty()) {
        // check READY after I/O and ARRIVAL to context switch in
        storeEventIfSooner(nxtEvnts, 0, cswitch);
    }

    return nxtEvnts;
}

void Scheduler::fastForward(std::vector<Event> nxtEvnts) {
    // update all timers
    unsigned int deltaT = nxtEvnts[0].timeToEvent;
    simulation_timer += deltaT;
    if (hasTimeSlice) {
        // if timeslice based, less time in slice left
        remainingtimeslice -= deltaT;
    }

    // cpu burst
    if (RUNNING) {
        RUNNING->doWork(deltaT);
    }

    std::deque<Process>::iterator rdybegin = READY.begin();
    std::deque<Process>::iterator rdyend = READY.end();
    while (rdybegin != rdyend) {
        rdybegin->waitTime(deltaT);
        ++rdybegin;
    }

    // io block
    std::deque<Process>::iterator iobegin = BLOCKED.begin();
    std::deque<Process>::iterator ioend = BLOCKED.end();
    while (iobegin != ioend) {
        iobegin->doIO(deltaT);
        ++iobegin;
    }

    // arriving processes
    std::deque<Process>::iterator arr = this->ARRIVAL.begin();
    std::deque<Process>::iterator arrend = this->ARRIVAL.end();
    while (arr != arrend) {
        arr->advanceArrival(deltaT);
        ++arr;
    }

    // finished updating time vars - handle events
    //for (unsigned int i = 0; i < nxtEvnts.size(); ++i) {
    for (const Event& evnt : nxtEvnts) {
        // this requires that nextEvents really has everything covered
        switch(evnt.type) {

            case burstDone:
                // PRINT HERE: finished CPU burst
                contextSwitch();
                break;

            case ioDone:
                if (!BLOCKED.front().getNumBurstsLeft()) {
                    // no more bursts after io is finished, proc should terminate
                	// should not have to, since should always end on cpu burst, but whatever:
                	BLOCKED.pop_front();
                } else {
                	BLOCKED.front().finishedIOBlock();
                    READY.push_back(BLOCKED.front());
                    BLOCKED.pop_front();
                    if (useTau) {
                        // SORT QUEUE
                        std::sort(READY.begin(), READY.end(), sortByTau);
                        if (isPreemptive && RUNNING
                                && iobegin->getTau() < RUNNING->getTau()) {
                            // preempt
                            // PRINT HERE: time 92ms: Process A (tau 78ms) completed I/O; preempting B [Q A]
                            pState = IOCOMPLETED;
                            printProcessState(pState, simulation_timer, &(READY.front()));
                            printPreemptState(&READY, RUNNING, pState);
                            printSimQ(&READY);
                            ++preemptions;
                            contextSwitch();
                        } else {
                            // not preemptive but sorts by tau, needs print statements
							pState = IOCOMPLETED;
                            printProcessState(pState, simulation_timer, &(READY.front()));
                            printPreemptState(&READY, RUNNING, pState);
                        }
                    } else {
                        // return to READY
                        // READY.push_back(BLOCKED.front());
                        // BLOCKED.pop_front();
                        pState = IOCOMPLETED;
                        printProcessState(pState, simulation_timer, &(READY.front()));
                        printSimQ(&READY);
                        // PRINT HERE: time 4556ms: Process B (tau 121ms) completed I/O; added to ready queue [Q B]
                    }
                }
                break;

            case arrival:
				pState = ARRIVE;
                printProcessState(pState, simulation_timer, &(ARRIVAL.front()));
				
                READY.push_back(ARRIVAL.front());
                ARRIVAL.pop_front();

                printSimQ(&READY);
                // PRINT HERE: time 18ms: Process B (tau 100ms) arrived; added to ready queue [Q B]
                if (useTau) {
                    // sort READY
                    if (isPreemptive && RUNNING
                            && arr->getTau() < RUNNING->getTau()) {
                        // preempt
                        // PRINT HERE: preemption?
                        ++preemptions;
                        contextSwitch();
                    }
                }
                break;

            case tslice:
                if (0 == remainingtimeslice) {
                    // timeslice done, context switch
                    pState = TIMESLICE;
                    printProcessState(pState, simulation_timer, RUNNING);
                    if (READY.empty()) {
                        // no context switch
                        printPreemptState(&READY, RUNNING, pState);
                        printSimQ(&READY);
                        // PRINT HERE: time 4709ms: Time slice expired; no preemption because ready queue is empty [Q <empty>]
                    } else {
                        printPreemptState(&READY, RUNNING, pState);
                        printSimQ(&READY);
                        // PRINT HERE: time 2908ms: Time slice expired; process B preempted with 6ms to go [Q A]
                        ++preemptions;
                        contextSwitch();
                    }
                }
                break;

            case cswitch:
                contextSwitch();
                break;
        }
    }
    return;
}

bool Scheduler::advance() {
    // return true if advanced onwards
    // return false if finished

    // advance to next event
    std::vector<Event> thingsHappening = nextEvents();
    if (thingsHappening.empty()) {
        return false;
    } else { // things are happening, so go do them
		//Printing statement 
        //fastForward(thingsHappening[0].timeToEvent);
        fastForward(thingsHappening);
        return true;
    }
}

unsigned long Scheduler::getTimer(){
    return this->simulation_timer;

}

void Scheduler::runSimulation(std::string algo){
	this->setAlgorithm(algo);
	while(advance()){
		// #ifdef DEBUG_MODE
			// printf("Advancing simulation in loop!\n");
		// #endif
	}
	printf("time %ldms: Simulator ended for %s ", this->simulation_timer, algo.c_str());
	printSimQ(&READY);
}


