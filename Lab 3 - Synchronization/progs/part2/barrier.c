#include <stdio.h>
#include <semaphore.h>
#include <sys/shm.h>

int nproc = 0;
int shmid_count, shmid_barrier, shmid_mutex;
int *count;
sem_t *barrier;
sem_t *mutex;

void init_barrier(int numproc) {
    int shmid;
    shmid_count = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    count = (int *) shmat(shmid_count, NULL, 0);
    *count = 0;
    nproc = numproc;
    shmid_barrier = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);
    barrier = (sem_t *) shmat(shmid_barrier, NULL, 0);
    sem_init(barrier, 1, 0);

    shmid_mutex = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);
    mutex = (sem_t *) shmat(shmid_mutex, NULL, 0);
    sem_init(mutex, 1, 1);
}

void reach_barrier() {
    sem_wait(mutex);
    (*count)++;
    sem_post(mutex);

    if(*count == nproc) {
        sem_post(barrier);
    } else {
        sem_wait(barrier);
        sem_post(barrier);
    }

}

void destroy_barrier(int my_pid) {
    if(my_pid != 0) {
        // Destroy the semaphores and detach
        // and free any shared memory. Notice
        // that we explicity check that it is
        // the parent doing it.
        sem_destroy(barrier);
        sem_destroy(mutex);
        shmdt(barrier);
        shmdt(mutex);
        shmdt(count);
        shmctl(shmid_count, IPC_RMID, 0);
        shmctl(shmid_mutex, IPC_RMID, 0);
        shmctl(shmid_barrier, IPC_RMID, 0);
        
    }
}


