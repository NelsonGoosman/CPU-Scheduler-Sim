#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "rand.h"
#include "utils.h"
#include "process_sim.h"
#include "process_structs.h"

void init_process_list(const int num_process, _process *processes, char *line, FILE *rand_num_file);
void run_simulation(_process *processes, int process_count, int quantum);

#endif