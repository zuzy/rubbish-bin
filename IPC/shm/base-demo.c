#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

#define ARRAY_SIZE      40000
#define MALLOC_SIZE     100000
#define SHM_SIZE        100000
#define SHM_MODE        0600

char array[ARRAY_SIZE];
int main(int argc, char *argv[])
{
    int shmid;
    char *ptr, *shmptr;
    printf("array[] from %p to %p\n", array, &array[ARRAY_SIZE]);
    printf("stack around %lx\n", (unsigned long) shmid);

    if((ptr = malloc(MALLOC_SIZE)) == NULL) {
        fprintf(stderr, "malloc error!\n");
    }
    printf("malloced from %p to %p\n", ptr, ptr+MALLOC_SIZE);

    if((shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0) {
        fprintf(stderr, "shmget error!\n");
    }
    if((shmptr = shmat(shmid, NULL, 0)) == (void *) -1) {
        fprintf(stderr, "shmat error\n");
    }
    printf("shared memory from %p to %p\n", shmptr, shmptr+SHM_SIZE);

    if(shmctl(shmid, IPC_RMID, 0) < 0) {
        fprintf(stderr, "shmctl error\n");
    }
    return 0;
}
