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
    FILE *fp;
    fp = fopen("accounts.txt", "r");
    int id, balance;

    // Deposit the money
    rewind(fp);
    while (fscanf(fp, "%d %d", &id, &balance) != EOF) {
        // When the id will be founded modify the balance in the file
        if (id == reg.id) {

            // Check if the user wants deposit or withdraw money

            if(reg.op == 2) {
                // Deposit money
                balance += reg.amount;

                printf("The client with the id: %d has deposited %d\n", reg.id, reg.amount);
                message = "[SERVER]: Deposit successfully";
            } else {
                // Withdraw money

                // Verify if the balance is enough to withdraw the money
                if (verify_balance(reg.id, reg.amount) == 1) {
                    balance -= reg.amount;
                    printf("The client with the id: %d has withdrawn %d\n", reg.id, reg.amount);
                    message = "[SERVER]: Withdraw successfully";
                } else {
                    printf("The client with the id: %d has not enough balance to withdraw %d\n", reg.id, reg.amount);
                    message = "[SERVER]: Not enough balance";
                }
            }
        }
        // Write the new data in a temporal file
        FILE *fp_temp;
        fp_temp = fopen("temp.txt", "a");
        if (fp_temp == NULL) {
            perror("Error to open the file");
            exit(1);
        }
        if (ftell(fp_temp) != 0) {
            fprintf(fp_temp, "\n%d %d", id, balance);
        } else {
            fprintf(fp_temp, "%d %d", id, balance);
        }
        fclose(fp_temp);       
    }
    fclose(fp);
    remove("accounts.txt");
    rename("temp.txt", "accounts.txt");
    return message;
}

int main(void) {
    system("clear");
    int temp;
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
                temp = verify_account(reg.id);
                // Put the data of temp in the FIFO
                fprintf(fp, "%d", temp);
                fclose(fp);

                // Verify if the account exists
                if (verify_account(reg.id) == 1) {
                    // Then, the account exists
                    
                    fp = fopen(FIFO_FILE, "r");
                    fgets(readBuf, sizeof(readBuf), fp);
                    sscanf(readBuf, "%d", &reg.amount);
                    printf("Amount to deposit to the account [%d]: %d\n", reg.id, reg.amount);
                    fclose(fp);    
                    
                    fp = fopen(FIFO_FILE, "w");
                    fputs(DWMoney(), fp);
                } else {
                    printf("\nThe account (%d) doesn't exist\n", reg.id);
                    fp = fopen(FIFO_FILE, "w");
                    fputs("[SERVER]: The account doesn't exist", fp);
                }
                fclose(fp);
                break;
            case 3:
                // Withdraw Money
                printf("[+]Withdrawing money...\n");
                fp = fopen(FIFO_FILE, "r");
                fgets(readBuf, sizeof(readBuf), fp);
                printf("ID received: [%s]\n", readBuf);
                sscanf(readBuf, "%d", &reg.id);
                fclose(fp);

                fp = fopen(FIFO_FILE, "w");
                temp = verify_account(reg.id);
                // Put the data of temp in the FIFO
                fprintf(fp, "%d", temp);
                fclose(fp);

                // Verify if the account exists

                if (verify_account(reg.id) == 1) {
                    // Then, the account exists
                    
                    fp = fopen(FIFO_FILE, "r");
                    fgets(readBuf, sizeof(readBuf), fp);
                    sscanf(readBuf, "%d", &reg.amount);
                    printf("Amount to withdraw to the account [%d]: %d\n", reg.id, reg.amount);
                    fclose(fp);    
                    
                    fp = fopen(FIFO_FILE, "w");
                    fputs(DWMoney(), fp);
                } else {
                    printf("\nThe account (%d) doesn't exist\n", reg.id);
                    fp = fopen(FIFO_FILE, "w");
                    fputs("[SERVER]: The account doesn't exist", fp);
                }
                fclose(fp);                
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