#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "process_structs.h"

void printStart(_process process_list[], uint32_t count);
void printFinal(_process finished_process_list[], scheduler_result result);
void printProcessSpecifics(_process process_list[], scheduler_result result);
void printSummaryData(_process process_list[], scheduler_result result);

#endif
