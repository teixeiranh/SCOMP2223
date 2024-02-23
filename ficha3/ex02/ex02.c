// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/mman.h>
// #include <sys/wait.h>
// #include <fcntl.h>
// #include <sys/stat.h>

// #define SHM_NAME "/myshm"
// #define SHM_SIZE sizeof(int)*2
// #define OPERATIONS 1000000

// int main()
// {
//     int* ptr;
//     int val1 = 10000, val2 = 500;

//     // Create shared memory object
//     /*
//     SHM_NAME: This is the name of the shared memory object. It is a string that specifies the name 
//     of the object, and is used to identify it among the other shared memory objects in the system.

//     O_CREAT|O_EXCL|O_RDWR: These are flags that specify the mode in which the shared memory object 
//     is created. O_CREAT indicates that the object should be created if it doesn't already exist, 
//     O_EXCL indicates that the object should be created exclusively, and O_RDWR indicates that 
//     the object should be opened for both reading and writing.

//     S_IRUSR|S_IWUSR: These are file permissions that specify the read and write permissions for the 
//     owner of the shared memory object. S_IRUSR indicates read permission for the owner, and S_IWUSR 
//     indicates write permission for the owner.
//     */
//     int fd = shm_open(SHM_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
//     if (fd == -1) {
//         perror("Error creating shared memory object");
//         exit(EXIT_FAILURE);
//     }

//     // Set the size of the shared memory object
//     if (ftruncate(fd, SHM_SIZE) == -1) {
//         perror("Error setting the size of the shared memory object");
//         exit(EXIT_FAILURE);
//     }

//     // Map shared memory object to process address space
//     /*
//     NULL: The starting address for the mapping is set to NULL, which allows the kernel to choose any 
//     suitable address.

//     SHM_SIZE: The length of the mapping is specified by SHM_SIZE, which is the size of the shared 
//     memory region.

//     PROT_READ|PROT_WRITE: This argument specifies the protection flags for the mapped memory. 
//     PROT_READ allows the memory to be read, and PROT_WRITE allows the memory to be written.

//     MAP_SHARED: This flag specifies that the mapping should be shared between multiple processes that 
//     have the file descriptor fd open. This means that changes to the mapped memory made by one process 
//     will be visible to other processes that have the same memory mapped.

//     fd: This is the file descriptor of the shared memory region that was created using shm_open.

//     0: This specifies the offset within the shared memory region that the mapping should start at. 
//     In this case, we start at the beginning of the shared memory region.
//     */
//     ptr = mmap(NULL, SHM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
//     if (ptr == MAP_FAILED) {
//         perror("Error mapping shared memory object to process address space");
//         exit(EXIT_FAILURE);
//     }

//     // Initialize shared memory values
//     ptr[0] = val1;
//     ptr[1] = val2;

//     // Create child process
//     pid_t pid = fork();

//     if (pid == -1) {
//         perror("Error creating child process");
//         exit(EXIT_FAILURE);
//     }
//     else if (pid == 0) { // Child process
//         for (int i = 0; i < OPERATIONS; i++) {
//             ptr[0]++;
//             ptr[1]--;
//         }
//         exit(EXIT_SUCCESS);
//     }
//     else { // Parent process
//         for (int i = 0; i < OPERATIONS; i++) {
//             ptr[0]--;
//             ptr[1]++;
//         }

//         // Wait for child process to finish
//         wait(NULL);

//         // Print final values
//         printf("Final value 1: %d\n", ptr[0]);
//         printf("Final value 2: %d\n", ptr[1]);

//         // Unmap shared memory object from process address space
//         if (munmap(ptr, SHM_SIZE) == -1) {
//             perror("Error unmapping shared memory object from process address space");
//             exit(EXIT_FAILURE);
//         }

//         // Close shared memory object
//         if (close(fd) == -1) {
//             perror("Error closing shared memory object");
//             exit(EXIT_FAILURE);
//         }

//         // Unlink shared memory object
//         if (shm_unlink(SHM_NAME) == -1) {
//             perror("Error unlinking shared memory object");
//             exit(EXIT_FAILURE);
//         }
//     }

//     return 0;
// }

/*
Since there is a race condition between the parent and child processes accessing the shared memory values, 
the results will vary in each run. To ensure data consistency we can use synchronization mechanisms such 
as semaphores to coordinate access to the shared memory between the parent and child processes.
Example below.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SHM_NAME "/myshm"
#define SHM_SIZE sizeof(int)*2
#define SEM_NAME "/mysem"
#define OPERATIONS 1000000

int main()
{
    int* ptr;
    sem_t* sem_ptr;
    int val1 = 10000, val2 = 500;

    // Create shared memory object
    int fd = shm_open(SHM_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if (fd == -1) {
        perror("Error creating shared memory object");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory object
    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("Error setting the size of the shared memory object");
        exit(EXIT_FAILURE);
    }

    // Map shared memory object to process address space
    ptr = mmap(NULL, SHM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Error mapping shared memory object to process address space");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory values
    ptr[0] = val1;
    ptr[1] = val2;

    // Create semaphore
    sem_ptr = sem_open(SEM_NAME, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 1);
    if (sem_ptr == SEM_FAILED) {
        perror("Error creating semaphore");
        exit(EXIT_FAILURE);
    }

    // Create child process
    pid_t pid = fork();

    if (pid == -1) {
        perror("Error creating child process");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) { // Child process
        for (int i = 0; i < OPERATIONS; i++) {
            sem_wait(sem_ptr);
            ptr[0]++;
            ptr[1]--;
            sem_post(sem_ptr);
        }
        exit(EXIT_SUCCESS);
    }
    else { // Parent process
        for (int i = 0; i < OPERATIONS; i++) {
            sem_wait(sem_ptr);
            ptr[0]--;
            ptr[1]++;
            sem_post(sem_ptr);
        }

        // Wait for child process to finish
        wait(NULL);

        // Print final values
        printf("Final value 1: %d\n", ptr[0]);
        printf("Final value 2: %d\n", ptr[1]);

        // Unmap shared memory object from process address space
        if (munmap(ptr, SHM_SIZE) == -1) {
            perror("Error unmapping shared memory object from process address space");
            exit(EXIT_FAILURE);
        }

        // Close shared memory object
        if (close(fd) == -1) {
            perror("Error closing shared memory object");
            exit(EXIT_FAILURE);
        }

        // Unlink shared memory object
        if (shm_unlink(SHM_NAME) == -1) {
            perror("Error unlinking shared memory object");
            exit(EXIT_FAILURE);
        }

        // Close semaphore
        if (sem_close(sem_ptr) == -1) {
            perror("Error closing semaphore");
            exit(EXIT_FAILURE);
        }

        // Remove semaphore
        if (sem_unlink("/mysem") == -1) {
            perror("Error unlinking semaphore");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
