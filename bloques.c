#include "bloques.h"

static int fd;
static sem_t *mutex;

int bmount(const char *path){
	fd = open(path, O_RDWR|O_CREAT, 0666);
	mutex = initSem();
	if(fd < 0){
		printf("Error al crear/abrir fichero\n");
		return -1;
	}
	return fd;
}

int bumount(){
	int c;
	deleteSem();
	if(c = close(fd) < 0){
		printf("Error al cerrar el fichero\n");
		return -1;
	}
	return c;
}

int bwrite(unsigned int bloque, const void *buf){
	if(lseek(fd, bloque*BLOCKSIZE, SEEK_SET) < 0){
		printf("Error de puntero de escritura\n");
		return -1;
	}
	int w;
	if(w = write(fd, buf, BLOCKSIZE) < 0){
		printf("Error al escribir en fichero\n");
		return -1;
	}
	return w;
}

int bread(unsigned int bloque, void *buf){
	if(lseek(fd, bloque*BLOCKSIZE, SEEK_SET) < 0){
		printf("Error de puntero de lectura\n");
		return -1;
	}
	int r;
	if(r = read(fd, buf, BLOCKSIZE) < 0){
		printf("Error de lectura de bloque\n");
		return -1;
	}
	return r;
}

void mi_waitSem(){
	
	waitSem(mutex);
	
}

void mi_signalSem(){
	
	signalSem(mutex);
	
}











