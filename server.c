/*
    We have a process called server, which is has a txt file called "accounts.txt" which contains the following structure:

    ID      Saldo
    10003   2000
    1234    3500

    The server process has the following options:
    1. Create a new account
    2. Withdraw money
    3. Deposit money

    This program will be implemented using a FIFO named, so the client process will send a structure to the server process, and the server process will read the structure and do the corresponding action.

    The structure will be the following:

    struct reg {
        int id;
        int op;
        int amount;
    } reg;

*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/stat.h>
#define FIFO_FILE "MYFIFO"

// Definition of the struct that will be used to send the data
struct reg {
    int id;
    int op;
    int amount;
} reg;

//TODO Functions that will be used in the program
//* Function to create a new account
void create_account() {
    FILE *fp;
    fp = fopen("accounts.txt", "a");
    fprintf(fp, "%d %d", reg.id, reg.amount);
    fclose(fp);
}
//* Function to withdraw money
void withdraw_money() {
    FILE *fp;
    fp = fopen("accounts.txt", "r");
    char buf[100];
    while(fgets(buf, 100, fp) != NULL) {
        printf("%s", buf);
    }
    fclose(fp);
}
//* Function to deposit money
void deposit_money() {
    FILE *fp;
    fp = fopen("accounts.txt", "r");
    char buf[100];
    while(fgets(buf, 100, fp) != NULL) {
        printf("%s", buf);
    }
    fclose(fp);
}



int main(void) {
    // Define the buffer that will be used to read the data from the FIFO pipe
    char buf[100];

    // Open the FIFO pipe controlling the errors
    FILE *fp;
    if ( (fp = fopen(FIFO_FILE, "r")) == NULL ) {
        perror("Error to open the FIFO pipe");
        exit(1);
    }
    // Read the data from the FIFO pipe
    fread(buf, 100, fp);
    // Close the FIFO pipe
    fclose(fp);

    // Copy the data from the buffer to the struct
    sscanf(buf, "%d %d %d", &reg.id, &reg.op, &reg.amount);

    // Check the operation that the user wants to do
    switch(reg.op) {
        case 1:
            create_account();
            break;
        case 2:
            withdraw_money();
            break;
        case 3:
            deposit_money();
            break;
        default:
            printf("Error: Invalid operation");
            break;
    }

    return 0;
}

