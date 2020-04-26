#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <deque>
#include <algorithm> 
#include <string>

#include "scheduler.h"



bool sortByArrvial(Process* a, Process* b) {
    if (a->getArrival() == b->getArrival()) {
        return (a->getId() < b->getId());
    }
    return (a->getArrival() < b->getArrival());
}

bool sortByTau(Process* a, Process* b) {
    //if (a->getTau() == b->getTau()) {
    if (a->tauEffective() == b->tauEffective()) {
        return (a->getId() < b->getId());
    }
    //return (a->getTau() < b->getTau());
    return (a->tauEffective() < b->tauEffective());
}

bool sortByIOTimeLeft(Process* a, Process* b) {
    if (a->ioTimeLeft() == b->ioTimeLeft()) {
        return (a->getId() < b->getId());
    }
    return (a->ioTimeLeft() < b->ioTimeLeft());
}

bool sortByRemainingBurstTime(Process* a, Process* b) {
    if (a->ioTimeLeft() == b->ioTimeLeft()) {
        return (a->getId() < b->getId());
    }
    return (a->ioTimeLeft() < b->ioTimeLeft());
}

//Print Simulation Queue
void printSimQ(std::deque<Process*> *queue) {
    printf("[Q");
    if (queue->empty()) {
        printf(" <empty>]\n");
        return;
    }
    for(unsigned int i = 0; i < queue->size(); ++i) {
        printf(" %c", (*queue)[i]->getId());
    }
    printf("]\n");
    fflush(stdout);
}

//For Preemption printing
void printPreemptState(std::deque<Process*> *queue, Process* cur, PrintState pState, Process* newAdded = NULL) {
    if (pState == TIMESLICE) {
        if (queue->empty()) {
            printf("no preemption because ready queue is empty ");
        } else {
            printf("process %c preempted with %dms to go ", cur->getId(), cur->burstTimeLeft());
        }
    }
    if (pState == PREEMPT && newAdded) {
        printf("Process %c (tau %.0fms) will preempt %c "
                , newAdded->getId(), newAdded->getTau(), cur->getId());
    }
    if (pState == IOPREEMPT && newAdded) {
        if (cur->getId() == newAdded->getId()) {
            printf("added to ready queue ");
        } else {
            printf("preempting %c ", newAdded->getId());
        }
    }
}

// Printing statements 
// Needs to be modified for process class
void printProcessState(PrintState p, int time, Process *cur,
        std::deque<Process*> *queue, 
        std::string algoUsed = "", 
        unsigned int tcs = 0, 
        Process* newAdded = NULL) {
#ifndef PRINTALL
    // Don't print past 1000ms 
    // Commented out for testing
    if (time >= 1000 && p != TERMINATED) {
        return;
    }
#endif
    if ( p == ARRIVE ) {
        if (0 != cur->getTau()) {
            printf("time %dms: Process %c (tau %.0fms) arrived; added to ready queue "
                    , time, cur->getId(), cur->getTau());
        } else {
            printf("time %dms: Process %c arrived; added to ready queue "
                    , time, cur->getId());
        }
    }
    if (p == START) {
        if (0 != cur->getTau()) {
            if(algoUsed == "SRT") {
                printf("time %dms: Process %c (tau %.0fms) started using the CPU with %dms burst remaining "
                        , time, cur->getId(), cur->getTau(), cur->burstTimeLeft());
            } else {
                printf("time %dms: Process %c (tau %.0fms) started using the CPU for %dms burst "
                        , time, cur->getId(), cur->getTau(), cur->burstTimeLeft());
            }
        } else {
            if(algoUsed == "RR") {
                if (cur->isMidBurst()) {
                    printf("time %dms: Process %c started using the CPU with %dms burst remaining "
                            , time, cur->getId(), cur->burstTimeLeft());
                } else {
                    printf("time %dms: Process %c started using the CPU for %dms burst "
                            , time, cur->getId(), cur->burstTimeLeft());
                }
            } else {
                printf("time %dms: Process %c started using the CPU for %dms burst "
                        , time, cur->getId(), cur->burstTimeLeft());
            }
        }
    }
    if (p == COMPLETED) {
        if (0 != cur->getTau()) {
            if (cur->getNumBurstsLeft() == 1) {
                printf("time %dms: Process %c (tau %.0fms) completed a CPU burst; %d burst to go "
                        , time, cur->getId(), cur->getTau(), cur->getNumBurstsLeft());
            } else {
                printf("time %dms: Process %c (tau %.0fms) completed a CPU burst; %d bursts to go "
                        , time, cur->getId(), cur->getTau(), cur->getNumBurstsLeft());
            }
        } else {
            if (cur->getNumBurstsLeft() == 1) {
                printf("time %dms: Process %c completed a CPU burst; %d burst to go "
                        , time, cur->getId(), cur->getNumBurstsLeft());
            } else {
                printf("time %dms: Process %c completed a CPU burst; %d bursts to go "
                        , time, cur->getId(), cur->getNumBurstsLeft());
            }	
        }
    }
    if (p == BLOCK) {
        printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms "
                , time, cur->getId(), time+cur->ioTimeLeft()+(tcs/2));
    }
    if (p == IOCOMPLETED) {
        if (0 != cur->getTau()) {
            printf("time %dms: Process %c (tau %.0fms) completed I/O; added to ready queue "
                    , time, cur->getId(), cur->getTau());
        } else {
            printf("time %dms: Process %c completed I/O; added to ready queue "
                    , time, cur->getId());
        }
    }
    if (p == IOPREEMPT) {
        if (0 != cur->getTau()) {
            printf("time %dms: Process %c (tau %.0fms) completed I/O; "
                    , time, cur->getId(), cur->getTau());
        } else {
            printf("time %dms: Process %c completed I/O; "
                    , time, cur->getId());
        }
        printPreemptState(queue, cur, p, newAdded);
    }
    if (p == TAU) {
        printf("time %dms: Recalculated tau = %.0fms for process %c "
                , time, cur->getTau(), cur->getId());
    }
    if (p == TERMINATED) {
        printf("time %dms: Process %c terminated ", time, cur->getId());
    }
    if (p == TIMESLICE) {
        printf("time %dms: Time slice expired; ", time);
        printPreemptState(queue, cur, p, newAdded);
    }
    if (p == PREEMPT) {
        printf("time %dms: ", time);
        printPreemptState(queue, cur, p, newAdded);
    }
    printSimQ(queue);
    fflush(stdout);
}



void setTauForAll(std::deque<Process*> *queue, bool isUsingTau) {
    if (isUsingTau) return;
    for( unsigned int i = 0; i < queue->size(); ++i) {
        (*queue)[i]->setTau(isUsingTau);
    }	
}


//////////////////////////////////Start of Class definitions//////////////////////////////////

//Constructor
Scheduler::Scheduler(std::vector<Process*> *processList,
        unsigned int tcontext,
        unsigned int tmslice, 
        unsigned int rr)
    : ARRIVAL()
    , READY()
    , BLOCKED()
    , COMPLETE()
    , RUNNING(NULL)
    , simulation_timer(0)
    , avgwait(0)
    , avgburst(0)
    , avgturnaround(0)
    , numCS(0)
    , tcs(tcontext)
    , isPreemptive(false)
    , preemptions(0)
    , hasTimeSlice(false)
    , timeslice(tmslice)
    , remainingtimeslice(tmslice)
    , rraddbgn(rr)
    , useTau(false)
      , pState()

{
    for (const auto& p: *processList) {
        ARRIVAL.push_back(p);
    }
    std::sort(ARRIVAL.begin(), ARRIVAL.end(), sortByArrvial);
}

Scheduler::~Scheduler() {
    // no dynamic allocs
}

//Sets the Algorithm states 
void Scheduler::setAlgorithm(std::string algo) {
    if (algo == "FCFS") {
        isPreemptive = false;
        hasTimeSlice = false;
        useTau = false;
    } else if (algo == "SJF") {
        isPreemptive = false;
        hasTimeSlice = false;
        useTau = true;
    } else if (algo == "SRT") {
        isPreemptive = true;
        hasTimeSlice = false;
        useTau = true;
    } else if (algo == "RR") {
        isPreemptive = true;
        hasTimeSlice = true;
        useTau = false;
    } else {
        perror("Error: Invalid Algorithm\n");
        return; 	
    }
    algoUsed = algo;
    setTauForAll(&ARRIVAL, useTau);
    printf("time %dms: Simulator started for %s ", 0, algo.c_str());
    printSimQ((&READY));

}

bool Scheduler::contextSwitchTime(bool swtIN) {
    // returns a bool - whether another switchIN/contextSwitch needs to be called
    //      again after completion of this one due to process finishing i/o or arriving
    //      with higher priority
    bool preemptAfter = false;

    for (unsigned int i = 0; i < tcs/2; ++i) {
    }

    if (RUNNING) {
        RUNNING->turnA(tcs/2);
    }

    // this expects that the switched in process is no longer on READY
    for (Process * rdy : READY) {
        // increase wait time for all processes not being switched in
        if (rdy != RUNNING) {
            rdy->waitTime(tcs/2);
        }
    }

    // also advance io
    for (Process* io : BLOCKED) {
        for (unsigned int i = 1; i <= tcs/2; ++i) {
            // jump by 1 ms until we hit half of context switch time
            if (io->ioTimeLeft() == 0) {
                // catch unhandled io

                io->finishedIOBlock();
                READY.push_back(io);
                BLOCKED.pop_front();
                // added to READY, increase wait time appropriately (i.e. remainder of tcs/2)
                io->waitTime(tcs/2 - i);
                if (useTau) {
                    std::sort(READY.begin(), READY.end(), sortByTau);
                }

                pState = IOCOMPLETED;
                printProcessState(pState, simulation_timer, io, &READY, algoUsed);
                // PRINT HERE: time 92ms: Process A (tau 78ms) completed I/O; preempting B [Q A]
                // PRINT HERE: time 4556ms: Process B (tau 121ms) completed I/O; added to ready queue [Q B]

                if (isPreemptive && RUNNING && !READY.empty()) {
                    if (useTau && io == READY.front() && READY.front()->getTau() < RUNNING->tauEffective()) {
                        // print 'will preempt' here, need to call switchIN again after

                        if (swtIN) {
                            pState = PREEMPT;
                            printProcessState(pState, simulation_timer, RUNNING, &READY);
                            // PRINT HERE: time 405ms: Process A (tau 54ms) will preempt B [Q A]
                            preemptAfter = true;
                        }
                    }
                }
                break; // don't underflow or try to decrement io any more
            } else {
                if (io->doIO(1)) {
                    if (i == tcs/2 && swtIN) {
                        // don't print, need to print after next process is switched in
                        // need to add process that was preempted in here?
                        /*
                        READY.push_back(RUNNING);
                        std::sort(READY.begin(), READY.end(), sortByTau);
                        */
                        break;
                    }
                    // return to READY
                    io->finishedIOBlock();
                    READY.push_back(io);
                    BLOCKED.pop_front();
                    // added to READY, increase wait time appropriately (i.e. remainder of tcs/2)
                    io->waitTime(tcs/2 - i);
                    if (useTau) {
                        std::sort(READY.begin(), READY.end(), sortByTau);
                    }

                    pState = IOCOMPLETED;
                    printProcessState(pState, simulation_timer+i, io, &READY);
                    // PRINT HERE: time 92ms: Process A (tau 78ms) completed I/O; preempting B [Q A]
                    // PRINT HERE: time 4556ms: Process B (tau 121ms) completed I/O; added to ready queue [Q B]


                    if (isPreemptive && RUNNING && !READY.empty()) {
                        if (useTau && io == READY.front() && READY.front()->getTau() < RUNNING->tauEffective()) {
                            // print 'will preempt' here, need to call switchIN again after

                            if (swtIN) {
                                if(algoUsed != "SRT"){
                                    pState = PREEMPT;
                                    printProcessState(pState, simulation_timer+i, RUNNING, &READY);
                                }
                                // PRINT HERE: time 405ms: Process A (tau 54ms) will preempt B [Q A]
                                preemptAfter = true;
                            }
                        }
                    }
                    break; // don't underflow or try to decrement io any more
                }
            }
        }
    }

    // also advance arrivals
    for(Process* arr : ARRIVAL) {
        for(unsigned int i = 0; i < tcs/2; ++i) {
            if (arr->getArrival() == 0) {
                // catch unhandled arr

                READY.push_back(arr);
                ARRIVAL.pop_front();
                // added to READY, increase wait time appropriately (i.e. remainder of tcs/2)
                arr->waitTime(tcs/2 - i);
                if (useTau) {
                    std::sort(READY.begin(), READY.end(), sortByTau);
                }

                pState = ARRIVE;
                printProcessState(pState, simulation_timer, arr, &READY);
                // PRINT HERE: time 92ms: Process A (tau 78ms) completed I/O; preempting B [Q A]
                // PRINT HERE: time 4556ms: Process B (tau 121ms) completed I/O; added to ready queue [Q B]


                if (isPreemptive && RUNNING && !READY.empty()) {
                    if (useTau && arr == READY.front() && READY.front()->getTau() < RUNNING->tauEffective()) {
                        // print 'will preempt' here, need to call switchIN again after

                        if (swtIN) {
                            pState = PREEMPT;
                            printProcessState(pState, simulation_timer, RUNNING, &READY);
                            // PRINT HERE: time 405ms: Process A (tau 54ms) will preempt B [Q A]
                            preemptAfter = true;
                        }
                    }
                }
                break; // don't underflow or try to decrement arr any more
            } else {
                if (arr->advanceArrival(1)) {
                    if (i == tcs/2 && swtIN) {
                        // don't print, need to print after next process is switched in
                        break;
                    }
                    // process arrives, add to READY

                    READY.push_back(arr);
                    ARRIVAL.pop_front();
                    arr->waitTime(tcs/2 - i);
                    if (useTau) {
                        std::sort(READY.begin(), READY.end(), sortByTau);
                    }

                    pState = ARRIVE;
                    printProcessState(pState, simulation_timer+i, arr, &READY, algoUsed);
                    // PRINT HERE: time 18ms: Process B (tau 100ms) arrived; added to ready queue [Q B]

                    if (isPreemptive && RUNNING && !READY.empty()) {
                        if (useTau && arr == READY.front() && READY.front()->getTau() < RUNNING->tauEffective()) {
                            // print 'will preempt' here, need to call switchIN again after

                            if (swtIN) {
                                pState = PREEMPT;
                                printProcessState(pState, simulation_timer+i, RUNNING, &READY);
                                // PRINT HERE: time 405ms: Process A (tau 54ms) will preempt B [Q A]
                                preemptAfter = true;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    // advance timer here
    simulation_timer += tcs/2;

    return preemptAfter;
}

bool Scheduler::switchOUT(bool forcePrint) {
    // return true if actually switched something out

    if (!RUNNING) {
        // nothing running, didn't switch out
        return false;
    }

    // if we are switching something out, save the burst time
    // --> is it still burst time if preempted? stats question...
    // this allows for burst duration of 0 if back to back preemptions - is that ok?
    BURSTS.push_back(simulation_timer - burstTimeStart);

    if (RUNNING->burstTimeLeft()) {
        // preemption, not done
        // move to READY
        ++preemptions;

        // tslice or preempted
        if (hasTimeSlice) {
            // tslice must be 0 here
            if (!remainingtimeslice && READY.empty()) {
                pState = TIMESLICE;
                printProcessState(pState, simulation_timer, RUNNING, &READY, algoUsed);
            } else {
                pState = TIMESLICE;
                printProcessState(pState, simulation_timer, RUNNING, &READY, algoUsed);

                contextSwitchTime(false);

                READY.push_back(RUNNING);
            }
        } else if (isPreemptive && useTau) {
            if (forcePrint) {
                pState = PREEMPT;
                printProcessState(pState, simulation_timer, RUNNING, &READY, algoUsed, 0, READY.front());
            }

            contextSwitchTime(false);

            READY.push_back(RUNNING);
            std::sort(READY.begin(), READY.end(), sortByTau);
        }
    } else {
        // finished burst
        RUNNING->finishedCPUBurst();

        if (RUNNING->getNumBurstsLeft()) {
            // more bursts = not done, move to I/O
            pState = COMPLETED; 
            printProcessState(pState , simulation_timer, RUNNING, &READY, algoUsed);

            if (useTau) {
                // recalculate tau before switching to i/o
                RUNNING->recalculateTau();
                pState = TAU;
                printProcessState(pState, simulation_timer, RUNNING, &READY, algoUsed);
            }

            pState = BLOCK;
            printProcessState(pState, simulation_timer, RUNNING, &READY, algoUsed, tcs);

            contextSwitchTime(false);

            BLOCKED.push_back(RUNNING);
            std::sort(BLOCKED.begin(), BLOCKED.end(), sortByIOTimeLeft);

        } else {
            // no more bursts, complete
            pState = TERMINATED;
            printProcessState(pState, simulation_timer, RUNNING, &READY, algoUsed);

            COMPLETE.push_back(RUNNING);

            contextSwitchTime(false);
        }
        // handle turnaround after tcs out
    }
    TURNAROUND.push_back(RUNNING->getTurnaround());
    RUNNING->turnAReset();
	WAIT.push_back(RUNNING->getWaitTime());
	RUNNING->resetWait();
    RUNNING = NULL;
    return true;
}

bool Scheduler::switchIN() {
    // return true if actually switched something in
    bool preemptAfter = false;

    // PRINT HERE: time 160ms: Process B (tau 100ms) started using the CPU with 85ms burst remaining [Q <empty>]
    if (READY.empty()) {
        // just exit
        return false;
    }

    if (!RUNNING) {
        RUNNING = READY.front();
        READY.pop_front();
    }

    if (hasTimeSlice) {
        remainingtimeslice = timeslice;
    }

    // something is supposed to preempt after, need to call contextSwitch again
    preemptAfter = contextSwitchTime(true);

    pState = START;
    printProcessState(pState, simulation_timer, RUNNING, &READY, algoUsed);
    burstTimeStart = simulation_timer; // burst starts after the time to switch in
    return preemptAfter;
}

void Scheduler::contextSwitch(bool swo, bool swi, bool forcePrint) {
    ++numCS;
#ifdef DEBUG_MODE
    printf("numCS %d\n", numCS);
#endif

    // SWITCH OUT
    if (swo) {
        if (switchOUT(forcePrint)) {
            // do we need to do anything if something is or is not switched out?
        }
    }

    // SWITCH IN
    if (swi) {
        if (switchIN()) {
            // calls contextSwitchTime inside to print 'started using CPU' after C/S in
            // need to process if something was supposed to preempt but could not because switch IN began
            contextSwitch(true, true, true);
            return;
        }
    }

    if (hasTimeSlice) {
        remainingtimeslice = timeslice;
    }

    return;
}

void Scheduler::processArrival(Process * newProcess) {
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
    enum eventType type;

    // check burst complete
    type = burstDone;
    if (RUNNING) {
        storeEventIfSooner(nxtEvnts, RUNNING->burstTimeLeft(), type);
    }

    // check io complete
    type = ioDone;
    for (Process* p : BLOCKED) {
        if (p->getNumIOLeft()) {
            storeEventIfSooner(nxtEvnts, p->ioTimeLeft(), type);
        }
    }

    // check arriving processes
    type = arrival;
    for (Process* p : ARRIVAL) {
        // TODO: quit after arrival is no longer the same value?
        // that's mostly for efficiency, who cares atm
        storeEventIfSooner(nxtEvnts, p->getArrival(), type);
    }

    // check timeslice if algo is timeslice based
    type = tslice;
    if (this->hasTimeSlice) {
        storeEventIfSooner(nxtEvnts, remainingtimeslice, type);
    }

    return nxtEvnts;
}

void Scheduler::updateTimers(unsigned int deltaT) {
    simulation_timer += deltaT;

    // cpu burst
    if (RUNNING) {
        RUNNING->doWork(deltaT);
        RUNNING->turnA(deltaT);

        if (hasTimeSlice) {
            // if timeslice based, less time in slice left
            remainingtimeslice -= deltaT;
        }
    }

    // wait times
    for (Process* p : READY) {
        if (p != RUNNING) {
            p->waitTime(deltaT);
        }
    }

    // io block
    for (Process* p: BLOCKED) {
        p->doIO(deltaT);
    }

    // arriving processes
    for (Process* p: ARRIVAL) {
        p->advanceArrival(deltaT);
    }

}

void Scheduler::fastForward(std::vector<Event> & nxtEvnts) {

    updateTimers(nxtEvnts[0].timeToEvent);

    Process * runstart = RUNNING;

    bool sout = false;
    bool sin = false;
    bool gotTSlice = false;

    // updated time, handle the incoming events
    for (const Event& evnt : nxtEvnts) {
        // this requires that nextEvents really has everything covered
        switch(evnt.type) {

            case burstDone:
                // CPU burst finished, switch OUT
				switchOUT();
                sout = true;
                break;

            case ioDone:
				if (BLOCKED.front()->ioTimeLeft())
					break;

                // add to READY
                BLOCKED.front()->finishedIOBlock();
                READY.push_back(BLOCKED.front());

                // sort if needed
                if (useTau) {
                    std::sort(READY.begin(), READY.end(), sortByTau);
                }

                // would it preempt?
                if (isPreemptive && RUNNING
                        && READY.front() == BLOCKED.front()) {
                    if (useTau && READY.front()->getTau() < RUNNING->tauEffective()) {
                        // will preempt, print that here

                        sout = true;
                        sin = true;
                    }
                }
                if(sout && sin){
                    pState = IOPREEMPT;
                    printProcessState(pState, simulation_timer, BLOCKED.front(), &READY, algoUsed, 0, RUNNING);

                } else{
                    // print i/o complete
                    pState = IOCOMPLETED;
                    printProcessState(pState, simulation_timer, BLOCKED.front(), &READY);
                }


                // remove element from BLOCKED
                BLOCKED.pop_front();
                break;

            case arrival:
				if (ARRIVAL.front()->getArrival())
					break;

                // add to READY
                if (rraddbgn) {
                    READY.push_front(ARRIVAL.front());
                } else {
                    READY.push_back(ARRIVAL.front());
                }

                // sort if needed
                if (useTau) {
                    std::sort(READY.begin(), READY.end(), sortByTau);
                }

                pState = ARRIVE;
                printProcessState(pState, simulation_timer, ARRIVAL.front(), &READY, algoUsed);

                // would it preempt?
                if (isPreemptive && RUNNING
                        && READY.front() == ARRIVAL.front()) {
                    if (useTau && READY.front()->getTau() < RUNNING->tauEffective()) {
                        // will preempt, print that here
                        sout = true;
                        sin = true;
                    }
                }

                // remove element from ARRIVAL
                ARRIVAL.pop_front();
                break;

            case tslice:
                // preemptive by nature
                if (!RUNNING) {
                    // nothing RUNNING, swap in?
                    //                    if (!READY.empty()) {
                    // no preemption
                    //                   } else {
                    // preempt
                    sin = true;
                    gotTSlice = true;
                    //                  }
                } else {
                    // running process
                    if (!READY.empty()) {
                        // will preempt here
                        sout = true;
                        sin = true;
                        gotTSlice = true;
                    } else {
                        // nothing on READY, don't preempt
                        // print here?

                        pState = TIMESLICE;
                        printProcessState(pState, simulation_timer, RUNNING, &READY, algoUsed);
                    }
                }

                // reset timeslice
                remainingtimeslice = timeslice;
                break;
        }
    }

    if (sout) {
        switchOUT();
    }

    if (!RUNNING) {
        sin = true;
    } else if (isPreemptive && !READY.empty()) {
        if (useTau && READY.front()->getTau() < RUNNING->tauEffective()) {
            // preempt
            sin = true;
        } else if (hasTimeSlice && gotTSlice) {
            sin = true;
        }
    }
    if (sin) {
        if (switchIN()) {
            // supposed to preempt if switchIN ret true
            contextSwitch(true, true, true);
        }
    }


    if (runstart != RUNNING && RUNNING != NULL) {
        ++numCS;
    }

    return;
}

bool Scheduler::simDone() {
    return (!RUNNING
            && READY.empty()
            && ARRIVAL.empty()
            && BLOCKED.empty());
}

void Scheduler::statCollection() {
    // preemptions and numCS should already be handled

    // calculate avg wait
    if (WAIT.size()) {
        // unsigned int nproc = COMPLETE.size();
        for (unsigned int wTime: WAIT) {
            avgwait += wTime;
        }
		avgwait /= WAIT.size();
    }

    if (BURSTS.size()) { // not super necessary, but just for safety
        for (unsigned int btime : BURSTS) {
            avgburst += btime;
        }
        avgburst /= BURSTS.size();
    }

    if (TURNAROUND.size()) { // not super necessary, but just for safety
        for (unsigned int ta : TURNAROUND) {
            avgturnaround += ta;
        }
        avgturnaround /= TURNAROUND.size();
    }
}


bool Scheduler::advance() {
    // return true if advanced onwards
    // return false if finished
    // first check if we're finished with the simulation
    if (simDone()) {
        // everything empty, we're done here, simulation is over
        // call stat checking functions? -> calculate avg wait, avg burst, avg turnaround, #preemptions, #cs?
        statCollection();
        return false;
    }

    // advance to next event
    std::vector<Event> thingsHappening = nextEvents();
    if (thingsHappening.empty()) {
        // this should not happen, due to the check from simDone()
        return false;
    } else { // things are happening, so go do them
        //Printing statement 
        //fastForward(thingsHappening[0].timeToEvent);
        fastForward(thingsHappening);
        return true;
    }
}

unsigned long Scheduler::getTimer() {
    return this->simulation_timer;
}

void Scheduler::runSimulation(std::string algo) {
    this->setAlgorithm(algo);
    while(advance()) {
        // #ifdef DEBUG_MODE
        // printf("Advancing simulation in loop!\n");
        // #endif
    }
    printf("time %ldms: Simulator ended for %s ", this->simulation_timer, algo.c_str());
    printSimQ(&READY);
}

void Scheduler::printStats(std::string algo) {

    FILE *sim_stats = fopen("simout.txt", "a");
    if(sim_stats == NULL){
        perror("ERROR: Could not open file simout.txt");
        exit(1);
    }

    fprintf(sim_stats, "Algorithm %s\n", algo.c_str());
    fprintf(sim_stats, "-- average CPU burst time: %.3f ms\n", avgburst);
    fprintf(sim_stats, "-- average wait time: %.3f ms\n", avgwait);
    fprintf(sim_stats, "-- average turnaround time: %.3f ms\n", avgturnaround);
    fprintf(sim_stats, "-- total number of context switches: %d\n", numCS);
    fprintf(sim_stats, "-- total number of preemptions: %u\n", preemptions);

    fclose(sim_stats);

}
