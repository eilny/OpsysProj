#include <stdint.h>
#include <queue>

#ifndef _H_PROCESS_H_
#define _H_PROCESS_H_

class Process {
    private:
        char process_ID; 
        unsigned int arrival_time; 
        unsigned int num_bursts;
        unsigned int remaining_time; //Remaining time for popped burst time (processed time doesn't help much)

        unsigned int turnaround_time;
        unsigned int wait_time;

        std::queue<unsigned int> * burst_times;
        std::queue<unsigned int> * io_times;

        enum State {
            READY,
            RUNNING,
            BLOCKED };
			
		State state;
		
	public:
	//Get & Set functions for accessing the variables 	
        Process(char pid, unsigned int arr, unsigned int processed);
        ~Process();
        void contextSwitch(bool switch_in, unsigned int tcshalf);
		void addBurst(unsigned int time);
		void addIo(unsigned int time);
		char getId();
		unsigned int getArrival();
		unsigned int getNumBursts();
		
};
#endif
