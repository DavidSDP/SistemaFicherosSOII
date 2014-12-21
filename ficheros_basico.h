#include <time.h>
#include "bloques.h"
#include <limits.h>
#include <string.h>

#define posSB 0
#define T_INODO 128

typedef struct Estr_superbloque{
	unsigned int primerMapa;
	unsigned int ultimoMapa;
	unsigned int primerInodos;
	unsigned int ultimoInodos;
	unsigned int primerDatos;
	unsigned int ultimoDatos;
	unsigned int inodoRaiz;
	unsigned int primerInodoLibre;
	unsigned int bloqueslibres;
	unsigned int inodoslibres;
	unsigned int totbloques;
	unsigned int totinodos;
	char padding[BLOCKSIZE-12*sizeof(unsigned int)];
} Superbloque;

typedef struct Estr_inodo{
	unsigned char tipo;
	unsigned char permisos;
	time_t atime;
	time_t mtime;
	time_t ctime;
	unsigned int nentradas;
	unsigned int bytesSize;
	unsigned int bloquesOcupados;
	unsigned int punterosDirectos[12]; // Bloques directos (12)
	unsigned int punterosIndirectos[3]; // Bloque indirecto simple, doble y triple
	char padding[T_INODO-18*sizeof(unsigned int)-3*sizeof(time_t)-2*4*sizeof(unsigned char)];
} Inodo;

int tamMB(unsigned int nbloques);
int tamAI(unsigned int ninodos);
int initSB(unsigned int nbloques, unsigned int ninodos);
int leerSuperbloque(Superbloque *sb);
unsigned int calcular_bloques();
int initMB(unsigned int nbloques);
int initAI(unsigned int ninodos);

int escribir_bit(unsigned int nbloque, unsigned int bit);
unsigned char leer_bit(unsigned int nbloque);
int reservar_bloque();
int liberar_bloque(unsigned int nbloque);
int escribir_inodo(Inodo inodo, unsigned int ninodo);
Inodo leer_inodo(unsigned int ninodo);
int reservar_inodo(unsigned char tipo, unsigned char permisos);
unsigned int encontrarNivel(Inodo inodo, unsigned int blogico, int *ptr);
int get_index(unsigned int blogico, unsigned int level);
int traducir_bloque_inodo(unsigned int ninodo, unsigned int blogico, unsigned char reservar);
int liberar_bloques_inodo(unsigned int ninodo, unsigned int blogico);
int vaciar_nivel(int level, int ptr, int primero, int blogico, int *bliberados);
int liberar_inodo(unsigned int ninodo);
