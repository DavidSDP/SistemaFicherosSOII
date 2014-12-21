#include "bloques.h"
#include "ficheros_basico.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_read_f(unsigned int ninodo, void *buf, unsigned int offset, unsigned int nbytes);

typedef struct estado_inodo{
	unsigned char tipo;
	unsigned char permisos;
	time_t atime;
	time_t mtime;
	time_t ctime;
	unsigned int nentradas;
	unsigned int bytesSize;
	unsigned int bloquesOcupados;
} STAT;

int mi_chmod_f(unsigned int ninodo, unsigned char permisos);
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);
int mi_stat_f(unsigned int ninodo, STAT *p_stat);
