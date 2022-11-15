#include <stdio.h>
#include <stdlib.h>
#define FIFO_FILE "MIFIFO"
FILE *fp;	
size_t result;
struct reg{
    int id;
    int opc;
    int saldo;
} registro;

int main(int argc, char *argv[]){
    registro.opc = 1;
    registro.ID = 30330;
    registro.saldo = 1100;
    if ( (fp = fopen(FIFO_FILE, "w")) == NULL ) {
        perror("Error al esccribir en la tuberia\n");
        exit(1);
    }
    fwrite(&registro, sizeof(struct reg), 1, fp);
    fclose(fp);
    return 0;
}