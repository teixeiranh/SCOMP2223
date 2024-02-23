#include <stdio.h>
#include <stdlib.h>    
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <string.h>

#define STR_SIZE 50 // String size for the name
#define NR_DISC 2   // Number of disciplines

/* "Aluno" struct to represent the student data.
*  It contains the student "numero", "nome" and "disciplinas". */
struct aluno
{
    int numero;
    char nome[STR_SIZE];
    int disciplinas[NR_DISC];
};

/* Method that calculates the highest and lowest grades of a student.
*  It receives the student data as a pointer and performs what is asked. */
void first_child(struct aluno *data)
{
    int highest = data->disciplinas[0];
    int lowest = data->disciplinas[0];

    for (int i = 1; i < NR_DISC; i++)
    {
        if (data->disciplinas[i] > highest)
        {
            highest = data->disciplinas[i];
        }
        if (data->disciplinas[i] < lowest)
        {
            lowest = data->disciplinas[i];
        }
    }

    printf("First child data:\n");
    printf("Highest grade: %d\n", highest);
    printf("Lowest grade: %d\n", lowest);
}

/* Method that calculates the average grade of a student.
*  It receives the student data as a pointer and performs what is asked.*/
void second_child(struct aluno *data)
{
    int sum = 0;

    for (int i = 0; i < NR_DISC; i++)
    {
        sum += data->disciplinas[i];
    }

    float average = (float)sum / NR_DISC;

    printf("Second child:\n");
    printf("Average grade: %.2f\n", average);
}

/* Main function to solve ex07 of PL3. */
int main()
{
    // Constants to facilitate coding
    const int DATA_SIZE = sizeof(struct aluno);
    const char *sharedMemoryName = "/student_data";

    int sharedMemoryFiledes; // "Filedes" is for File Descriptor
    struct aluno *sharedMemoryData;

    // Create shared memory "segment"
    sharedMemoryFiledes = shm_open(sharedMemoryName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (sharedMemoryFiledes == -1)
    {
        perror("shm_open error!");
        exit(1);
    }

    // Truncate shared memory, resizing it to the appropriate size
    if (ftruncate(sharedMemoryFiledes, DATA_SIZE) == -1)
    {
        perror("ftruncate error!");
        exit(1);
    }

    // Map shared memory
    sharedMemoryData = (struct aluno *)mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryFiledes, 0);
    if (sharedMemoryData == MAP_FAILED)
    {
        perror("mmap error!");
        exit(1);
    }

    // Parent process gets user input, that is going to be stored in shared memory
    printf("Enter student number: ");
    scanf("%d", &sharedMemoryData->numero);
    printf("Enter student name: ");
    scanf("%s", sharedMemoryData->nome);
    for (int i = 0; i < NR_DISC; i++)
    {
        printf("Enter grade for class %d: ", i + 1);
        scanf("%d", &sharedMemoryData->disciplinas[i]);
    }

    // Create child processes
    pid_t child1, child2;

    child1 = fork();
    if (child1 == 0) // first child
    {
        first_child(sharedMemoryData);
        exit(0);
    }
    else // Parent
    {
        child2 = fork(); // second child
        if (child2 == 0)
        {
            second_child(sharedMemoryData);
            exit(0);
        }
    }

    // Wait for child processes
    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    // Unmap shared memory
    if (munmap(sharedMemoryData, DATA_SIZE) == -1)
    {
        perror("munmap error!");
        exit(1);
    }

    // Remove shared memory "segment"
    if (shm_unlink(sharedMemoryName) == -1)
    {
        perror("shm_unlink error!");
        exit(1);
    }

    return 0;
}
