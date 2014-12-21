#include "directorios.h"

int main(int argc, char **argv){
	
	if(argc != 4){ // ./mi_chmod disco_imagen 
		printf("Faltan argumentos\n");
		return -1;
	}
	bmount(argv[1]);
	empty(0);
	empty(1);
	unsigned char permisos = argv[2][0];
	
	int a = mi_chmod(argv[3], permisos);
	if(a < 0){
		printf("Error al cambiar los permisos\n");
		return -1;
	}
	
	
	bumount();
	printf("Permisos cambiados correctamente\n");
	int len = strlen(argv[3]);
	if(argv[3][len-1]=='/'){
		return 0; //directorio
	}
	return 1; //fichero
	
	
	
	
}
