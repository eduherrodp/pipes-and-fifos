#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/stat.h>
#define FIFO_FILE "MIFIFO"
struct reg {
    int id;
    int opc;
    int saldo;
} registro;

int main(void) {
    FILE *fp;
    char readbuf[80];
    umask(0);
    mknod(FIFO_FILE, S_IFIFO|0666, 0);
    while (1) {
        if ( (fp = fopen(FIFO_FILE, "r")) == NULL ) {
            printf("No se puede abrir la tubería\n");
            return 0;
        }
        fread(&registro, sizeof(struct reg), 1, fp);
        printf("ID: %d, Opción: %d, Saldo: %d\n", registro.id, registro.opc, registro.saldo);
        fclose(fp);
    }
    return 0;
}