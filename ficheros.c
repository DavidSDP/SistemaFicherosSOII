#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){
	Inodo inodo = leer_inodo(ninodo);
	int bytesescritos = 0;
	if ((inodo.permisos & 2) != 2) {
		printf("Error: no tienes los permisos de escritura necesarios\n");
		return -1;
	}
	unsigned int primerlogico = offset/BLOCKSIZE;
	unsigned int ultimologico = (offset+nbytes-1)/BLOCKSIZE;
	if(primerlogico == ultimologico){
		// Caso de un unico bloque modificado ->
		unsigned char buf_bloque[BLOCKSIZE];
		memset(buf_bloque, 0, sizeof(buf_bloque));
		int desp1 = offset%BLOCKSIZE;
		int bfisico = traducir_bloque_inodo(ninodo, primerlogico, 1); // Obtenemos el bloque fisico y reservamos bloque si es necesario
		if(bread(bfisico, buf_bloque) < 0) return -1;
		memcpy(buf_bloque+desp1, buf_original, nbytes);
		int a;
		if((a = bwrite(bfisico, &buf_bloque)) < 0) return -1;
		bytesescritos += a;
	}else{
		// Primer bloque ->
		int desp = 0;
		unsigned char buf_bloque[BLOCKSIZE];
		memset(buf_bloque, 0, sizeof(buf_bloque));
		int desp1 = offset%BLOCKSIZE;
		int bfisico = traducir_bloque_inodo(ninodo, primerlogico, 1); // Obtenemos el bloque fisico y reservamos bloque si es necesario
		if(bfisico < 0) return bytesescritos;
		if(bread(bfisico, buf_bloque) < 0) return -1;
		memcpy(buf_bloque+desp1, buf_original, BLOCKSIZE-desp1);
		int a;
		if((a = bwrite(bfisico, &buf_bloque)) < 0) return -1;
		desp += desp1;
		bytesescritos += a;
		// Bloques intermedios ->
		int i;
		for(i = primerlogico + 1; i < ultimologico; i++){
			bfisico = traducir_bloque_inodo(ninodo, i, 1);
		if(bfisico < 0) return bytesescritos;
			memcpy(buf_bloque, buf_original + (BLOCKSIZE - desp1) + (i - primerlogico - 1) * BLOCKSIZE, BLOCKSIZE);
			int a;
			if((a = bwrite(bfisico, &buf_bloque)) < 0) return -1;
			bytesescritos += a;
		}
		// Último bloque ->
		int desp2 = (offset+nbytes-1)%BLOCKSIZE;
		bfisico = traducir_bloque_inodo(ninodo, ultimologico, 1);
		if(bfisico < 0) return bytesescritos;
		if(bread(bfisico, &buf_bloque) < 0) return -1;
		memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
		if((a = bwrite(bfisico, &buf_bloque)) < 0) return -1;
		bytesescritos += a;
	}
	// Actualizamos el inodo ->
	inodo = leer_inodo(ninodo);
	if(offset+nbytes > inodo.bytesSize){
		inodo.bytesSize = offset + nbytes;
	}
	inodo.mtime = time(NULL);
	inodo.ctime = time(NULL);
	if(escribir_inodo(inodo, ninodo) < 0) return -1;
	return bytesescritos;
}


int mi_read_f(unsigned int ninodo, void *buf, unsigned int offset, unsigned int nbytes){
	
	Inodo inodo = leer_inodo(ninodo);
	if(!(inodo.permisos & 4)){
		printf("Error: no tiene los permisos necesarios\n");
		return -1;
	}
	if(offset + nbytes > inodo.bytesSize){ // Caso en el que los bytes a leer superan el tamaño de fichero
		nbytes = inodo.bytesSize - offset;
	}
	if(offset >= inodo.bytesSize){ // Caso en el que se quiere empezar a leer en una posicion posterior al fin de fichero
		nbytes = 0;
		return nbytes;
	}
	int bfisico = 0;
	unsigned int primerlogico = offset/BLOCKSIZE;
	unsigned int ultimologico = (offset+nbytes-1)/BLOCKSIZE;
	unsigned char buffer[BLOCKSIZE];
	unsigned char baux[BLOCKSIZE];
	memset(baux, 0, sizeof(baux));
	memset(buffer, 0, sizeof(buffer));
	int bytesleidos = 0;
	if(primerlogico == ultimologico){ // Caso mismo bloque a leer
		bfisico = traducir_bloque_inodo(ninodo, primerlogico, 0);
		if(bread(bfisico, &buffer) < 0) return -1;
		int desp1 = offset % BLOCKSIZE;
		memcpy(buf, buffer+desp1, nbytes);
		bytesleidos += nbytes;

	}else{ // Caso varios bloques
		// Primer bloque ->
		bfisico = traducir_bloque_inodo(ninodo, primerlogico, 0);
		if(bread(bfisico, &buffer) < 0) return -1;
		int desp1 = offset % BLOCKSIZE;
		memcpy(buf, buffer + desp1, BLOCKSIZE - desp1);
		bytesleidos += BLOCKSIZE - desp1;
		memset(buffer, 0, sizeof(buffer));
		// Bloques intermedios ->
		int i;
		for(i = primerlogico + 1; i < ultimologico; i++){
			bfisico = traducir_bloque_inodo(ninodo, i, 0);
			if(bread(bfisico, &buffer) < 0) return -1;
			memcpy(buf + bytesleidos, buffer, BLOCKSIZE);
			bytesleidos += BLOCKSIZE;
			memset(buffer, 0, sizeof(buffer));
		}
		// Ultimo bloque ->
		int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
		bfisico = traducir_bloque_inodo(ninodo, ultimologico, 0);
		if(bread(bfisico, &buffer) < 0) return -1;
		memcpy(buf + bytesleidos, buffer, desp2 + 1);
		bytesleidos += desp2 + 1;
	}
	

	inodo = leer_inodo(ninodo);
	inodo.atime = time(NULL);
	if(escribir_inodo(inodo, ninodo) < 0) return -1;
	return bytesleidos;
}


int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
	Inodo inodo = leer_inodo(ninodo);
	inodo.permisos = permisos;
	inodo.ctime = time(NULL);
	int a = escribir_inodo(inodo, ninodo);
	return a;
}

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
	Inodo i = leer_inodo(ninodo);
	if(!(i.permisos & 2)){
		printf("Error: no tienes los permisos necesarios\n");
		return -1;
	}
	int primer_bloque = nbytes/BLOCKSIZE;
	int bloqueinicial = primer_bloque + 1;
	int offset = nbytes%BLOCKSIZE;
	char buf[BLOCKSIZE];
	memset(buf, 0, sizeof(buf));
	mi_read_f(ninodo, buf, primer_bloque*BLOCKSIZE, BLOCKSIZE);
	char empty[BLOCKSIZE-offset];
	memset(empty, 0, sizeof(empty));
	memcpy(buf+offset, empty, sizeof(empty));
	mi_write_f(ninodo, buf, primer_bloque*BLOCKSIZE, BLOCKSIZE);
	liberar_bloques_inodo(ninodo, bloqueinicial);
	Inodo inodo = leer_inodo(ninodo);
	inodo.mtime = time(NULL);
	inodo.ctime = time(NULL);
	inodo.bytesSize = nbytes;
	int a = escribir_inodo(inodo, ninodo);
	return a;
}

int mi_stat_f(unsigned int ninodo, STAT *p_stat){
	Inodo inodo = leer_inodo(ninodo);
	p_stat->tipo = inodo.tipo;
	p_stat->permisos = inodo.permisos;
	p_stat->atime = inodo.atime;
	p_stat->mtime = inodo.mtime;
	p_stat->ctime = inodo.ctime;
	p_stat->nentradas = inodo.nentradas;
	p_stat->bytesSize = inodo.bytesSize;
	p_stat->bloquesOcupados = inodo.bloquesOcupados;
	return 0;
}





