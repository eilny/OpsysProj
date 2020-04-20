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
    , cswitch
};

struct Event {
    unsigned int timeToEvent;
    eventType type;
};

//For picking print statements
enum PrintState {ARRIVE, START, COMPLETED, BLOCK, IOCOMPLETED, IOPREEMPT, TAU, TERMINATED, TIMESLICE, PREEMPT, ERROR};


// parent class for scheduling algorithms
class Scheduler {
    private:
        std::deque<Process> ARRIVAL;
        std::deque<Process> READY;
        std::deque<Process> BLOCKED;
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

        bool useTau;
		
		PrintState pState;

    public:
		//Constructor
		Scheduler(std::vector<Process> *processList,
			unsigned int tcontext,
			unsigned int tmslice, 
			unsigned int rr);
        ~Scheduler();
		void setAlgorithm(std::string algo);
	
	
        bool switchOUT();
        bool switchIN();
        void contextSwitchTime(bool switchIN);
        void contextSwitch();
        void processArrival(Process newProcess);
        void storeEventIfSooner(std::vector<Event> & events, unsigned int time, enum eventType type);
        unsigned int nextEvent();
		std::vector<Event> nextEvents();
        bool advance();
        void fastForward(std::vector<Event> nxtEvnts);
		unsigned long getTimer();
		
		void runSimulation(std::string algo);
};

#endif
