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

// Class that will be used to read the file "accounts.txt"

class File {
    private:
        FILE *file;
        char *filename;
        char *mode;
    public:
        File(char *filename, char *mode) {
            this->filename = filename;
            this->mode = mode;
            this->file = fopen(filename, mode);
        }

        // Method that will be used to write the data to the file
        void write(struct reg reg) {
            fprintf(this->file, "%d\t%d ", reg.id, reg.amount);
        }
        // Method that will be used to read the data from the file
        void read(struct reg reg) {
            fscanf(this->file, "%d\t%d", &reg.id, &reg.amount);
        }
        // Method that will be used to close the file
        void close() {
            fclose(this->file);
        }

        // Method that will be used to search for an ID in the file
        int search(int id) {
            int found = 0;
            while(!feof(this->file)) {
                fscanf(this->file, "%d\t%d", &reg.id, &reg.amount);
                if(reg.id == id) {
                    found = 1;
                    break;
                }
            }
            return found;
        }

        // Method that will be used to update the data in the file
        void update(int id, int amount) {
            int found = 0;
            FILE *temp = fopen("temp.txt", "w");
            while(!feof(this->file)) {
                fscanf(this->file, "%d\t%d", &reg.id, &reg.amount);
                if(reg.id == id) {
                    found = 1;
                    fprintf(temp, "%d\t%d ", reg.id, amount);
                } else {
                    fprintf(temp, "%d\t%d ", reg.id, reg.amount);
                }
            }
            fclose(this->file);
            fclose(temp);
            remove(this->filename);
            rename("temp.txt", this->filename);
            this->file = fopen(this->filename, this->mode);
        }
};

int main(void) {
    // Define the buffer that will be used to read the data from the FIFO pipe
    char buf[100];

    // Create object of the File class
    File file("accounts.txt", "r+");
    
    // Create a IFO if it doesn't exist
    umask(0);
    mknod(FIFO_FILE, S_IFIFO|0666, 0);

    // Cicle that will be used to read the data from the FIFO pipe
    while(1) {
        // Open the FIFO pipe controlling the errors
        FILE *fp;
        if ( (fp = fopen(FIFO_FILE, "r")) == NULL ) {
            perror("Error to open the FIFO pipe");
            exit(1);
        }

        // Read the data from the FIFO pipe
        fgets(buf, 100, fp);
        // Close the FIFO pipe
        fclose(fp);

        // Copy the data from the buffer to the struct
        sscanf(buf, "%d %d %d", &reg.id, &reg.op, &reg.amount);

        // Check if the ID exists in the file
        if(file.search(reg.id)) {
            // If the ID exists, check the operation
            if(reg.op == 1) {
                // If the operation is 1, update the data in the file
                file.update(reg.id, reg.amount);
            } else if(reg.op == 2) {
                // If the operation is 2, update the data in the file
                file.update(reg.id, reg.amount);
            } else if(reg.op == 3) {
                // If the operation is 3, update the data in the file
                file.update(reg.id, reg.amount);
            }
        } else {
            // If the ID doesn't exist, write the data to the file
            file.write(reg);
        }
    }
}

