#include "directorios.h"

int main(int argc, char **argv){
	if(argc != 3){
		printf("Faltan argumentos\n");
		return -1;
	}
	if(argv[2][strlen(argv[2])-1] == '/'){
		printf("No es fichero\n");
		return -1;
	}
	bmount(argv[1]);
	empty(0);
	empty(1);
	int i = 0;
	int bytesleidos = 0;
	int tamanyobuffer = 1500;
	unsigned char buffer[tamanyobuffer];
	memset(buffer, 0 , sizeof(buffer));
	int a = 0;
	while((a = mi_read(argv[2], buffer, i, tamanyobuffer)) > 0){
		bytesleidos += a;
		write(1, buffer, a);
		memset(buffer, 0 , sizeof(buffer));
		i+=tamanyobuffer;
	}
	if((a < 0)&&(bytesleidos == 0)){
		printf("Error al leer el fichero\n");
		return -1;
	}
	char string[128];
	memset(string, 0 ,sizeof(string));
	sprintf(string, "Bytes leídos: %d\n", bytesleidos);
	write(2, string, strlen(string));
	bumount();
	return 0;
	
}


