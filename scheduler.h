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
        std::deque<Process*> ARRIVAL;
        std::deque<Process*> READY;
        std::deque<Process*> BLOCKED;
        std::vector<Process*> COMPLETE;

        Process* RUNNING;

        // for stat tracking of avg burst duration
        std::vector<unsigned int> BURSTS;
        unsigned long burstTimeStart;

        // simulation timer
        unsigned long simulation_timer = 0;

        float avgwait;
        float avgburst;
        float avgturnaround;
		
        unsigned int numCS;
        unsigned int tcs;

        bool isPreemptive;
        unsigned int preemptions;

        bool hasTimeSlice;
        unsigned int timeslice;
        unsigned int remainingtimeslice; // for detecting end of timeslice
		bool rraddbgn;

        bool useTau;
		
		PrintState pState;

    public:
		//Constructor
		Scheduler(std::vector<Process*> *processList,
			unsigned int tcontext,
			unsigned int tmslice, 
			unsigned int rr);
        ~Scheduler();
		void setAlgorithm(std::string algo);
	
	
        bool contextSwitchTime(bool switchIN);
        bool switchOUT();
        bool switchIN();
        void contextSwitch();

        void processArrival(Process * newProcess);

        void storeEventIfSooner(std::vector<Event> & events, unsigned int time, enum eventType type);
		std::vector<Event> nextEvents();
        
        bool advance();
        void updateTimers(unsigned int deltaT);
        void fastForward(std::vector<Event> & nxtEvnts);

		unsigned long getTimer();
		
		void runSimulation(std::string algo);
		
		void printStats(std::string algo);
};

#endif
