#include <stdint.h>
#include <queue>

#ifndef _H_PROCESS_H_
#define _H_PROCESS_H_

enum State {
    RDY, // ready for CPU
    RUN, // running on CPU
    BLK, // blocked on I/O
    CMP  // completed
};

class Process {
    private:
        char process_ID; 
        unsigned int arrival_time; 

        unsigned long turnaround_time;
        unsigned long wait_time;
		
		float tau;
		float alpha;

        std::vector<unsigned int> * burst_times;
        std::vector<unsigned int> * io_times;

        // not to be messed with after creation
        // needed for stats at end, determining turnaround/wait as well
        unsigned int pristine_arrival_time; 
        std::vector<unsigned int> * pristine_burst_times;
        std::vector<unsigned int> * pristine_io_times;

		State state;

	public:
	//Get & Set functions for accessing the variables 	
        Process(char pid, unsigned int arr, unsigned int processed, float lambda, float alp);
        ~Process();
        void contextSwitch(bool switch_in);
		void addBurst(unsigned int time);
		void addIo(unsigned int time);
		
		char getId();
		unsigned int getArrival();
		unsigned int getNumBursts();
		unsigned int getNumBurstsLeft();
		unsigned int getNumIOLeft();
		float getTau();

        State setState(State newstate);
        unsigned int burstTimeLeft();
        unsigned int ioTimeLeft();
        bool doWork(unsigned int deltaT);
        bool doIO(unsigned int deltaT);
		void recalculateTau(int burstTime);
        bool advanceArrival(unsigned int deltaT);
		void setTau(bool useTau);
        void waitTime(unsigned int deltaT);

        void finishedCPUBurst();
        void finishedIOBlock();
        void setTurnAround(unsigned long runTimeEnd);

};
#endif
