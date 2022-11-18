// Path client.c
#include <stdio.h>
#include <stdlib.h>
#define FIFO_FILE "MYFIFO"
FILE *fp;
// Definition of the struct that will be used to send the data
struct reg {
    int id;
    int op;
    int amount;
} reg;

int main (int argc, char *argv[]) {
    // Define the buffer that will be used to write the data to the FIFO pipe
    char buf[100];

    // Check if the user has entered the correct number of arguments
    if(argc != 4) {
        printf("Usage: %s <id> <op> <amount>", argv[0]);
        exit(1);
    }

    // Copy the data from the arguments to the struct
    reg.id = atoi(argv[1]);
    reg.op = atoi(argv[2]);
    reg.amount = atoi(argv[3]);

    // Copy the data from the struct to the buffer
    sprintf(buf, "%d %d %d", reg.id, reg.op, reg.amount);
    
    // Open the FIFO pipe controlling the errors
    if ( (fp = fopen(FIFO_FILE, "w")) == NULL ) {
        perror("Error to open the FIFO pipe");
        exit(1);
    }

    // Write the data to the FIFO pipe
    fputs(buf, fp);
    // Close the FIFO pipe
    fclose(fp);

    return 0;
}
// How to execute the program:
// $ gcc -o client client.c
// $ ./client 1 1 100
// $ ./client 1 2 50
// $ ./client 1 3 0

