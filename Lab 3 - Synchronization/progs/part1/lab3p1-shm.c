#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define NUM_PROCESSES 5

int main() {

    int i, j, pid;
    int shmid;
    int* turn;

    shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600); //110 -> read and write bits set
    turn = shmat(shmid, NULL, 0); //get pointer to shared memory
    
    turn[0] = 0;

    for(i=0; i<NUM_PROCESSES; i++)
    {
        if((pid = fork()) == 0) {
            break;
        }
    }

    if(pid == 0) {

        while(turn[0] != i); //wait for turn

        printf("I am child %d\n", i);

        for(j = i*10; j<i*10 + 10; j++){
            printf("%d ", j);
            fflush(stdout);
            usleep(250000);
        }

        printf("\n\n");

        turn[0] += 1; //signal change in turn
        shmdt((void*) turn); //detach shared mem

    }
    else {
        for(i=0; i<NUM_PROCESSES; i++) 
            wait(NULL);
            //parent detach and destroys shared mem after children are all done.
            shmdt((void*) turn);
            shmctl(shmid, IPC_RMID, 0);
    }

}

