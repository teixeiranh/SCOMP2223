#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_RECORDS 100
#define SEM_RECORDS "sem_records"
#define SHM_NAME "shm_records"

// Structure to hold data records
typedef struct {
    int number;
    char name[50];
    char address[100];
} Data_Records;

// Shared memory structure
typedef struct {
    sem_t* sem_record;
    Data_Records records[MAX_RECORDS];
} SharedMemory;

void insertRecord(SharedMemory* sharedMemory) {
    sem_wait(sharedMemory->sem_record);

    // Read data record
    Data_Records newRecord;
    printf("Enter client's number: ");
    scanf("%d", &(newRecord.number));
    printf("Enter client's name: ");
    scanf("%s", newRecord.name);
    printf("Enter client's address: ");
    scanf("%s", newRecord.address);

    // Check if maximum number of records reached
    int currentRecords = 0;
    for (int i = 0; i < MAX_RECORDS; i++) {
        if (sharedMemory->records[i].number != 0) {
            currentRecords++;
        }
    }

    if (currentRecords == MAX_RECORDS) {
        printf("Maximum number of records reached.\n");
    } else {
        // Add new record to shared memory
        for (int i = 0; i < MAX_RECORDS; i++) {
            if (sharedMemory->records[i].number == 0) {
                sharedMemory->records[i] = newRecord;
                break;
            }
        }
    }

    sem_post(sharedMemory->sem_record);
}

void consultRecord(SharedMemory* sharedMemory) {
    sem_wait(sharedMemory->sem_record);

    int searchNumber;
    printf("Enter client number to search: ");
    scanf("%d", &searchNumber);

    // Search for the record with the matching identification number
    Data_Records* foundRecord = NULL;
    for (int i = 0; i < MAX_RECORDS; i++) {
        if (sharedMemory->records[i].number == searchNumber) {
            foundRecord = &(sharedMemory->records[i]);
            break;
        }
    }

    // Display the record data if found
    if (foundRecord != NULL) {
        printf("Client Number: %d\n", foundRecord->number);
        printf("Client Name: %s\n", foundRecord->name);
        printf("Client Address: %s\n", foundRecord->address);
    } else {
        printf("Record not found.\n");
    }

    sem_post(sharedMemory->sem_record);
}

void consultAllRecords(SharedMemory* sharedMemory) {
    sem_wait(sharedMemory->sem_record);

    // Iterate through the shared memory and display all records with valid data
    for (int i = 0; i < MAX_RECORDS; i++) {
        if (sharedMemory->records[i].number != 0) {
            printf("Client Number: %d\n", sharedMemory->records[i].number);
            printf("Client Name: %s\n", sharedMemory->records[i].name);
            printf("Client Address: %s\n", sharedMemory->records[i].address);
            printf("\n");
        }
    }

    sem_post(sharedMemory->sem_record);
}

int main() {
    // Create or open the shared memory object
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Set the size of the shared memory object
    if (ftruncate(fd, sizeof(SharedMemory)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // Map the shared memory object into the address space
    SharedMemory* sharedMemory = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Create or open the sem_record semaphore
    sharedMemory->sem_record = sem_open(SEM_RECORDS, O_CREAT, 0666, 1);
    if (sharedMemory->sem_record == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // Menu loop
    int choice;
    while (1) {
        printf("1. Insert Record\n");
        printf("2. Consult Record\n");
        printf("3. Consult All Records\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                insertRecord(sharedMemory);
                break;
            case 2:
                consultRecord(sharedMemory);
                break;
            case 3:
                consultAllRecords(sharedMemory);
                break;
            case 4:
                // Clean up and exit
                sem_close(sharedMemory->sem_record);
                sem_unlink(SEM_RECORDS);
                munmap(sharedMemory, sizeof(SharedMemory));
                close(fd);
                shm_unlink(SHM_NAME);
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}


//Race conditions occur when multiple programs access and modify shared data 
//concurrently, potentially leading to inconsistent or incorrect results. 
//In this scenario, the shared memory is the critical section where race conditions 
//can occur. To handle them, we can use semaphores.
//By using semaphores, we ensure mutual exclusion and limit the number of concurrent 
//accesses to the shared memory, preventing race conditions.To test for potential 
//race conditions, we can create multiple instances of the Consult and Insert 
//programs and have them execute concurrently. Generate random identification 
//numbers and perform insert and consult operations simultaneously. 