#include "rand.h"

static const uint32_t SEED_VALUE = 200; // Seed value for reading from file

/**
 * Reads a random non-negative integer X from a file with a given line named random-numbers (in the current directory)
 */
uint32_t getRandNumFromFile(uint32_t line, FILE *random_num_file_ptr)
{
    uint32_t end, loop;
    char str[512];

    rewind(random_num_file_ptr); // reset to be beginning
    for (end = loop = 0; loop < line; ++loop)
    {
        if (0 == fgets(str, sizeof(str), random_num_file_ptr))
        {            // include '\n'
            end = 1; // can't input (EOF)
            break;
        }
    }
    if (!end)
    {
        return (uint32_t)atoi(str);
    }

    // fail-safe return
    return (uint32_t)1804289383;
}

/**
 * Reads a random non-negative integer X from a file named random-numbers.
 * Returns the CPU Burst: : 1 + (random-number-from-file % upper_bound)
 */
uint32_t randomOS(uint32_t upper_bound, uint32_t process_indx, FILE *random_num_file_ptr)
{
    // char str[20];

    uint32_t unsigned_rand_int = (uint32_t)getRandNumFromFile(SEED_VALUE + process_indx, random_num_file_ptr);
    uint32_t returnValue = 1 + (unsigned_rand_int % upper_bound);

    return returnValue;
}
