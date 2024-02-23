/* Ex12.
Consider a supermarket with 5 barcode readers distributed along the shop. Every time a customer uses a barcode
reader the product’s name and its price should be printed on the screen. Simulate this system through processes
and pipes:
a. The parent process has access to the products database;
b. Each child process represents a barcode reader;
c. There is a pipe shared by all 5 child processes to request the product’s information;
d. The parent process replies to the requesting child with the corresponding product’s information through
a pipe that it shares only with it. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#define NUM_PRODUCTS 5
#define NUM_BARCODE_READERS 5

#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

typedef struct {
    int id;
    char name[20];
    float price;
} Product;

int main() {
    int i;
    int child_to_parent[2];
    int parent_to_child[NUM_BARCODE_READERS][2];
    int product_id;
    int child_idx;
    pid_t pid;

    // Create pipes
    if (pipe(child_to_parent) == -1) {
        perror("child_to_parent pipe");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < NUM_BARCODE_READERS; i++) {
        if (pipe(parent_to_child[i]) == -1) {
            perror("parent_to_child pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Create child processes for barcode readers
    for (i = 0; i < NUM_BARCODE_READERS; i++) {
        pid = fork();
        
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            close(parent_to_child[i][PIPE_WRITE_END]); // Close unused write end
            close(child_to_parent[PIPE_READ_END]); // Close unused read end

            product_id = i + 1;
            child_idx = i;

            while (1) {
                // Request info from parent process
                write(child_to_parent[PIPE_WRITE_END], &product_id, sizeof(product_id));
                write(child_to_parent[PIPE_WRITE_END], &child_idx, sizeof(child_idx));

                // Receive product information from parent process
                Product p;
                read(parent_to_child[i][PIPE_READ_END], &p, sizeof(Product));

                // Print product information
                printf("Child with pid %d printed: ID %d. %s - $%.2f\n", getpid(), p.id, p.name, p.price);
                break;
            }
            close(parent_to_child[i][PIPE_READ_END]); // Close write end
            close(child_to_parent[PIPE_WRITE_END]); // Close read end
            exit(EXIT_SUCCESS);
        } 
    }

    Product products[NUM_PRODUCTS] = {
        {1, "Product 1", 1.99},
        {2, "Product 2", 2.99},
        {3, "Product 3", 3.99},
        {4, "Product 4", 4.99},
        {5, "Product 5", 5.99}
    };
    
    // Parent process
    close(child_to_parent[PIPE_WRITE_END]); // Close write end
    for (i = 0; i < NUM_PRODUCTS; i++) {
        close(parent_to_child[i][PIPE_READ_END]); // Close read end

        // Receive request from a child process
        read(child_to_parent[PIPE_READ_END], &product_id, sizeof(product_id));
        read(child_to_parent[PIPE_READ_END], &child_idx, sizeof(child_idx));

        // Find product information 
        if (product_id == products[i].id) {
            // and send back to the same child process
            write(parent_to_child[child_idx][PIPE_WRITE_END], &products[i], sizeof(Product));
        }
        close(parent_to_child[i][PIPE_WRITE_END]); // Close write end    
    }
    close(child_to_parent[PIPE_READ_END]);

    // Wait for child processes to finish
    wait(NULL);
    
    exit(EXIT_SUCCESS);

    return 0;
}

/*
./ex12
Child with pid 28765 printed: ID 1. Product 1 - $1.99
Child with pid 28767 printed: ID 3. Product 3 - $3.99
Child with pid 28766 printed: ID 2. Product 2 - $2.99
Child with pid 28768 printed: ID 4. Product 4 - $4.99
Child with pid 28769 printed: ID 5. Product 5 - $5.99
*/