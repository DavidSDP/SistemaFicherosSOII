#include "ficheros_basico.h"

int tamMB(unsigned int nbloques){
	if(((nbloques/8)%BLOCKSIZE)>0){
		return ((nbloques/8)/BLOCKSIZE + 1);
	}
	return ((nbloques/8)/BLOCKSIZE);
}

int tamAI(unsigned int ninodos){
	if(((ninodos*T_INODO)%BLOCKSIZE)>0){
		return (((ninodos*T_INODO)/BLOCKSIZE)+1);
	}
	return (((ninodos*T_INODO)/BLOCKSIZE));
}

int initSB(unsigned int nbloques, unsigned int ninodos){

	Superbloque SB;
	SB.primerMapa = posSB + (sizeof(Superbloque)/BLOCKSIZE);
	SB.ultimoMapa = SB.primerMapa + tamMB(nbloques) - 1;
	SB.primerInodos = SB.ultimoMapa + 1;
	SB.ultimoInodos = SB.primerInodos + tamAI(ninodos) - 1;
	SB.primerDatos = SB.ultimoInodos + 1;
	SB.ultimoDatos = nbloques - 1;
	SB.inodoRaiz = 0;
	SB.primerInodoLibre = 0;
	SB.bloqueslibres = nbloques-1-tamMB(nbloques)-tamAI(nbloques); //Actualizados los bloques libres según etapa 3
	SB.inodoslibres = ninodos;
	SB.totbloques = nbloques;
	SB.totinodos = ninodos;
	return bwrite(posSB, &SB);
}

int leerSuperbloque(Superbloque *sb){
	return bread(posSB, sb);
}

unsigned int calcular_bloques(){
	Superbloque sb;
	leerSuperbloque(&sb);
	return sb.totbloques;
}

int initMB(unsigned int nbloques){

	Superbloque sb;
	leerSuperbloque(&sb);
	unsigned char initacero[BLOCKSIZE];
	memset(initacero, 0, sizeof(initacero));
	int i;
	for(i = 0; i < tamMB(nbloques); i++){
		bwrite(sb.primerMapa + i, initacero);
	}
	// Iniciamos los bits del superbloque, mapa de bits y array de inodos
	int tamSB = 1;
	for(i = 0; i < tamSB; i++){
		if(escribir_bit(i, 1) < 0){ //Superbloque
			printf("Error al escribir bit\n");
			return -1;
		}
	}
	for(i = 0; i < tamMB(nbloques); i++){
		if(escribir_bit(i+tamSB, 1) < 0){ //Mapa de bits
			printf("Error al escribir bit\n");
			return -1;
		}
	}
	for(i = 0; i < tamAI(nbloques); i++){
		if(escribir_bit(i+tamSB+tamMB(nbloques), 1) < 0){; //Array de inodos
			printf("Error al escribir bit\n");
			return -1;
		}
	}
	return 0;
}

int initAI(unsigned int ninodos){

	Superbloque sb;
	leerSuperbloque(&sb);
	int x = 1;
	Inodo inodos[BLOCKSIZE/T_INODO];
	int i = sb.primerInodos;
	while(i <= sb.ultimoInodos){
		int j = 0;
		int over = 0;
		while( (j < (BLOCKSIZE/T_INODO)) &&(!over) ){
			inodos[j].tipo = 'l';
			if(x < ninodos){
				inodos[j].punterosDirectos[0] = x;
				x++;
			}else{
				inodos[j].punterosDirectos[0] = UINT_MAX;
				over = 1;
			}
			j++;
		}
		bwrite(i, inodos);
		i++;
	}
	return 0;
}

int escribir_bit(unsigned int nbloque, unsigned int bit){ // Escribe un bit a partir de la posicion 0 del mapa de bits. Se le pasa por parametro el bloque del sistema que se quiere marcar o desmarcar.
	Superbloque sb;
	leerSuperbloque(&sb);
	unsigned char mascara = 128;
	unsigned char bufferMB[BLOCKSIZE];
	memset(bufferMB, 0, sizeof(bufferMB));
	
	int posbyte = nbloque / 8;
	int posbit = nbloque % 8;
	int bloque_MB = nbloque/(BLOCKSIZE*8);
	bread(sb.primerMapa + bloque_MB, bufferMB); // Leemos el mapa de bits
	if(bit == 0){
		mascara = mascara >> posbit;
		bufferMB[posbyte%BLOCKSIZE] &= ~mascara;
	}else if(bit == 1){
		mascara = mascara >> posbit;
		bufferMB[posbyte%BLOCKSIZE] |= mascara;
	}else{
		printf("El bit no es ni 1 ni 0\n");
		return -1;
	}

	return bwrite(sb.primerMapa + bloque_MB, bufferMB);
}

unsigned char leer_bit(unsigned int nbloque){
	Superbloque sb;
	leerSuperbloque(&sb);
	unsigned char mascara = 128;
	unsigned char bufferMB[BLOCKSIZE];
	memset(bufferMB, 0, sizeof(bufferMB));
	int posbyte = (nbloque/8)%BLOCKSIZE;
	int posbit = nbloque % 8;
	unsigned int bl = nbloque/(BLOCKSIZE*8);
	mascara = mascara >> posbit;
	if(bread(sb.primerMapa+bl, bufferMB) < 0) return -1;
	mascara = mascara & bufferMB[posbyte]; // Realizamos operacion AND binario de la mascara con el byte correspondiente
	mascara = mascara >> (7-posbit); // Desplazamos el bit hacia la derecha
	return mascara;

}

int reservar_bloque(){
	Superbloque sb;
	leerSuperbloque(&sb);
	
	if(sb.bloqueslibres == 0){
		printf("No hay bloques libres\n");
		return -1;
	}
	unsigned char buffer[BLOCKSIZE];
	unsigned char bufaux[BLOCKSIZE];
	memset(buffer, 0, sizeof(buffer));
	memset(bufaux, 255, sizeof(bufaux));
	unsigned char mask = 128;
	unsigned int posbit = 0;
	int j;
	int i = sb.primerMapa;
	int found = 0;
	int pos = 0;
	int posbyte = 0;
	while((!found)&&(i <= sb.ultimoMapa)){
		if(bread(i, &buffer) < 0) return -1;
		if(memcmp(buffer, bufaux, sizeof(buffer)) != 0){
			int found2 = 0;
			j = 0;
			while((!found2)&&(j < sizeof(buffer))){
				if(buffer[j] < 255){
					while(buffer[j] & mask){
						buffer[j] <<= 1;
						posbit++;					
					}
						posbyte = j;
						pos = i;	
					found2 = 1;
				}
				j++;
			}
			found = 1;
		}
		i++;
	}
	unsigned int nbloque = ((pos-sb.primerMapa)*BLOCKSIZE + posbyte)*8 + posbit;
	// Actualizamos mapa de bits
	escribir_bit(nbloque, 1);
	
	sb.bloqueslibres = sb.bloqueslibres - 1;
	bwrite(posSB, &sb);
	return nbloque;
}

int liberar_bloque(unsigned int nbloque){
	Superbloque sb;
	leerSuperbloque(&sb);
	
	// Actualizamos mapa de bits
	if(escribir_bit(nbloque, 0) < 0){
		printf("Error al escribir bit\n");
		return -1;
	};

	sb.bloqueslibres = sb.bloqueslibres + 1;
	
	bwrite(posSB, &sb);
	
	return nbloque;
}

int escribir_inodo(Inodo inodo, unsigned int ninodo){
	Superbloque sb;
	leerSuperbloque(&sb);
	int nbloque = sb.primerInodos + ninodo/(BLOCKSIZE/T_INODO);
	Inodo arrayInodos[BLOCKSIZE/T_INODO];
	bread(nbloque, arrayInodos);
	arrayInodos[ninodo % (BLOCKSIZE/T_INODO)] = inodo;
	int a = bwrite(nbloque, arrayInodos);
	return a;
}

Inodo leer_inodo(unsigned int ninodo){
	Superbloque sb;
	leerSuperbloque(&sb);
	int nbloque = sb.primerInodos + ninodo/(BLOCKSIZE/T_INODO);
	Inodo arrayInodos[BLOCKSIZE/T_INODO];
	bread(nbloque, &arrayInodos);
	Inodo inodo = arrayInodos[ninodo % (BLOCKSIZE/T_INODO)];
	return inodo;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){
	Superbloque sb;
	leerSuperbloque(&sb);
	int ninodo = sb.primerInodoLibre;
	Inodo inodo = leer_inodo(ninodo);
	unsigned int siguientelibre = inodo.punterosDirectos[0];
	inodo.tipo = tipo;
	inodo.permisos = permisos;
	inodo.nentradas = 1;
	inodo.bytesSize = 0;
	inodo.atime = time(NULL);
	inodo.mtime = time(NULL);
	inodo.ctime = time(NULL);
	inodo.bloquesOcupados = 0;
	memset(inodo.punterosDirectos, 0, sizeof(inodo.punterosDirectos));
	memset(inodo.punterosIndirectos, 0, sizeof(inodo.punterosIndirectos));
	escribir_inodo(inodo, sb.primerInodoLibre); // Escribimos el inodo reservado
	

	// Actualizamos la lista enlazada de inodos libres

	sb.inodoslibres--; // Reducimos la cantidad de inodos libres, controlada por el superbloque

	sb.primerInodoLibre = siguientelibre; // El primer inodo libre será siempre al que apunte el superbloque.primerInodoLibre, por lo que para actualizar la lista sólo habrá que corregir la posición del primero. (Para el liberar_inodo deberemos actualizar sb.primerInodoLibre=idnuevo_inodo y nuevo_inodo.punterosDirectos[0]=sb.primerInodoLibre (necesario swap))
	bwrite(posSB, &sb); // Escribimos el superbloque actualizado
	return ninodo;
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int blogico, unsigned char reservar){
	
	Inodo inodo = leer_inodo(ninodo);
	int ptr = 0;
	int antptr = 0;
	unsigned int buffer[BLOCKSIZE/sizeof(unsigned int)]; // 256 entradas de unsigned int por bloque
	int level = encontrarNivel(inodo, blogico, &ptr); // Encuentra el nivel y actualiza el ptr
	int levelfinal = level;
	int i;
	while(level > 0){
		if(ptr == 0){
			if(!reservar){
				return -1;
			}else{
				ptr = reservar_bloque();
				inodo.bloquesOcupados++;
				inodo.ctime = time(NULL);
				if(level == levelfinal){
					inodo.punterosIndirectos[level-1] = ptr;
				}else{
					buffer[i] = ptr;
					if(bwrite(antptr, &buffer) < 0) return -1;
				}
			}
		}
		memset(buffer, 0, (BLOCKSIZE/sizeof(unsigned int))); // Ponemos el buffer a 0s
		if(bread(ptr, &buffer) < 0) return -1;
		i = get_index(blogico, level);
		antptr = ptr;
		ptr = buffer[i];
		level--;
	}
	if(ptr == 0){
			if(!reservar){
				return -1;
			}else{
					ptr = reservar_bloque();
					inodo.bloquesOcupados++;
					inodo.ctime = time(NULL);
				if(levelfinal == 0){
					inodo.punterosDirectos[blogico] = ptr;
				}else{
					buffer[i] = ptr;
					if(bwrite(antptr, &buffer) < 0) return -1;
				}
			}
	}
	escribir_inodo(inodo, ninodo);
	return ptr;
}

unsigned int encontrarNivel(Inodo inodo, unsigned int blogico, int *ptr){
	int npunteros = BLOCKSIZE/sizeof(unsigned int); // 256
	int directos = 12;
	int indirectos0 = npunteros + directos;
	int indirectos1 = npunteros*npunteros + indirectos0;
	int indirectos2 = npunteros*npunteros*npunteros + indirectos1;
	if(blogico < directos){
		*ptr = inodo.punterosDirectos[blogico];
		return 0;
	}else if(blogico < indirectos0){
		*ptr = inodo.punterosIndirectos[0];
		return 1;
	}else if(blogico < indirectos1){
		*ptr = inodo.punterosIndirectos[1];
		return 2;
	}else if(blogico < indirectos2){
		*ptr = inodo.punterosIndirectos[2];
		return 3;
	}else{
		printf("No existe el bloque\n");
		*ptr = 0;
		return -1;
	}
}


int get_index(unsigned int blogico, unsigned int level){
	int npunteros = BLOCKSIZE/sizeof(unsigned int); // 256
	int directos = 12;
	int indirectos0 = npunteros + directos;
	int indirectos1 = npunteros*npunteros + indirectos0;
	int indirectos2 = npunteros*npunteros*npunteros + indirectos1;
	if(blogico < directos){
		return blogico;
	}else if(blogico < indirectos0){
		return blogico-directos;
	}else if(blogico < indirectos1){
		if(level == 2){
			return ((blogico-indirectos0)/(npunteros));
		}else if(level == 1){
			return ((blogico-indirectos0)%(npunteros));
		}
	}else if(blogico < indirectos2){
		if(level == 3){
			return ((blogico-indirectos1)/(npunteros*npunteros));
		}else if(level == 2){
			return (((blogico-indirectos1)%(npunteros*npunteros))/npunteros);
		}else if(level == 1){
			return (((blogico-indirectos1)%(npunteros*npunteros))%npunteros);
		}
	}
}

int vaciar_nivel(int level, int ptr, int primero, int blogico, int *bliberados){
	unsigned int buffer[BLOCKSIZE/sizeof(unsigned int)];
	unsigned int baux[BLOCKSIZE/sizeof(unsigned int)];
	unsigned int ceros[BLOCKSIZE/sizeof(unsigned int)];
	memset(buffer, 0, sizeof(buffer));
	memset(baux, 0, sizeof(baux));
	memset(ceros, 0, sizeof(ceros));
	if(ptr != 0){
		if(level == 1){
			bread(ptr, buffer);
			int x;
			if(primero == 1){
				x = get_index(blogico, level);
			}else{
				x = 0;
			}
			while(x < 256){
				if(buffer[x] != 0){
					bwrite(buffer[x], ceros);
					liberar_bloque(buffer[x]);
					buffer[x] = 0;
					(*bliberados)++;
				}
				x++;
			}
			bwrite(ptr, buffer);
		}else{
			bread(ptr, buffer);
			int x;
			if(primero == 1){
				x = get_index(blogico, level);
			}else{
				x = 0;
			}
			level--;
			while(x < 256){
				if(buffer[x] != 0){
					vaciar_nivel(level, buffer[x], primero, blogico, bliberados);
					primero = 0;
					bread(buffer[x], baux);
					if(memcmp(baux, ceros, sizeof(baux)) == 0){
						liberar_bloque(buffer[x]);
						buffer[x] = 0;
						(*bliberados)++;
					}
				}
				x++;
			}
			bwrite(ptr, buffer);
		}
	}
}

int liberar_inodo(unsigned int ninodo){
	
	liberar_bloques_inodo(ninodo, 0);
	
	Superbloque sb;
	leerSuperbloque(&sb);
	Inodo inodo = leer_inodo(ninodo);
	inodo.tipo = 'l';
	inodo.punterosDirectos[0] = sb.primerInodoLibre;
	sb.primerInodoLibre = ninodo;
	sb.inodoslibres++;
	escribir_inodo(inodo, ninodo);
	bwrite(posSB, &sb);
	return ninodo;
	
}

int liberar_bloques_inodo(unsigned int ninodo, unsigned int blogico){
	
	Inodo inodo = leer_inodo(ninodo);
	int ptr;
	int esprimero = 0;
	int primero = 1;
	int resto = 0;
	int bloquesliberados = 0;
	int levelfinal = encontrarNivel(inodo, blogico, &ptr);
	int level = levelfinal;
	unsigned int bufferaux[BLOCKSIZE/sizeof(unsigned int)];
	unsigned int ceros[BLOCKSIZE/sizeof(unsigned int)];
	memset(ceros, 0, sizeof(ceros));
	memset(bufferaux, 0, sizeof(bufferaux));
	int i, j;
	if(levelfinal == 0){
		for(i = blogico; i < 12; i++){
			if(inodo.punterosDirectos[i] != 0){
					liberar_bloque(inodo.punterosDirectos[i]);
					inodo.punterosDirectos[i] = 0;
					bloquesliberados++;
			}
		}
		level++;
	}
	for(j = level; j < 4; j++){
		if(inodo.punterosIndirectos[j-1] != 0){
			if(j == levelfinal){
				esprimero = primero;
			}else{
				esprimero = resto;
			}
			vaciar_nivel(j, inodo.punterosIndirectos[j-1], primero, blogico, &bloquesliberados);
			bread(inodo.punterosIndirectos[j-1], bufferaux);
			if(memcmp(bufferaux, ceros, sizeof(bufferaux)) == 0){
				liberar_bloque(inodo.punterosIndirectos[j-1]);
				inodo.punterosIndirectos[j-1] = 0;
				bloquesliberados++;
			}
		}
	}
	
	inodo.bloquesOcupados = inodo.bloquesOcupados - bloquesliberados;
	inodo.ctime = time(NULL);
	escribir_inodo(inodo, ninodo);
	return 0;
}

