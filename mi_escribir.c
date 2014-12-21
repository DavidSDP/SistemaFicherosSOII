#include "directorios.h"

int main(int argc, char **argv){
	if(argc != 4){ // ./mi_escribir disco path fichero
		printf("Faltan argumentos\n");
		return -1;
	}
	
	FILE *archivo;
	int c;
	archivo = fopen(argv[3], "r");
	if(archivo < 0) return -1;
	
	bmount(argv[1]);
	empty(0);
	empty(1);
	
	
	
	int i = 0;
	int tamanyoinodo = tamanyo_archivo(argv[2]);
	if(tamanyoinodo < 0){
		printf("Error: no es un fichero\n");
		return -1;
	}
	int posicion = tamanyoinodo;
	int contador;
	char buffer[BLOCKSIZE];
	char aux[1];
	memset(buffer,0, sizeof(buffer));
	c = getc(archivo);
	while(c != EOF){
		contador = 1;
		while((c != EOF)&&(contador != BLOCKSIZE)){
			memset(aux, 0 ,sizeof(aux));
			sprintf(aux, "%c", c);
			strcat(buffer, aux);
			contador++;
			c = getc(archivo);
		}
		mi_write(argv[2], &buffer, (posicion)+(i*sizeof(buffer)), contador);
		memset(buffer,0, sizeof(buffer));
		i++;
	}
	
	if(fclose(archivo) < 0) return -1;
	bumount();
	
	return 0;
	
}


int tamanyo_archivo(char *ruta){
	
	STAT estado;
	if(mi_stat(ruta, &estado) < 0){
		printf("Fichero no encontrado\n");
		return -1;
	}
	if(estado.tipo != 'f'){
		printf("No es fichero\n");
		return -1;
	}
	return estado.bytesSize;
	
}





