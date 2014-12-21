#include "directorios.h"

void mostrarStat(STAT *p_stat);

int main(int argc, char **argv){
	if(argc != 3){
		printf("Faltan argumentos\n");
		return -1;
	}
	bmount(argv[1]);
	empty(0);
	empty(1);
	STAT stat;
	if(mi_stat(argv[2], &stat) < 0){
		printf("Error al mostrar estado del inodo\n");
		return -1;
	}
	printf("Inodo de la ruta '%s': \n", argv[2]);
	mostrarStat(&stat);
	
	bumount();
	return 0;
	
}

void mostrarStat(STAT *p_stat){

	printf("	Tipo: %c\n", p_stat->tipo);
	printf("	Permisos: %c\n", p_stat->permisos);
	
	// Imprime fecha y hora
	struct tm *tm;
	char tmp[100];
	tm = localtime(&(p_stat->atime)); 
	sprintf(tmp,"%d-%02d-%02d %02d:%02d:%02d\t",tm->tm_year+1900,
	tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	printf("	atime: %s\n", tmp);
	memset(tmp, 0 ,sizeof(tmp));
	tm = localtime(&(p_stat->mtime)); 
	sprintf(tmp,"%d-%02d-%02d %02d:%02d:%02d\t",tm->tm_year+1900,
	tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	printf("	mtime: %s\n", tmp);
	memset(tmp, 0 ,sizeof(tmp));
	tm = localtime(&(p_stat->ctime)); 
	sprintf(tmp,"%d-%02d-%02d %02d:%02d:%02d\t",tm->tm_year+1900,
	tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	printf("	ctime: %s\n", tmp);
	
	printf("	Numero de enlaces: %u\n", p_stat->nentradas);
	printf("	Tamaño del inodo: %u bytes\n", p_stat->bytesSize);
	printf("	Bloques ocupados: %u\n", p_stat->bloquesOcupados);
	printf("\n");
}

