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

    This program create a conversation between the client and the server, so the client will send a structure to the server, and the server will read the structure and do the corresponding action.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/stat.h>
#include <string.h>
#define FIFO_FILE "MYFIFO"

// Definition of the struct that will be used to send the data
struct reg {
    int id;
    int op;
    int amount;
} reg;

//TODO Functions that will be used in the program
//* Function to verify if the account exists if the account exists return 1, else return 0
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
// Function to verify balance
int verify_balance(int id, int amount) {
    FILE *fp;
    fp = fopen("accounts.txt", "r");
    int id_file, balance_file;
    while (fscanf(fp, "%d %d", &id_file, &balance_file) != EOF) {
        if (id_file == id) {
            if (balance_file >= amount) {
                return 1;
            }
        }
    }
    return 0;
}

//* Function to create a new account, return a string with the message to be sent to the client

char *create_account(int id) {
    // Verify if the account already exists
    if (verify_account(id) == 1) {
        printf("The client id already exists (%d)\n", id);
        return "[SERVER]: Account already exists";
    } else {
        // If not exists, create the account with the initial balance of 0
        FILE *fp;
        fp = fopen("accounts.txt", "a");
        // Check if the file was created correctly
        if (fp == NULL) {
            // This message only will be shown in the server
            perror("Error to open the file");
            exit(1);
        }
        fprintf(fp, "\n%d 0", id);
        printf("Client has created a new account with the id: %d\n", id);
        return "[SERVER]: Account created successfully";
    }
}

//* Function to withdraw money
void withdraw_money() {
    // Verify if the account exists
    if (verify_account(reg.id) == 1) {
        // Verify if the account has enough money
        if (verify_balance(reg.id, reg.amount) == 1) {
            // Withdraw the money
            FILE *fp;
            fp = fopen("accounts.txt", "r");
            int id, balance;
            while (fscanf(fp, "%d %d", &id, &balance) != EOF) {
                if (id == reg.id) {
                    balance -= reg.amount;
                    printf("New balance: %d", balance);
                } else {
                    printf("Error to withdraw money");
                }
            }
            fclose(fp);
        } else {
            printf("Error: The account doesn't have enough money");
        }
    } else {
        printf("Error: The account doesn't exist");
    }
}
//* Function to deposit money
void deposit_money() {
    // Verify if the account exists
    if (verify_account(reg.id) == 1) {
        // Deposit the money
        FILE *fp;
        fp = fopen("accounts.txt", "r");
        int id, balance;
        while (fscanf(fp, "%d %d", &id, &balance) != EOF) {
            if (id == reg.id) {
                balance += reg.amount;
                printf("New balance: %d", balance);
            } else {
                printf("Error to deposit money");
            }
        }
        fclose(fp);
    } else {
        printf("Error: The account doesn't exist");
    }
}

int main(void) {
    system("clear");
    char readBuf[80];
    char writeBuf[80]; // Proces child will write in this buffer

    // Create the FIFO if it does not exist
    umask(0);
    mknod(FIFO_FILE, S_IFIFO | 0666, 0);

    // Open the FIFO for reading
    FILE *fp = fopen(FIFO_FILE, "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }
    fgets(readBuf, sizeof(readBuf), fp);
    // Put the id in the struct
    reg.id = atoi(readBuf);
    // Close the FIFO
    fclose(fp);

    // Open the FIFO for writing
    fp = fopen(FIFO_FILE, "w");
    // Call and store into the writebuf the return value of the function create_account
    strcpy(writeBuf, create_account(reg.id));
    fprintf(fp, "%s", writeBuf);
    fclose(fp);
    return 0;
}