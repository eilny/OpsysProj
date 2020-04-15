#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <deque>
#include <math.h>
#include <string>
#include "process.h"


//for picking out process names  
std::string processName = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

//Get arrival time
float getTime(float lambda, float max) {
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

