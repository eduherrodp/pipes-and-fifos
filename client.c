// Path: client.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/stat.h>
#include <string.h>
#define FIFO_FILE "MYFIFO"

// Function to verify if the account exists if the account exists return 1, else return 0
int verify_account(int id) {
    FILE *fp;
    fp = fopen("accounts.txt", "r");
    int id_file;
    while (fscanf(fp, "%d", &id_file) != EOF) {
        if (id_file == id) {
            return 1;
        }
    }
    return 0;
}

// Definition of the struct that will be used to send the data
struct reg {
    int id;
    int op;
    int amount;
} reg;

int main(void) {

    // Creating the named fifo
    mknod(FIFO_FILE, S_IFIFO | 0666, 0);

    char readbuf[80];
    char writeBuf[80];

    // Show the menu to the client
    printf(" 1 - Create account\n 2 - Deposit\n 3 - Withdraw\n 4 - Exit\n");
    printf("\nEnter the option: ");
    scanf("%d", &reg.op);

    switch (reg.op) {
        case 1:
            system("clear");
            // The client will be to create a new account with balance 0
            printf("Creating a new account...\n\n");
            // Search what is the last id in the file
            printf("Please, insert a ID for the new account: ");
            scanf("%d", &reg.id);

            // Open the fifo for writing
            FILE *fp = fopen(FIFO_FILE, "w");
            if (fp == NULL) {
                printf("Error opening file");
                exit(1);
            }
            // Put the data in the FIFO file
            fprintf(fp, "%d", reg.id);
            fclose(fp);

            // Open the fifo for reading
            fp = fopen(FIFO_FILE, "r");
            fgets(readbuf, sizeof(readbuf), fp);
            printf("\n%s", readbuf);
            fclose(fp);

            break;
    }

    return 0;
}