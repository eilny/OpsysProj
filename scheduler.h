#ifndef _H_SCHEDULER_H_
#define _H_SCHEDULER_H_

// parent class for scheduling algorithms
class Scheduler {
    private:
        std::vector<Process> READY;
        std::vector<Process> RUNNING;
        std::vector<Process> BLOCKED;

        float avgwait;
        unsigned int preemptions;
    public:
}

#endif
