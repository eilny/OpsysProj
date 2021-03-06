#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <deque>
#include <math.h>
#include <string>
#include <vector>
#include <queue>

#include "process.h"
#include "scheduler.h"


//for picking out process names  
std::string processName = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";


//Get arrival time (and other times I think) remember to floor the returned value for the correct values 
float getTime(float lambda, float max){
	float x, r;
	r = drand48();
	x = -log(r) / lambda;
	while( x > max ){
		r = drand48();
		x = -log(r) / lambda;
	}
	return x;
}

//get Burst Time 
int getNumBurst() {
	float r = drand48();
	return (int) std::ceil((r * 100));
}

//Setups up process list 
void getProcessList(int seed, float lambda, int maxTime, std::vector<Process*> *processes, int numProcess, float alpha){
	srand48(seed); //Seeds the rng 
	//Fills in each process for total process 
	for(int i = 0; i < numProcess; i++){
		int arr = floor(getTime(lambda, maxTime));
		int bursts = getNumBurst();
		Process *temp = new Process(processName[i], arr, bursts, lambda, alpha);
		for(int j = 0; j < bursts; j++){
			temp->addBurst(ceil(getTime(lambda, maxTime)));
			if ( j == (bursts - 1)){
				break;
			}
			temp->addIo(ceil(getTime(lambda, maxTime)));
		}
		processes->push_back(temp);
		// delete temp;
	}
	
} 

//Prints Starting Arrival list 
void printArrivalList(std::vector<Process*> processes, int numProcess, float tau = 0){
	for(int i = 0; i < numProcess; ++i){
		if (tau != 0){
			if(processes[i]->getNumBursts() == 1){
				fprintf(stdout, "Process %c [NEW] (arrival time %d ms) %d CPU burst (tau %.0fms)\n"
					, processes[i]->getId()
					, processes[i]->getArrival()
					, processes[i]->getNumBursts()
					, tau);
			}else{
				fprintf(stdout, "Process %c [NEW] (arrival time %d ms) %d CPU bursts (tau %.0fms)\n"
					, processes[i]->getId()
					, processes[i]->getArrival()
					, processes[i]->getNumBursts()
					, tau);
			}
		}
		else{
			fprintf(stdout, "Process %c [NEW] (arrival time %d ms) %d CPU "
				, processes[i]->getId()
				, processes[i]->getArrival()
				, processes[i]->getNumBursts());
			if(processes[i]->getNumBursts() > 1){
				printf("bursts\n");
			}else{
				printf("burst\n");
			}
		}
	}
	fflush(stdout);
}

void clearProcesses(std::vector<Process *> * procs)
{
	for (Process * p: *procs) {
		delete p;
	}
}

int main( int argc, char ** argv) {

    if (argc < 8 || argc > 9) {
        fprintf(stderr, "USAGE: ./a.out <seed> <lambda> <upper bound> < number of processes> <context switch time (even)> <alpha > <RR time slice> [<RR [BEGINNING|END]>]\n");
        return EXIT_FAILURE;
    }

    // Submitty:
#ifndef DEBUG_MODE
    setvbuf( stdout, NULL, _IONBF, 0 );
#endif

    // argv[1] - seed
    int seed = atoi(argv[1]);
    // argv[2] - lamda (interval times)
    float lambda = atof(argv[2]);
    // argv[3] - upper bound
    unsigned int upperbound = atoi(argv[3]);
    // argv[4] - number of processes to simulate
    unsigned int nproc = atoi(argv[4]);
    // argv[5] - context switch time (ms) - expects even value
    unsigned int tcs = atoi(argv[5]);
    // argv[6] - alpha (cpu burst time estimate)
    float alpha = atof(argv[6]);
    // argv[7] - RR timeslice
    unsigned int timeslice = atoi(argv[7]);
    // argv[8] - rradd: place processes at beginning or end of queue
	bool rraddbgn = false;
	if (argc == 9){
		if (0 == strcmp("BEGINNING", argv[8])) {
			rraddbgn = true;
		} else if (0 == strcmp("END", argv[8])) {
			rraddbgn = false;
		} else {
			fprintf(stderr, "USAGE: ./a.out <seed> <lambda> <upper bound> < number of processes> \
					<context switch time (even)> <alpha > <RR time slice> <RR [BEGINNING|END]>\n");
			return EXIT_FAILURE;
		}
	}

#ifdef DEBUG_MODE
    fprintf(stdout, "args:  seed: %d lambda: %f upper bound: %u nproc: %u tcs: %u alpha: %f rrtimeslice: %u rr begin: %u %s\n"
            , seed
            , lambda
            , upperbound
            , nproc
            , tcs
            , alpha
            , timeslice
            , rraddbgn
            , (rraddbgn ? "BEGINNING" : "END") );
#endif



    // set up queues
	
	std::vector<Process*> *baseProcesses = new std::vector<Process*>();

	// Get Processes List 
	getProcessList(seed, lambda, upperbound, baseProcesses, nproc, alpha);
	
	
    // Run simulations
	//FCFS
    Scheduler *FCFS = new Scheduler(baseProcesses, tcs, timeslice, rraddbgn);
	printArrivalList(*baseProcesses, nproc);
	FCFS->runSimulation("FCFS");
	printf("\n");
	

	clearProcesses(baseProcesses);
	delete baseProcesses;
	baseProcesses = new std::vector<Process*>();
	getProcessList(seed, lambda, upperbound, baseProcesses, nproc, alpha);
	printArrivalList(*baseProcesses, nproc, 1/lambda);
	//SJF
	Scheduler *SJF = new Scheduler(baseProcesses, tcs, timeslice, rraddbgn);
	SJF->runSimulation("SJF");
	printf("\n");
	
	
	clearProcesses(baseProcesses);
	delete baseProcesses;
	baseProcesses = new std::vector<Process*>();
	getProcessList(seed, lambda, upperbound, baseProcesses, nproc, alpha);
	printArrivalList(*baseProcesses, nproc, 1/lambda);
	//SRT
	Scheduler *SRT = new Scheduler(baseProcesses, tcs, timeslice, rraddbgn);
	SRT->runSimulation("SRT");
	printf("\n");
	
	
	clearProcesses(baseProcesses);
	delete baseProcesses;
	baseProcesses = new std::vector<Process*>();
	getProcessList(seed, lambda, upperbound, baseProcesses, nproc, alpha);
	printArrivalList(*baseProcesses, nproc);
	//RR
	Scheduler *RR = new Scheduler(baseProcesses, tcs, timeslice, rraddbgn);
	RR->runSimulation("RR");

	clearProcesses(baseProcesses);
	delete baseProcesses;
	// Stat Stuff
	FCFS->printStats("FCFS");
	SJF->printStats("SJF");
	SRT->printStats("SRT");
	RR->printStats("RR");
	
	
	
	delete FCFS;
	delete SJF;
	delete SRT;
	delete RR;

	
}

