#ifndef PROCESSSIM_H
#define PROCESSSIM_H
#include "scheduler.h"
#include "process_structs.h"

int cmp_process(const void *a, const void *b);
scheduler_result first_come_first_serve(_process *processes, uint32_t process_cnt);
scheduler_result shortest_job_first(_process *processes, uint32_t process_cnt);
scheduler_result round_robin(_process *processes, uint32_t process_cnt, uint32_t quantum);
void calculate_burst(_process *process);

#endif