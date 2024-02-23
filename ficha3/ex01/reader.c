#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "student.h"
#include <sys/stat.h>

#define SHM_NAME "student_shm"

/* Main function to implement the reader for ex01 of PL3. */
int main()
{
    const int DATA_SIZE = sizeof(Student);
    int sharedMemoryFiledes; // File descriptor
    Student *sharedStudent;  // Pointer to the shared memory containing the Student structure

    sharedMemoryFiledes = shm_open(SHM_NAME, O_RDONLY, S_IRUSR | S_IWUSR); // Open the shared memory object
    // O_RDONLY: Open the object for reading only

    // Map the shared memory object to the process's address space
    sharedStudent = (Student *)mmap(NULL, DATA_SIZE, PROT_READ, MAP_SHARED, sharedMemoryFiledes, 0);
    // PROT_READ: The object can be read
    // MAP_SHARED: The object is shared between processes

    // Active waiting loop: wait until the has_data flag is set to 1
    while (!sharedStudent->has_data)
    {
        usleep(100); // wait 100 microseconds to avoid busy waiting
    }

    // Print the student data on the screen
    printf("Student number: %d\n", sharedStudent->number);
    printf("Student name: %s\n", sharedStudent->name);
    printf("Student address: %s\n", sharedStudent->address);

    // Unmap the shared memory object. We need to do these lines to close the
    // mapping, close the file descriptor and remove the shared memory object.
    munmap(sharedStudent, sizeof(Student)); // Unmap the shared memory
    close(sharedMemoryFiledes);             // Close the file descriptor
    shm_unlink("student_shm");              // Remove the shared memory object
    return (0);
}
