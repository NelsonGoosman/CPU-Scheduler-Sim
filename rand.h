#ifndef RAND_H
#define RAND_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t getRandNumFromFile(uint32_t line, FILE *random_num_file_ptr);

uint32_t randomOS(uint32_t upper_bound, uint32_t process_indx, FILE *random_num_file_ptr);

#endif
