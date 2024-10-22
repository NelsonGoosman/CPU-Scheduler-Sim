#include "scheduler.h"

#define MAX_LINE_SIZE 256
#define _QUANTUM 2

/*
Input file format: n (A B C M) ...
n = # of processes
A = Arrival time
B = Random integers in the interval (0, B]
C = Total CPU time needed for that process
M = I/O burst time multiplier
*/

/*
Initialize process list function
given a number of proceses, a list of uninitialized processes, the input line from the given input file, and a pointer to the rand file
this function will either calculate needed values to fill the process struct, read them from a file, or set as default values
*/

void init_process_list(const int num_process, _process *processes, char *line, FILE *rand_num_file)
{
    char *ptr = line;
    uint32_t a, b, c, m;

    _process default_process = {
        .A = 0,
        .B = 0,
        .C = 0,
        .M = 0,
        .processID = -1,
        .status = UNSTARTED,
        .finishingTime = 0,
        .currentCPUTimeRun = 0,
        .currentIOBlockedTime = 0,
        .currentWaitingTime = 0,
        .IOBurst = 0,
        .CPUBurst = 0,
        .quantum = 0,
        .isFirstTimeRunning = false};

    // move past the number of processes in input file
    while (*ptr != ' ' && *ptr != '\0')
        ptr++;
    ptr++;

    for (int i = 0; i < num_process; i++) // setting values for each process
    {
        // initialize all process values to default
        processes[i] = default_process;
        // read in a b c m from file and set to values
        sscanf(ptr, "(%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 ")", &a, &b, &c, &m); // %u or "(%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 ")"
        processes[i].A = a;
        processes[i].B = b;
        processes[i].C = c;
        processes[i].M = m;
        // set unique process id
        processes[i].processID = i;
        // calculate cpu and io burst
        processes[i].CPUBurst = randomOS(b, processes[i].processID, rand_num_file);
        processes[i].IOBurst = processes[i].CPUBurst * m;

        // Move pointer past the current tuple
        while (*ptr != ')' && *ptr != '\0')
            ptr++;
        ptr++; // Move past the closing parenthesis
        while (*ptr == ' ')
            ptr++; // Skip whitespace
    }
}

/*
Runs the scheduler simulation for First come first serve, round robin, and shortest job first, then
print information about each simulation
*/
void run_simulation(_process *processes, int process_count, int quantum)
{
    // copying the original initialized process list for each individual simulation so it is not effected for each simulation
    _process *fcfs_copy = (_process *)malloc(sizeof(_process) * process_count);
    _process *sjf_copy = (_process *)malloc(sizeof(_process) * process_count);
    _process *rr_copy = (_process *)malloc(sizeof(_process) * process_count);
    memcpy(fcfs_copy, processes, sizeof(_process) * process_count);
    memcpy(sjf_copy, processes, sizeof(_process) * process_count);
    memcpy(rr_copy, processes, sizeof(_process) * process_count);

    printf("\n*************** START OF FIRST COME FIRST SERVE ***************\n");
    printStart(fcfs_copy, process_count);
    scheduler_result fcfs_res = first_come_first_serve(fcfs_copy, process_count);
    printFinal(fcfs_copy, fcfs_res);
    printProcessSpecifics(fcfs_copy, fcfs_res);
    printSummaryData(fcfs_copy, fcfs_res);
    printf("\n**************** END OF FIRST COME FIRST SERVE ****************\n");

    printf("\n*************** START OF ROUND ROBIN ***************\n");
    printStart(rr_copy, process_count);
    scheduler_result rr_res = round_robin(rr_copy, process_count, quantum);
    printFinal(rr_copy, rr_res);
    printProcessSpecifics(rr_copy, rr_res);
    printSummaryData(rr_copy, rr_res);
    printf("\n**************** END OF ROUND ROBIN ****************\n");

    printf("\n*************** START OF SHORTEST JOB FIRST ***************\n");
    printStart(sjf_copy, process_count);
    scheduler_result sjf_res = shortest_job_first(sjf_copy, process_count);
    printFinal(sjf_copy, sjf_res);
    printProcessSpecifics(sjf_copy, sjf_res);
    printSummaryData(sjf_copy, sjf_res);
    printf("\n**************** END OF SHORTEST JOB FIRST ****************\n");

    printf("SIMULATION COMPLETE\n");

    free(fcfs_copy);
    free(sjf_copy);
    free(rr_copy);
}

int main(int argc, char *argv[])
{
    const char *RANDOM_NUMBER_FILE_NAME = "random-numbers";
    char line_buff[MAX_LINE_SIZE];

    // make sure that there are exactly two command line args (executable name and file path)
    assert(argc == 2);

    FILE *input = fopen(argv[1], "r");
    FILE *rnumfile = fopen(RANDOM_NUMBER_FILE_NAME, "r");

    if (rnumfile == NULL || input == NULL)
    {
        // exit if either file could not be opened
        fprintf(stderr, "Error: file not found or could not be opened.\n");
        return 1;
    }

    // read input file and get total number of processes
    fgets(line_buff, MAX_LINE_SIZE, input);
    int total_num_of_process = 0;
    sscanf(line_buff, "%d", &total_num_of_process);

    _process *process_list = (_process *)malloc(total_num_of_process * sizeof(_process));
    init_process_list(total_num_of_process, process_list, line_buff, rnumfile);

    run_simulation(process_list, total_num_of_process, _QUANTUM);

    fclose(rnumfile);
    fclose(input);
    free(process_list);

    return 0;
}