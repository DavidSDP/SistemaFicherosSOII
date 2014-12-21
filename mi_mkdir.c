#include "directorios.h"

int main(int argc, char **argv){
	if(argc != 4){
		printf("Faltan argumentos\n");
		return -1;
	}
	bmount(argv[1]);
	empty(0);
	empty(1);
	unsigned char permisos = argv[2][0];
	int a = mi_creat(argv[3], permisos);
	if(a < 0){
		printf("Error al crear fichero/directorio\n");
		return -1;
	}
	bumount();
	int len = strlen(argv[3]);
	if(argv[3][len-1]=='/'){
		return 0; //directorio
	}
	return 1; //fichero
	
}



