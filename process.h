#include <stdint.h>
#include <queue>

#ifndef _H_PROCESS_H_
#define _H_PROCESS_H_

enum State {
    RDY,
    RUN,
    BLK
};

class Process {
    private:
        char process_ID; 
        unsigned int arrival_time; 
        unsigned int num_bursts;

        unsigned int turnaround_time;
        unsigned int wait_time;

        std::vector<unsigned int> * burst_times;
        std::vector<unsigned int> * io_times;

			
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
        State setState(State newstate);
        unsigned int burstTimeLeft();
        unsigned int ioTimeLeft();
        void doWork(unsigned int deltaT);
        void doIO(unsigned int deltaT);
		
};
#endif
