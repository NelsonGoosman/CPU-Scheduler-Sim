#include "process_sim.h"

#define iterate(memhead, offset, i, memsize) &memhead[((offset - memhead) + i) % memsize] // iterates through an array in a circle, so values greater than len(array) will wrap around

void calculate_burst(_process *process)
{
    FILE *rand_file = fopen("random-numbers", "r");
    process->CPUBurst = randomOS(process->B, process->processID, rand_file);
    fclose(rand_file);
    process->IOBurst = process->CPUBurst * process->M;
}

int cmp_process(const void *a, const void *b)
{
    _process *process_a = (_process *)a;
    _process *process_b = (_process *)b;
    return process_a->A - process_b->A;
}

scheduler_result first_come_first_serve(_process *processes, uint32_t process_cnt)
{
    scheduler_result res = {0, 0, 0, 0, 0}; // result of the simulation

    qsort(processes, process_cnt, sizeof(_process), cmp_process); // sort by arrival time

    _process *current_p = NULL;       // current process
    _process *running_process = NULL; // process that is running
    while (res.TOTAL_FINISHED_PROCESSES < process_cnt)
    {
        _process *mem_offset = (running_process == NULL) ? processes : running_process;
        for (int i = 0; i < process_cnt; i++)
        {
            current_p = iterate(processes, mem_offset, i, process_cnt);
            // terminated process, skip to next iteration
            if (current_p->status == TERMINATED)
            {
                continue;
            }

            // blocked, run IO burst
            if (current_p->status == BLOCKED)
            {
                // incriment the ioblocked time and decriment time left on io burst
                current_p->currentIOBlockedTime++;
                current_p->IOBurst--;
                // incriment the total cycle blocked result
                res.TOTAL_NUMBER_OF_CYCLES_SPENT_BLOCKED++;
                // if io burst is 0 then the process is ready otherwise its blocked
                current_p->status = (current_p->IOBurst == 0) ? READY : BLOCKED;
            }
            // unstarted, but the process is ready
            else if (current_p->status == UNSTARTED && current_p->A == res.CURRENT_CYCLE)
            {
                // initialize process info for new process (the rest is already initialized by the constructor function init process called in main)
                current_p->isFirstTimeRunning = true;
                current_p->status = READY;
                // update result data
                res.TOTAL_CREATED_PROCESSES++;
            }
            // ready
            if (current_p->status == READY)
            {
                if (running_process == NULL)
                {
                    calculate_burst(current_p);
                    current_p->status = RUNNING;
                    running_process = current_p;
                }
                else
                {
                    current_p->currentWaitingTime++;
                }
            }
            // running
            else if (current_p->status == RUNNING)
            {
                current_p->currentCPUTimeRun++;
                current_p->CPUBurst--;
                if (current_p->isFirstTimeRunning)
                    res.TOTAL_STARTED_PROCESSES++;
                current_p->isFirstTimeRunning = false;

                if (current_p->currentCPUTimeRun >= current_p->C) // cpu runtime is greater than or equal to the runtime of the process, terminate
                {
                    current_p->status = TERMINATED;
                    current_p->finishingTime = res.CURRENT_CYCLE;
                    running_process = NULL;
                    res.TOTAL_FINISHED_PROCESSES++;
                }
                else if (current_p->CPUBurst <= 0) // otherwise block the process and do some io burst
                {
                    running_process = NULL;
                    current_p->status = BLOCKED;
                }
            }
        }

        res.CURRENT_CYCLE++;
    }

    return res;
}

scheduler_result shortest_job_first(_process *processes, uint32_t process_cnt)
{
    scheduler_result res = {0, 0, 0, 0, 0};                       // result of the simulation
    qsort(processes, process_cnt, sizeof(_process), cmp_process); // sort by arrival time
    _process *process = NULL;                                     // current process

    while (res.TOTAL_FINISHED_PROCESSES < process_cnt)
    {
        _process *shortest = NULL;

        for (int i = 0; i < process_cnt; i++)
        {
            process = &processes[i];

            if (process->status == TERMINATED)
            {
                continue;
            }

            // blocked, run IO burst
            if (process->status == BLOCKED)
            {
                // incriment the ioblocked time and decriment time left on io burst
                process->currentIOBlockedTime++;
                process->IOBurst--;
                // incriment the total cycle blocked result
                res.TOTAL_NUMBER_OF_CYCLES_SPENT_BLOCKED++;
                // if io burst is 0 then the process is ready otherwise its blocked
                process->status = (process->IOBurst > 0) ? BLOCKED : READY;
            }
            // unstarted, but the process is ready
            else if (process->status == UNSTARTED && process->A == res.CURRENT_CYCLE)
            {
                // initialize process info for new process (the rest is already initialized by the constructor function init process called in main)
                process->isFirstTimeRunning = true;
                process->status = READY;
                // update result data
                res.TOTAL_CREATED_PROCESSES++;
            }
            // ready
            if (process->status == READY)
            {
                if (shortest == NULL) // if shortest is null, than the shortest job is the current process
                {
                    shortest = process;
                }
                else
                { // compare remaining cpu time of current process with the shortest job found so far. if the current process is shorter than the current shortest process
                    // reassign the shortest process to the current process
                    if ((process->C - process->currentCPUTimeRun) < (shortest->C - shortest->currentCPUTimeRun))
                    {
                        shortest = process;
                    }
                }
            }
            // running
            else if (process->status == RUNNING)
            {
                process->currentCPUTimeRun++;
                process->CPUBurst--;
                if (process->isFirstTimeRunning)
                    res.TOTAL_STARTED_PROCESSES++;
                process->isFirstTimeRunning = false;

                if (process->currentCPUTimeRun >= process->C) // cpu runtime is greater than or equal to the runtime of the process, terminate
                {
                    res.TOTAL_FINISHED_PROCESSES++;
                    process->status = TERMINATED;
                    process->finishingTime = res.CURRENT_CYCLE;
                }
                else if (process->CPUBurst <= 0) // otherwise block the process and do some io burst
                {
                    process->status = BLOCKED;
                }
            }
        }

        if (shortest != NULL)
        {
            calculate_burst(shortest);
            shortest->status = RUNNING;
            shortest->currentWaitingTime--;
        }
        res.CURRENT_CYCLE++;
    }

    return res;
}

scheduler_result round_robin(_process *processes, uint32_t process_cnt, uint32_t quantum)
{
    scheduler_result res = {0, 0, 0, 0, 0}; // result of the simulation

    qsort(processes, process_cnt, sizeof(_process), cmp_process); // sort by arrival time

    _process *current_p = NULL;       // current process
    _process *running_process = NULL; // process that is running

    while (res.TOTAL_FINISHED_PROCESSES < process_cnt)
    {
        _process *mem_offset = (running_process == NULL) ? processes : running_process;
        for (int i = 0; i < process_cnt; i++)
        {
            current_p = iterate(processes, mem_offset, i, process_cnt);
            // terminated process, skip to next iteration
            if (current_p->status == TERMINATED)
            {
                continue;
            }

            // blocked, run IO burst
            if (current_p->status == BLOCKED)
            {
                // incriment the ioblocked time and decriment time left on io burst
                current_p->currentIOBlockedTime++;
                current_p->IOBurst--;
                // incriment the total cycle blocked result
                res.TOTAL_NUMBER_OF_CYCLES_SPENT_BLOCKED++;
                // if io burst is 0 then the process is ready otherwise its blocked
                current_p->status = (current_p->IOBurst == 0) ? READY : BLOCKED;
            }
            // unstarted, but the process is ready
            else if (current_p->status == UNSTARTED && current_p->A == res.CURRENT_CYCLE)
            {
                // initialize process info for new process (the rest is already initialized by the constructor function init process called in main)
                current_p->isFirstTimeRunning = true;
                current_p->status = READY;
                // update result data
                res.TOTAL_CREATED_PROCESSES++;
            }
            // ready
            if (current_p->status == READY)
            {
                if (running_process == NULL)
                {
                    calculate_burst(current_p);
                    current_p->status = RUNNING;
                    running_process = current_p;
                }
                else
                {
                    current_p->currentWaitingTime++;
                }
            }
            // running
            else if (current_p->status == RUNNING)
            {
                current_p->currentCPUTimeRun++;
                current_p->CPUBurst -= quantum;
                // if (current_p->CPUBurst < 0)
                //     current_p->CPUBurst = 0;
                if (current_p->isFirstTimeRunning)
                    res.TOTAL_STARTED_PROCESSES++;
                current_p->isFirstTimeRunning = false;

                if (current_p->currentCPUTimeRun >= current_p->C) // cpu runtime is greater than or equal to the runtime of the process, terminate
                {
                    current_p->status = TERMINATED;
                    current_p->finishingTime = res.CURRENT_CYCLE;
                    running_process = NULL;
                    res.TOTAL_FINISHED_PROCESSES++;
                }
                else // otherwise block the process and do some io burst
                {
                    running_process = NULL;
                    current_p->status = BLOCKED;
                }
            }
        }

        res.CURRENT_CYCLE++;
    }

    return res;
}