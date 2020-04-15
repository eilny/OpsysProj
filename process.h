#include <stdint.h>

#ifndef _H_PROCESS_H_
#define _H_PROCESS_H_

class process {
    private:
        unsigned int process_ID;
        unsigned int arrival_time;
        unsigned int processing_time;
        unsigned int processed_time;

        unsigned int turnaround_time;
        unsigned int wait_time;
};
#endif
