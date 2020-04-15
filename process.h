#include <stdint.h>

#ifndef _H_PROCESS_H_
#define _H_PROCESS_H_

class Process {
    private:
        unsigned int process_ID;
        unsigned int arrival_time;
        unsigned int processed_time;

        unsigned int turnaround_time;
        unsigned int wait_time;

        std::queue<unsigned int> burst_times;
        std::queue<unsigned int> io_times;

        enum State {
            READY,
            RUNNING,
            BLOCKED }
		
	public:
	//Get & Set functions for accessing the variables 	
        Process(unsigned int pid, unsigned int arr, unsigned int processed);
        ~Process();
        void contextSwitch(bool switch_in, unsigned int tcshalf);
};
#endif
