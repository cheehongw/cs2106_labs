#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

#define NUM_PROCESSES 5

int main() {

    int i, j, pid, shmid;
    sem_t *sem;

    shmid = shmget(IPC_PRIVATE, sizeof(sem_t) * NUM_PROCESSES, IPC_CREAT | 0600);
    sem = (sem_t *) shmat(shmid, NULL, 0);
    
    for(i=0; i<NUM_PROCESSES; i++)
    {   
        int initial = i == 0 ? 1 : 0;
        sem_init(&sem[i], 1, initial); //initialize semaphore for each array pos
        if((pid = fork()) == 0) {
            break;
        }
    }

    if(pid == 0) {
        sem_wait(&sem[i]);
        printf("I am child %d\n", i);

        for(j = i*10; j<i*10 + 10; j++){
            printf("%d ", j);
            fflush(stdout);
            usleep(250000);
        }

        printf("\n\n");

        if (i != NUM_PROCESSES - 1) sem_post(&sem[i + 1]);
        sem_destroy(&sem[i]);
    }
    else {
        for(i=0; i<NUM_PROCESSES; i++) 
            wait(NULL);
        shmdt(sem);
        shmctl(shmid, IPC_RMID, 0);
    }

}

