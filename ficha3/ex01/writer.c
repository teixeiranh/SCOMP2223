#include <stdio.h>
#include <fcntl.h>    // Defines O_* constants
#include <sys/mman.h> // Defines mmap() and munmap()
#include <unistd.h>
#include "student.h"
#include <sys/stat.h> // used to get shm_open() and shm_unlink()

#define SHM_NAME "student_shm"

/* Main function to implement the writer for ex01 of PL3. */
int main()
{
    const int DATA_SIZE = sizeof(Student);
    int sharedMemoryFiledes;              // File descriptor
    Student *sharedStudent; // Pointer to the shared memory containing the Student structure

    // Create a new shared memory object and set its size to the size of the Student structure
    sharedMemoryFiledes = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    // O_CREAT: Create the shared memory object if it does not exist
    ftruncate(sharedMemoryFiledes, sizeof(Student));

    // Map the shared memory object to the process's address space
    sharedStudent = (Student *)mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryFiledes, 0);
    // PROT_READ: Read-only access
    // PROT_WRITE: Write-only access
    // MAP_SHARED: Share the mapping with other processes

    // Initialize the has_data flag to 0, signaling that there is no data yet
    sharedStudent->has_data = 0;

    // Read the student data from the keyboard
    printf("Enter student number: ");
    scanf("%d", &sharedStudent->number);

    printf("Enter student name: ");
    scanf("%s", sharedStudent->name);

    printf("Enter student address: ");
    scanf("%s", sharedStudent->address);

    // Set the has_data flag to 1, signaling that the data is now available
    sharedStudent->has_data = 1;

    // Unmap the shared memory object. We need to do these lines to close the
    // mapping, close the file descriptor and remove the shared memory object.
    munmap(sharedStudent, sizeof(Student)); // Unmap the shared memory
    close(sharedMemoryFiledes);                           // Close the file descriptor
    return (0);
}
