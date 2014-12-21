#include "directorios.h"

char busqueda[4*BLOCKSIZE];

int found;

int lookinto(unsigned char *path);
int addline(unsigned char *path, int ninodo, Inodo i);

int main(int argc, char **argv){
	if(argc != 3){ // ./mi_search disco_imagen nombre_de_fichero
		printf("Faltan argumentos\n");
		return -1;
	}
	found = 0;
	memset(busqueda, 0, sizeof(busqueda));
	strcpy(busqueda, argv[2]);
	
	bmount(argv[1]);
	empty(0);
	empty(1);
	
	lookinto("/");
	
	bumount();
	char encontrados[512];
	memset(encontrados, 0, sizeof(encontrados));
	sprintf(encontrados, "\nFound %d matches\n", found);
	write(1, encontrados, sizeof(encontrados));
	return 0;
	
}


int lookinto(unsigned char *path){
	Entrada entrada;
	memset(&entrada, 0, sizeof(Entrada));
	int i = 0;
	unsigned char ruta[4*BLOCKSIZE];
	unsigned char path2[4*BLOCKSIZE];
	while(mi_read(path, &entrada, i*sizeof(Entrada), sizeof(Entrada)) > 0){
		Inodo inodo = leer_inodo(entrada.inodo);
		if(inodo.permisos & 4){ //Si tiene permisos de lectura continuamos
			if(strcmp(entrada.nombre, busqueda) == 0){
				memset(ruta, 0, sizeof(ruta));
				if(inodo.tipo == 'd'){
					sprintf(ruta, "%s%s/", path, entrada.nombre);
				}else{
					sprintf(ruta, "%s%s", path, entrada.nombre);
				}
				found++;
				addline(ruta, entrada.inodo, inodo);
			}
			if(inodo.tipo == 'd'){
				memset(path2, 0, sizeof(path2));
				sprintf(path2, "%s%s/", path, entrada.nombre);
				lookinto(path2);
			}
		}
		i++;
		memset(&entrada, 0, sizeof(Entrada));
	}
	
}

int addline(unsigned char *path, int ninodo, Inodo i){
	unsigned char buffer[512];
	memset(buffer, 0, sizeof(buffer));
	strcat(buffer, path); // Concatena nombre
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
				sprintf(relleno, "  %d  %d  %d  ", ninodo, i.nentradas, i.bytesSize);
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
	write(1, buffer, sizeof(buffer));
	fflush(stdout);
}


