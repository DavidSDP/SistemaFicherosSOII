#include "ficheros.h"
#define numeroentradas 500;

int empty(int lectura);

int isempty(int lectura);

int isfull(int lectura);

int get(int lectura, const char *camino, int *p_inodo);

int removeLast(int lectura);

int add(int lectura, const char *camino, int p_inodo);

int extraer_camino(const char *camino, char *inicial, char *final);

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos);

typedef struct entrada_fichero {
	char nombre[60];
	unsigned int inodo;
} Entrada; // 64 bytes

int mi_creat(const char *camino, unsigned char permisos);

int mi_dir(const char *camino, char *buffer);

int mi_unlink(const char *camino);

int mi_chmod(const char *camino, unsigned char permisos);

int mi_stat(const char *camino, STAT *p_stat);

typedef struct ultima_entrada {
	char camino[512];
	unsigned int p_inodo;
	struct ultima_entrada *next;
} UltimaEntrada;

typedef struct cola {
	UltimaEntrada *first;
	UltimaEntrada *last;
	int size;
	int maxsize;
} Cache;

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes);

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes);

Cache cacheLectura;

Cache cacheEscritura;

