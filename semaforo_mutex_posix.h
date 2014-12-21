#include <semaphore.h> // sem_open, sem_unlink, sem_post, sem_wait
#include <stdlib.h>

#define SEM_NAME "/mymutex" /* Usamos este nombre para el semáforo mutex */ 
#define SEM_INIT_VALUE 1 /* Valor inicial de los mutex */

sem_t *initSem();
void deleteSem();
void signalSem(sem_t *sem);
void waitSem(sem_t *sem);
