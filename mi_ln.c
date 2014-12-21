#include "directorios.h"

int main(int argc, char **argv){
	if(argc != 4){ // ./mi_ln disco_imagen fichero_origen fichero_destino
		printf("Faltan argumentos\n");
		return -1;
	}
	if(argv[2][strlen(argv[2])-1] == '/'){
		printf("No es fichero\n");
		return -1;
	}
	if(argv[3][strlen(argv[3])-1] == '/'){
		printf("No es fichero\n");
		return -1;
	}
	
	bmount(argv[1]);
	empty(0);
	empty(1);
	if(mi_link(argv[2], argv[3]) < 0){
		printf("Error al enlazar los ficheros\n");
		return -1;
	}
	
	bumount();
	
	printf("Fichero enlazado correctamente\n");
	
	return 0;
	
}


