#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include "config.h"
#include "barrier.h"

#define NUM_PROCESSES  8

int main() {

    int vect[VECT_SIZE];
    int pid, shmid_largest, shmid_smallest;
    int *largest, *smallest;

    float per_process_raw = (float) VECT_SIZE / NUM_PROCESSES;
    int per_process = (int) per_process_raw;

    clock_t start, end;
    double time_taken;

    if(per_process_raw != (float) per_process) {
        printf("Vector size of %d is not divisible by %d processes.\n", VECT_SIZE, NUM_PROCESSES);
        exit(-1);
    }

    srand(24601);
    int i;

    for(i=0; i<VECT_SIZE; i++) {   //generate random numbers
        vect[i] = rand();
    } 

    shmid_largest = shmget(IPC_PRIVATE, sizeof(int) * NUM_PROCESSES, IPC_CREAT | 0600);
    shmid_smallest = shmget(IPC_PRIVATE, sizeof(int) * NUM_PROCESSES, IPC_CREAT | 0600);
    largest = shmat(shmid_largest, NULL, 0);
    smallest = shmat(shmid_smallest, NULL, 0);

    init_barrier(NUM_PROCESSES + 1);

    for(i=0; i<NUM_PROCESSES; i++) {  //generate child processes
        pid = fork();

        if(pid == 0)
            break;
    }

    int j;
    int big = -INT_MAX;
    int small = INT_MAX;

    start = clock();


    if(pid == 0) {
        int start = i * per_process;            //define search start
        int end = i * per_process + per_process;   //define search end

        for(j=start; j<end; j++){
            if(vect[j] > big)
                big = vect[j];

            if(vect[j] < small)
                small = vect[j];
        }
        largest[i] = big;
        smallest[i] = small;

        reach_barrier();

    }
    else 
    {
        start = clock();
        reach_barrier();
        for(j=0; j<NUM_PROCESSES; j++)
        {
            if(largest[j] > big)
                big = largest[j];

            if(smallest[j] < small)
                small = smallest[j];
        }
        end = clock();
        time_taken = ((double) end - start) / CLOCKS_PER_SEC;

        printf("\nNumber of items: %d\n", VECT_SIZE);
        printf("Smallest element is %d\n", small);
        printf("Largest element is %d\n", big);
        printf("Time taken is %3.20fs\n\n", time_taken);

        // Clean up process table
        for(j=0; j<NUM_PROCESSES; j++)
            wait(NULL);
            destroy_barrier(pid);
            shmdt(largest); shmdt(smallest);
            shmctl(shmid_largest, IPC_RMID, 0); shmctl(shmid_smallest, IPC_RMID, 0);
    }
}

