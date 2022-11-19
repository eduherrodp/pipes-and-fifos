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

char *create_account() {
    // Verify if the account already exists
    if (verify_account(reg.id) == 1) {
        printf("The client id already exists (%d)\n", reg.id);
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
        // Add the new account to the file called "accounts.txt"
        fprintf(fp, "\n%d 0", reg.id); // The balance is 0
        fclose(fp);
        // This message only will be shown in the server
        printf("Client has created a new account with the id: %d\n", reg.id);
        return "[SERVER]: Account created successfully";
    }
}

// Function to deposit money or withdraw money
char *DWMoney() {
    char *message;
    // if the operation is 2, deposit money, else withdraw money
    // Verify if the account exists 
    // ! the parameter reg.id is not necessary because is in the structure
    if ( verify_account(reg.id) == 1 ) {
        FILE *fp;
        fp = open("accounts.txt", "r+");
        int id, balance;
        if (reg.op == 2) {
            // Deposit the money
            while (fscanf(fp, "%d %d", &id, &balance) != EOF) {
                if (id == reg.id) {
                    balance += reg.amount;
                    printf("New balance: %d", balance);
                    message = "[SERVER]: Deposit successfully (%d)\n Your new balance: %d", reg.amount, balance;
                } else {
                    printf("Error to deposit money");
                }
            }
            fclose(fp);
            return message;
        } else {
            // Verify if the account has enough money
            if (verify_balance(reg.id, reg.amount) == 1) {
                // Withdraw the money
                while (fscanf(fp, "%d %d", &id, &balance) != EOF) {
                    if (id == reg.id) {
                        balance -= reg.amount;
                        printf("New balance: %d", balance);
                        message = "[SERVER]: Withdraw successfully (%d)\n Your new balance: %d", reg.amount, balance;
                    } else {
                        printf("Error to withdraw money");
                    }
                }
                fclose(fp);
                return message;
            } else {
                printf("Error: The account doesn't have enough money (%d)", reg.id);
                return "[SERVER]: The account doesn't have enough money";
            }
        }
    } else {
        printf("Error: The account doesn't exist (%d)", reg.id);
        return "[SERVER]: Account doesn't exist";
    }
}

int main(void) {
    system("clear");
    // Print "####[LOG]####" with foreground color white and background color green"
    printf("####[LOG]####\n"); // 
    FILE *fp;
    char readBuf[80];
    // Create the FIFO if it does not exist
    umask(0);
    mknod(FIFO_FILE, S_IFIFO | 0666, 0);
    do {
        printf("Waiting for a client...\n");
        // Open the FIFO for read only
        fp = fopen(FIFO_FILE, "r");
        // Read the data from the FIFO
        fgets(readBuf, sizeof(readBuf), fp);
        // Print the data read from the FIFO
        printf("Option received: [%s]\n", readBuf);
        // Convert the string to a struct
        sscanf(readBuf, "%d",&reg.op);
        // Close the FIFO
        fclose(fp);

        switch(reg.op) {
            case 1:
                // Create a new account
                printf("[+]Creating a new account...\n");
                // Open the FIFO for read only
                fp = fopen(FIFO_FILE, "r");
                // Read the data from the FIFO
                fgets(readBuf, sizeof(readBuf), fp);
                // Print the data read from the FIFO
                printf("ID received: [%s]\n", readBuf);
                // Convert the string to a struct
                sscanf(readBuf, "%d",&reg.id);
                // Close the FIFO
                fclose(fp);

                // Open the FIFO for write only
                fp = fopen(FIFO_FILE, "w");
                // Write the data to the FIFO
                fputs(create_account(), fp);
                // Close the FIFO
                fclose(fp);
                break;
            case 2:
                // Deposit Money
                printf("[+]Depositing money...\n");
                fp = fopen(FIFO_FILE, "r");
                fgets(readBuf, sizeof(readBuf), fp);
                printf("ID received: [%s]\n", readBuf);
                sscanf(readBuf, "%d", &reg.id);
                fclose(fp);
                fp = fopen(FIFO_FILE, "w");
                fputs(DWMoney(), fp);
                fclose(fp);
                break;
            case 3:
                
                break;
            case 0: // Exit
                printf("Exiting...\n");
                break;
            default:
            
                printf("Invalid option");
        }
    } while (reg.op != 0);

    
    return 0;
}