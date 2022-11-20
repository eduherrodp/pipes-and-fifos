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
    FILE *fp;
    // Creating the named fifo
    mknod(FIFO_FILE, S_IFIFO | 0666, 0);
    char readbuf[80];
    do {
        // Show the menu to the client
        system("clear");
        printf("\nWelcome!\n\n 1 - Create account\n 2 - Deposit\n 3 - Withdraw\n 0 - Exit\n");
        printf("\nEnter the option: ");
        scanf("%d", &reg.op);
         // Open the fifo to writing
        fp = fopen(FIFO_FILE, "w");
        // Put the data in the FIFO file
        fprintf(fp, "%d", reg.op);
        fclose(fp);
        int temp;
        switch (reg.op) {
            case 1:
                system("clear");
                // The client will be to create a new account with balance 0
                printf("Creating a new account...\n\n");
                // Search what is the last id in the file
                printf("Please, insert a ID for the new account: ");
                scanf("%d", &reg.id);

                // Open the fifo for writing
                fp = fopen(FIFO_FILE, "w");
                // Put the data in the FIFO file
                fprintf(fp, "%d", reg.id);
                fclose(fp);

                // Open the fifo for reading
                fp = fopen(FIFO_FILE, "r");
                fgets(readbuf, sizeof(readbuf), fp);
                printf("\n%s", readbuf);
                fclose(fp);
                // Wait 3 seconds
                sleep(2);
                break;
            case 2:
                system("clear");
                printf("Depositing money...\n\n");
                printf("Please, insert your ID: ");
                scanf("%d", &reg.id);
                
                // First verify if the account exists
                
                fp = fopen(FIFO_FILE, "w");
                fprintf(fp, "%d", reg.id);
                fclose(fp);
                
                // Get the data from the FIFO file
                fp = fopen(FIFO_FILE, "r");
                fgets(readbuf, sizeof(readbuf), fp);
                sscanf(readbuf, "%d", &temp);
                fclose(fp);

                if (temp == 1) {
                    printf("\n\tAccount found! [%d]\n", reg.id);
                    printf("Please, insert the amount to deposit: ");
                    scanf("%d", &reg.amount);
                    // Open the fifo for writing
                    fp = fopen(FIFO_FILE, "w");
                    // Put the data in the FIFO file
                    fprintf(fp, "%d", reg.amount);
                    fclose(fp);
                    // Open the fifo for reading
                    fp = fopen(FIFO_FILE, "r");
                    fgets(readbuf, sizeof(readbuf), fp);
                    printf("\n%s", readbuf);
                    fclose(fp);

                } else {
                    printf("The account does not exist!\n");
                }
                sleep(2);
                break;
            case 3:
                system("clear");
                printf("Withdrawing money...\n\n");
                printf("Please, insert your ID: ");
                scanf("%d", &reg.id);

                // First verify if the account exists
                fp = fopen(FIFO_FILE, "w");
                fprintf(fp, "%d", reg.id);
                fclose(fp);

                // Get the data from the FIFO file
                fp = fopen(FIFO_FILE, "r");
                fgets(readbuf, sizeof(readbuf), fp);
                sscanf(readbuf, "%d", &temp);
                fclose(fp);

                if (temp == 1) {
                    printf("\n\tAccount found! [%d]\n", reg.id);
                    printf("Please, insert the amount to withdraw: ");
                    scanf("%d", &reg.amount);
                    // Open the fifo for writing
                    fp = fopen(FIFO_FILE, "w");
                    // Put the data in the FIFO file
                    fprintf(fp, "%d", reg.amount);
                    fclose(fp);
                    // Open the fifo for reading
                    fp = fopen(FIFO_FILE, "r");
                    fgets(readbuf, sizeof(readbuf), fp);
                    printf("\n%s", readbuf);
                    fclose(fp);
                } else {
                    printf("The account does not exist!\n");
                }
                sleep(2);
                break;
            case 0:
                system("clear");
                printf("Exiting...\n\n");
                sleep(2);
                break;
            default:
                system("clear");
                printf("Invalid option!\n\n");
                sleep(2);
                break;
        }
    } while(reg.op != 0);

    

    return 0;
}