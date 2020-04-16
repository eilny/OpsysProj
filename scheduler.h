#include <vector>
#include <queue>
#include "Process.h"

#ifndef _H_SCHEDULER_H_
#define _H_SCHEDULER_H_

// parent class for scheduling algorithms
class Scheduler {
    private:
        std::vector<Process> Incoming;

        std::vector<Process> READY;
        std::vector<Process> BLOCKED;
        std::vector<Process>::iterator RUNNING;
        std::vector<Process>::iterator IO;

        // simulation timer
        unsigned long simulation_timer = 0;

        float avgwait;
        unsigned int preemptions;
        unsigned int tcs;

        bool hasTimeSlice;
        unsigned int timeslice;
		unsigned int rraddbgn;
		
		// bool sortByArrvial(Process a, Process b);

    public:
		//Constructor
		Scheduler(std::vector<Process> processList,
			unsigned int tcontext,
			unsigned int tmslice, 
			unsigned int rr);
	
        void contextSwitch(Process toIO, Process toCPU);
        void processArrival(Process newProcess);
        unsigned int timeToNextEvent();
        void advance();
		unsigned long getTimer();
		
};

#endif
