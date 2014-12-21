/*fichero bloques.h */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> /* Modos de apertura y función open()*/
#include <stdio.h>
#include <stdlib.h>
#include "semaforo_mutex_posix.h"



#define BLOCKSIZE 1024 //bytes

int bmount(const char *path);
int bumount();
int bwrite(unsigned int bloque, const void *buf);
int bread(unsigned int bloque, void *buf);
void mi_waitSem();
void mi_signalSem();
