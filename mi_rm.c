#include "directorios.h"

int directorio_vacio(char *ruta);

int main(int argc, char **argv){
	if(argc != 3){
		printf("Faltan argumentos\n");
		return -1;
	}
	
	bmount(argv[1]);
	empty(0);
	empty(1);
	
	if(argv[2][strlen(argv[2])-1] == '/'){
		if(directorio_vacio(argv[2]) < 1){
			printf("Directorio no vacío\n");
			return -1;
		}
	}
	
	if(mi_unlink(argv[2]) < 0){
		printf("Error al desenlazar los ficheros/directorios\n");
		return -1;
	}
	
	bumount();
	
	printf("Fichero/directorio desenlazado correctamente\n");
	
	return 0;
	
}

int directorio_vacio(char *ruta){
	
	STAT estado;
	if(mi_stat(ruta, &estado) < 0){
		printf("Directorio no encontrado\n");
		return -1;
	}
	if(estado.tipo != 'd'){
		printf("No es directorio\n");
		return -1;
	}
	if(estado.bytesSize == 0) return 1;
	return 0;
	
}



