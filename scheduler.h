#include <vector>
#include <queue>
#include <string>

#include "process.h"

#ifndef _H_SCHEDULER_H_
#define _H_SCHEDULER_H_

enum eventType {
    burstDone
    , ioDone
    , arrival
    , tslice
    , switchOUT
    , switchIN
};

struct Event {
    unsigned int timeToEvent;
    eventType type;
};

//For picking print statements
enum PrintState {ARRIVE, START, COMPLETED, BLOCK, IOCOMPLETED, TAU, TERMINATED, TIMESLICE, PREEMPT, ERROR};


// parent class for scheduling algorithms
class Scheduler {
    private:
        std::vector<Process> ARRIVAL;
        std::vector<Process> READY;
        std::vector<Process> BLOCKED;
        std::vector<Process> COMPLETE;

        Process* RUNNING;

        // simulation timer
        unsigned long simulation_timer = 0;

        float avgwait;
        float avgburst;
        float avgturnaround;
		

        unsigned int numCS;
        unsigned int tcs;
        unsigned int nextCS; // for detecting next of context switch event

        bool isPreemptive;
        unsigned int preemptions;

        bool hasTimeSlice;
        unsigned int timeslice;
        unsigned int remainingtimeslice; // for detecting end of timeslice
		unsigned int rraddbgn;

        bool sortsByTime;
		
		PrintState pState;
		
		// bool sortByArrvial(Process a, Process b);

    public:
		//Constructor
		Scheduler(std::vector<Process> *processList,
			unsigned int tcontext,
			unsigned int tmslice, 
			unsigned int rr);
		void setAlgorithm(std::string algo);
	
	
        void switchOUT();
        void switchIN();
        void contextSwitchTime(bool switchIN);
        void contextSwitch();
        void processArrival(Process newProcess);
        void storeEventIfSooner(std::vector<Event> & events, unsigned int time, enum eventType type);
        unsigned int nextEvent();
		std::vector<Event> nextEvents();
        bool advance();
        void fastForward(unsigned int deltaT);
		unsigned long getTimer();
		
		void runSimulation(std::string algo);
		
		
};

#endif
