#include "directorios.h"
	
int empty(int lectura){
	UltimaEntrada *aux;
	aux = (UltimaEntrada *)malloc(sizeof(UltimaEntrada));
	memset(aux->camino, 0, sizeof(aux->camino));
	if(!lectura){
		cacheEscritura.first = aux;
		cacheEscritura.last = aux;
		cacheEscritura.size = 0;
		cacheEscritura.maxsize = numeroentradas;
	}else{
		cacheLectura.first = aux;
		cacheLectura.last = aux;
		cacheLectura.size = 0;
		cacheLectura.maxsize = numeroentradas;
	}
	return 0;
	
}

int isempty(int lectura){
	Cache *cache;
	if(!lectura){
		cache = &cacheEscritura;
	}else{
		cache = &cacheLectura;
	}
	if((cache->size) == 0) return 1;
	return 0;
}

int isfull(int lectura){
	Cache *cache;
	if(!lectura){
		cache = &cacheEscritura;
	}else{
		cache = &cacheLectura;
	}
	if((cache->size) == (cache->maxsize)) return 1;
	return 0;
}

int get(int lectura, const char *camino, int *p_inodo){
	Cache *cache;
	if(!lectura){
		cache = &cacheEscritura;
	}else{
		cache = &cacheLectura;
	}
	int not_found = -1;
	char empty[60];
	memset(empty, 0, sizeof(empty));
	UltimaEntrada *entrada;
	entrada = cache->first;
	int i = 1;
	while((i <= cache->size)&&(strcmp(entrada->camino, empty) != 0)){
		if(strcmp(entrada->camino, camino) == 0){
			*p_inodo = entrada->p_inodo;
			return 0;
		}
		entrada = entrada->next;
		i++;
	}
	*p_inodo = -1;
	return not_found;
}

int removeLast(int lectura){
	Cache *cache;
	if(!lectura){
		cache = &cacheEscritura;
	}else{
		cache = &cacheLectura;
	}
	UltimaEntrada *entrada = cache->first;
	UltimaEntrada *previous;
	if(cache->size != 1){
		while(entrada != cache->last){
			previous = entrada;
			entrada = entrada->next;
		}
		previous->next = NULL;
		cache->last = previous;
		cache->size = cache->size - 1;
	}else{
		cache->first = NULL;
		cache->last = NULL;
		cache->size = 0;
	}
	return 0;
}

int add(int lectura, const char *camino, int p_inodo){
	Cache *cache;
	if(!lectura){
		cache = &cacheEscritura;
	}else{
		cache = &cacheLectura;
	}
	UltimaEntrada *entrada;
	entrada = (UltimaEntrada *)malloc(sizeof(UltimaEntrada));
	strcpy(entrada->camino, camino);
	entrada->p_inodo = p_inodo;
	if(isfull(lectura)){
		removeLast(lectura);
	}
	entrada->next = cache->first;
	cache->first = entrada;
	
	if(isempty(lectura)){
		cache->last = entrada;
		cache->size = 1;
		return 0;
	}
	cache->size = cache->size + 1;
	return 0;
}

int extraer_camino(const char *camino, char *inicial, char *final){
	int fichero = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	if(camino[i] == '/'){
		i++;
		while((camino[i] != '/')&&(camino[i] != 0)){
			inicial[j] = camino[i];
			i++;
			j++;
		}
		if(camino[i] == '/'){
			fichero = 0;
		}else{
			fichero = 1;
		}
		while(camino[i] != 0){
			final[k] = camino[i];
			i++;
			k++;
		}
	}
	return fichero; // 0 si inicial es directorio, 1 si es fichero
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){
	int error_al_extraer_camino = -1;
	int sin_permisos_lectura = -2;
	int no_existe_entrada = -3;
	int no_existe_dir_intermedio = -4;
	int sin_permisos_escritura = -5;
	int entrada_ya_existe = -6;
	Inodo inodo;
	Entrada entrada;
	char inicial[60];
	memset(inicial, 0, sizeof(inicial));
	char final[60];
	memset(final, 0, sizeof(final));
	char empty[60];
	memset(empty, 0, sizeof(empty));
	if(strcmp(camino_parcial, "/") == 0){ // Es directorio raiz
		*p_inodo = 0;
		*p_entrada = 0;
		return 0;
	}
	int tipo = extraer_camino(camino_parcial, inicial, final);
	if(tipo < 0) return error_al_extraer_camino;
	inodo = leer_inodo(*p_inodo_dir);
	memset(entrada.nombre, '\0', sizeof(entrada.nombre));
	
	// MEJORA BUFFER
	int nbloque = 0;
	int numbloques;
	if(inodo.bytesSize%BLOCKSIZE == 0){
		numbloques = inodo.bytesSize/BLOCKSIZE;
	}else{
		numbloques = inodo.bytesSize/(BLOCKSIZE) + 1;
	}
	int numentradas = inodo.bytesSize/(sizeof(Entrada));
	int nentrada = 0;
	int existe = 0;
	
	if(numentradas > 0){
		Entrada arrayentradas[BLOCKSIZE/(sizeof(Entrada))]; //16 entradas
		memset(arrayentradas, 0, sizeof(arrayentradas));
		if(mi_read_f(*p_inodo_dir, &arrayentradas, nbloque*BLOCKSIZE, BLOCKSIZE) < 0) return sin_permisos_lectura;
		int salir = 0;
		while((nbloque < numbloques)&&(salir == 0)){
			int a = 0;
			while((a < (BLOCKSIZE/(sizeof(Entrada))))&&(salir == 0)){
				Inodo i = leer_inodo(arrayentradas[a].inodo);
				int tipoinodo;
				if(i.tipo == 'd'){
					tipoinodo = 0;
				}else{
					tipoinodo = 1;
				}
				if((strcmp(inicial, arrayentradas[a].nombre) == 0)&&(tipoinodo == tipo)){
					entrada = arrayentradas[a];
					salir = 1;
					existe = 1;
				}else if(strcmp(empty, arrayentradas[a].nombre) == 0){
					salir = 1;
				}else{
					a++;
					nentrada++;
				}
			}
			if(salir != 1){
				nbloque++;
				memset(arrayentradas, 0, sizeof(arrayentradas));
				mi_read_f(*p_inodo_dir, &arrayentradas, nbloque*BLOCKSIZE, BLOCKSIZE);
			}
		}
	}
	
	//END OF MEJORA BUFFER
	if(existe == 0){
		// MODO LECTURA
		if(reservar == '0') return no_existe_entrada;
		// MODO ESCRITURA
		strcpy(entrada.nombre, inicial);
		if(tipo == 0){ // tipo = 0 si es directorio
			if(strcmp(final, "/") == 0){
				entrada.inodo = reservar_inodo('d', permisos);
			}else{
				return no_existe_dir_intermedio;
			}
		}else{
			entrada.inodo = reservar_inodo('f', permisos);
		}
		if(mi_write_f(*p_inodo_dir, &entrada, nentrada*sizeof(Entrada), sizeof(Entrada)) < 0){
			if(entrada.inodo != -1){
				liberar_inodo(entrada.inodo);
			}
			return sin_permisos_escritura;
		}
	}
	
	if((strcmp(final, "/") == 0)||(strcmp(final, "") == 0)){
		if((existe == 1) && (reservar == '1')) return entrada_ya_existe;
		*p_inodo = entrada.inodo;
		*p_entrada = nentrada;
		return 0;
	}else{
		*p_inodo_dir = entrada.inodo;
		return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
	}
}

int mi_creat(const char *camino, unsigned char permisos){
	
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	int a;
	mi_waitSem();
	a = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, '1', permisos);
	if(a < 0){
		switch(a){
			case -1: printf("Error al extraer camino\n"); break;
			case -2: printf("Sin permisos de lectura\n"); break;
			case -3: printf("No existe entrada\n"); break;
			case -4: printf("No existe directorio intermedio\n"); break;
			case -5: printf("Sin permisos escritura\n"); break;
			case -6: printf("Entrada ya existe\n"); break;
		}
		mi_signalSem();
		return -1;
	}
	mi_signalSem();
	return 0;
}

int mi_dir(const char *camino, char *buffer){

	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	int a;
	a = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, '0', '0');
	if(a < 0){
		switch(a){
			case -1: printf("Error al extraer camino\n"); return -1;
			case -2: printf("Sin permisos de lectura\n"); return -1;
			case -3: printf("No existe entrada\n"); return -1;
			case -4: printf("No existe directorio intermedio\n"); return -1;
			case -6: printf("Entrada ya existe\n"); return -1;
		}
		return -1;
	}
	
	Inodo inodo = leer_inodo(p_inodo);
	if(!(inodo.permisos & 4)){
		printf("Sin permisos de lectura\n");
		return -1;
	}
	if(inodo.tipo != 'd'){
		printf("No es directorio\n");
		return -1;
	}
	char empty[60];
	memset(empty, 0, sizeof(empty));
	Entrada arrayentradas[BLOCKSIZE/(sizeof(Entrada))];
	memset(arrayentradas, 0, sizeof(arrayentradas));
	int nbloque = 0;
	int nentradas = 0;
	int numbloques;
	if(inodo.bytesSize%BLOCKSIZE == 0){
		numbloques = inodo.bytesSize/BLOCKSIZE;
	}else{
		numbloques = inodo.bytesSize/(BLOCKSIZE) + 1; // +1? -SÍ
	}
	if(mi_read_f(p_inodo, &arrayentradas, nbloque*BLOCKSIZE, BLOCKSIZE) < 0) return -1;
	while(nbloque < numbloques){
		int salir = 0;
		int a = 0;
		while(a < (BLOCKSIZE/(sizeof(Entrada)))&&(salir == 0)){
			if(strcmp(arrayentradas[a].nombre, empty) != 0){
				nentradas++;
				strcat(buffer, arrayentradas[a].nombre); // Concatena nombre
				Inodo i = leer_inodo(arrayentradas[a].inodo);
				if(i.tipo == 'd'){
					strcat(buffer, "/");
				}
				// Concatena permisos
				strcat(buffer, ",");
				if(i.permisos & 4){
					strcat(buffer,"r");
				}else{
					strcat(buffer,"-");
				}
				strcat(buffer, ",");
				if(i.permisos & 2){
					strcat(buffer,"w");
				}else{
					strcat(buffer,"-");
				}
				strcat(buffer, ",");
				if(i.permisos & 1){
					strcat(buffer,"x");
				}else{
					strcat(buffer,"-");
				}
				// Concatena datos inodo
				char relleno[32];
				sprintf(relleno, "  %d  %d  %d  ", arrayentradas[a].inodo, i.nentradas, i.bytesSize);
				strcat(buffer,relleno);
				// Concatena fecha y hora
				struct tm *tm;
				char tmp[100];
				tm = localtime(&i.mtime); 
				sprintf(tmp,"%d-%02d-%02d %02d:%02d:%02d\t",tm->tm_year+1900,
				tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
				strcat(buffer,tmp);
				// Salto de linea para siguiente entrada
				strcat(buffer, "\n");
			}else{
				salir = 1;
			}
			a++;
		}
		nbloque++;
		if((salir == 0)&&(nbloque < numbloques)){
			memset(arrayentradas, 0, sizeof(arrayentradas));
			mi_read_f(p_inodo, &arrayentradas, nbloque*BLOCKSIZE, BLOCKSIZE);
		}
	}
	return nentradas;
}

int mi_link(const char *camino1, const char *camino2){
	
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	Entrada entrada;
	int ninodo = 0;
	if(*(camino1+strlen(camino1)-1)!='/'){ // si camino1 no es un directorio entra en if
		mi_waitSem();
		int a = buscar_entrada(camino1, &p_inodo_dir, &p_inodo, &p_entrada, '0', '0');
			if(a == 0){ // si camino1 existe...
			Inodo perm = leer_inodo(p_inodo); // Leemos inodo para obtener los permisos del fichero1 y ponerlos en el fichero2
			ninodo = p_inodo;
			p_inodo_dir = 0;
			p_inodo = 0;
			p_entrada = 0;
			char permiso = perm.permisos;
			
			int b = buscar_entrada(camino2, &p_inodo_dir, &p_inodo, &p_entrada, '1', permiso);
			if((b == 0)||(b == -6)){
				if(mi_read_f(p_inodo_dir, &entrada, p_entrada*sizeof(Entrada), sizeof(Entrada)) < 0){
					printf("Error al leer entrada\n");
					return -1;
				}
				liberar_inodo(entrada.inodo);
				entrada.inodo = ninodo;
				if(mi_write_f(p_inodo_dir, &entrada, p_entrada*sizeof(Entrada), sizeof(Entrada)) < 0){
					printf("Error al escribir entrada\n");
					return -1;
				}
				Inodo inodo = leer_inodo(ninodo);
				inodo.nentradas++;
				inodo.ctime = time(NULL);
				escribir_inodo(inodo, ninodo);
				
			}else{
				switch(b){
					case -1: printf("Error al extraer camino\n"); break;
					case -2: printf("Sin permisos de lectura\n"); break;
					case -3: printf("No existe entrada\n"); break;
					case -4: printf("No existe directorio intermedio\n"); break;
					case -5: printf("Sin permisos escritura\n"); break;
				}
				mi_signalSem();
				return -1;
			}
		}else{
			switch(a){
				case -1: printf("Error al extraer camino\n"); break;
				case -2: printf("Sin permisos de lectura\n"); break;
				case -3: printf("No existe entrada\n"); break;
				case -4: printf("No existe directorio intermedio\n"); break;
				case -5: printf("Sin permisos escritura\n"); break;
			}
				mi_signalSem();
			return -1;
		}
				mi_signalSem();
	}
	return 0;
}

int mi_unlink(const char *camino){
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	mi_waitSem();
	int a = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, '0', '0');
	if(a == 0){
		Inodo inodo = leer_inodo(p_inodo_dir);
		int nentradas = ((inodo.bytesSize)/(sizeof(Entrada)));
		if(p_entrada != (nentradas-1)){
			Entrada entrada;
			if(mi_read_f(p_inodo_dir, &entrada, inodo.bytesSize-(sizeof(Entrada)), sizeof(Entrada)) < 0){
				mi_signalSem();
				return -1;
			}
			if(mi_write_f(p_inodo_dir, &entrada, p_entrada*sizeof(Entrada), sizeof(Entrada)) < 0){
				mi_signalSem();
				return -1;
			}
		}
		if(mi_truncar_f(p_inodo_dir, inodo.bytesSize - (sizeof(Entrada))) < 0){
			mi_signalSem();
			return -1;
		}
		Inodo i = leer_inodo(p_inodo);
		i.nentradas--;
		if(i.nentradas < 1){
			liberar_inodo(p_inodo);
		}else{
			i.ctime = time(NULL);
			escribir_inodo(i, p_inodo);
		}
	}else{
		switch(a){
			case -1: printf("Error al extraer camino\n"); break;
			case -2: printf("Sin permisos de lectura\n"); break;
			case -3: printf("No existe entrada\n"); break;
			case -4: printf("No existe directorio intermedio\n"); break;
			case -5: printf("Sin permisos escritura\n"); break;
		}
		mi_signalSem();
		return -1;
	}
	mi_signalSem();
	return 0;
}

int mi_chmod(const char *camino, unsigned char permisos){
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	int a = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, '0', permisos);
	if(a < 0){
		switch(a){
			case -1: printf("Error al extraer camino\n"); break;
			case -2: printf("Sin permisos de lectura\n"); break;
			case -3: printf("No existe entrada\n"); break;
			case -4: printf("No existe directorio intermedio\n"); break;
			case -5: printf("Sin permisos escritura\n"); break;
			case -6: printf("Entrada ya existe\n"); break;
		}
		return -1;
	}
	int s = mi_chmod_f(p_inodo, permisos);
	return s;
}

int mi_stat(const char *camino, STAT *p_stat){
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	int a = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, '0', '0');
	if(a < 0){
		switch(a){
			case -1: printf("Error al extraer camino\n"); break;
			case -2: printf("Sin permisos de lectura\n"); break;
			case -3: printf("No existe entrada\n"); break;
			case -4: printf("No existe directorio intermedio\n"); break;
			case -5: printf("Sin permisos escritura\n"); break;
			case -6: printf("Entrada ya existe\n"); break;
		}
		return -1;
	}
	int b = mi_stat_f(p_inodo, p_stat);
	return b;
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){
	int p_inodo = -2;
	if(!isempty(1)){
		get(1, camino, &p_inodo); // Buscamos en la caché la entrada
	}
	if(p_inodo < 0){
		p_inodo = 0;
		unsigned int p_inodo_dir = 0;
		unsigned int p_entrada = 0;
		//mi_waitSem();
		int a = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, '0', '0');
		if(a < 0){
			switch(a){
				case -1: printf("Error al extraer camino\n"); break;
				case -2: printf("Sin permisos de lectura\n"); break;
				case -3: printf("No existe entrada\n"); break;
				case -4: printf("No existe directorio intermedio\n"); break;
				case -5: printf("Sin permisos escritura\n"); break;
				case -6: printf("Entrada ya existe\n"); break;
			}
			//mi_signalSem();
			return -1;
		}
		add(1, camino, p_inodo); // Añadimos entrada a la caché
		//mi_signalSem();
	}
	int bl = mi_read_f(p_inodo, buf, offset, nbytes); // Leemos entrada
	return bl;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
	int p_inodo = -2;
	if(!isempty(0)){
		get(0, camino, &p_inodo);
	}
	if(p_inodo < 0){
		p_inodo = 0;
		unsigned int p_inodo_dir = 0;
		unsigned int p_entrada = 0;
		mi_waitSem();
		int a = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, '0', '0');
		
		if(a < 0){
			switch(a){
				case -1: printf("Error al extraer camino\n"); break;
				case -2: printf("Sin permisos de lectura\n"); break;
				case -3: printf("No existe entrada\n"); break;
				case -4: printf("No existe directorio intermedio\n"); break;
				case -5: printf("Sin permisos escritura\n"); break;
				case -6: printf("Entrada ya existe\n"); break;
			}
			mi_signalSem();
			return -1;
		}
		
		add(0, camino, p_inodo); // Añadimos entrada a la caché
		mi_signalSem();
	}
	mi_waitSem();
	int be = mi_write_f(p_inodo, buf, offset, nbytes);
	mi_signalSem();
	return be;
}

