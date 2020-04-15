#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <deque>
#include <math.h>
#include <string>
#include "process.h"


//for picking out process names  
std::string processName = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

//For picking print statements
enum PrintState {ARRIVE, START, COMPLETED, BLOCK, IOCOMPLETED, TAU, TERMINATED};

//Get arrival time (and other times I think) remember to floor the returned value for the correct values 
float getTime(float lambda, float max){
	float x, r;
	r = drand48();
	x = -log(r) / lambda;
	while( x > max ){
		r = drand48();
		x = -(log(r)) / lambda;
	}
	return x;
}

//get Burst Time 
int getNumBurst() {
	float r = drand48();
	return (int) std::ceil((r * 100));
}

// Printing statements 
// Needs to be modified for process class
// void printProcessState(PrintState p, int time, process cur, float tau){
	
	// if( p == ARRIVE ){
		// if(0 != tau){
			// printf("time %dms: Process %c (tau %.0fms) arrived; added to ready queue ", time, cur.getId(), tau);
		// }else{
			// printf("time %dms: Process %c arrived; added to ready queue ", time, cur.getId());
		// }
	// }
	// if(p == START){
		// printf("time %dms: Process %c started using the CPU for %dms burst ", time, cur.getId(), cpu);
	// }
	// if(p == COMPLETED){
		// printf("time %dms: Process %c completed a CPU burst; %d bursts to go ", time, cur.getId(), cur.getRemainBurst());
	// }
	// if(p == BLOCK){
		// printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms", time, cur.getId(), time+io);
	// }
	// if(p == IOCOMPLETED){
		// if(0 != tau){
			// printf("time %dms: Process %c (tau %.0fms) completed I/O; added to ready queue ", time, cur.getId(), tau);
		// }else{
			// printf("time %dms: Process %c completed I/O; added to ready queue ", time, cur.getId());
		// }
	// }
	// if(p == TAU){
		// printf("time %dms: Recalculated tau = %.0fms for process %c ", time, tau, cur.getId());
	// }
	// if(p == TERMINATED){
		// printf("time %dms: Process %c terminated ", time, cur.getId());
	// }
	// fflush(stdout);
// }


int main( int argc, char ** argv) {

    if (argc < 9) {
        fprintf(stderr, "USAGE: ./a.out <seed> <lambda> <upper bound> < number of processes> \
                <context switch time (even)> <alpha > <RR time slice> <RR [BEGINNING|END]>\n");
        return EXIT_FAILURE;
    }

    // Submitty:
    setvbuf( stdout, NULL, _IONBF, 0 );

    // argv[1] - seed
    int seed = atoi(argv[1]);
    // argv[2] - lamda (interval times)
    int lambda = atoi(argv[2]);
    // argv[3] - upper bound
    unsigned int upperbound = atoi(argv[3]);
    // argv[4] - number of processes to simulate
    unsigned int nproc = atoi(argv[4]);
    // argv[5] - context switch time (ms) - expects even value
    unsigned int tcs = atoi(argv[5]);
    // argv[6] - alpha (cpu burst time estimate)
    unsigned int alpha = atoi(argv[6]);
    // argv[7] - RR timeslice
    unsigned int timeslice = atoi(argv[7]);
    // argv[8] - rradd: place processes at beginning or end of queue
    unsigned int rraddbgn;
    if (0 == strcmp("BEGINNING", argv[8])) {
        rraddbgn = 1;
    } else if (0 == strcmp("END", argv[8])) {
        rraddbgn = 0;
    } else {
        fprintf(stderr, "USAGE: ./a.out <seed> <lambda> <upper bound> < number of processes> \
                <context switch time (even)> <alpha > <RR time slice> <RR [BEGINNING|END]>\n");
        return EXIT_FAILURE;
    }

#ifdef DEBUG_MODE
    fprintf(stdout, "args:  seed: %d lambda: %d upper bound: %u nproc: %u tcs: %u alpha: %u rrtimeslice: %u rr begin: %u %s\n"
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
    std::deque<int> READY;
    std::deque<int> RUNNING;
    std::deque<int> BLOCKED;
	
	

    /* Run simulations
	*/

}

