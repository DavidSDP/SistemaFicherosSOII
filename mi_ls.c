#include "directorios.h"

int main(int argc, char **argv){
	if(argc != 3){
		printf("Faltan argumentos\n");
		return -1;
	}
	if(argv[2][strlen(argv[2])-1] != '/'){
		printf("No es directorio\n");
		return -1;
	}
	bmount(argv[1]);
	empty(0);
	empty(1);
	
	char buffer[50*BLOCKSIZE]; //51200 caracteres máximo, unas 900 entradas
	memset(buffer, 0, sizeof(buffer));
	int a = mi_dir(argv[2], buffer);
	if(a < 0){
		printf("Error al listar directorio\n");
		return -1;
	}
	printf("Contenido del directorio '%s':\n", argv[2]);
	printf("%s\n", buffer);
	
	bumount();
	return 0;
	
}


