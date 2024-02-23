#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_PATH_LENGTH 512
#define MAX_WORD_LENGTH 64
#define NUM_CHILDREN 10

#define SHM_NAME "/myshm"
#define SHM_SIZE sizeof(search_info)*NUM_CHILDREN

typedef struct {
    char path[MAX_PATH_LENGTH];
    char word[MAX_WORD_LENGTH];
    int count;
} search_info;

int count_word_occurrences(const char *path, const char *word) {
    int occurrences = 0;

    // Getting directory 
    char cwd[512];
    getcwd(cwd, sizeof(cwd));

    // Opening the file
    FILE *file = fopen(strcat(cwd, path), "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    // Go over the lines
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char *ptr = line;
        // Searching for the word in the line, enters while loop if there is a match
        while ((ptr = strstr(ptr, word)) != NULL) {
            // Incrementing occurrences counter
            occurrences++;
            // Moving pointer to the end of matched word
            ptr += strlen(word);
        }
    }

    // Close the file
    fclose(file);

    return occurrences;
}

int main() {
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

    // Create shared memory area for local maximums
    search_info *info = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (info == MAP_FAILED) {
        perror("Error creating shared memory area");
        exit(EXIT_FAILURE);
    }

    const char *paths[NUM_CHILDREN] = {"/hello.txt", "/world.txt", "/we.txt", "/are.txt", "/vitor.txt", 
                                        "/nuno.txt", "/joao.txt", "/matos.txt", "/and.txt", "/maia.txt"};
    const char *words[NUM_CHILDREN] = {"hello", "world", "we", "are", "vitor", "nuno", "joao", "matos", "and", "maia"};
    for (int i = 0; i < NUM_CHILDREN; i++) {
        strcpy(info[i].path, paths[i]);
        strcpy(info[i].word, words[i]);
        info[i].count = 0;
    }

    // Create 10 new processes
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("Failed to create child process");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            info[i].count = count_word_occurrences(info[i].path, info[i].word);
            exit(EXIT_SUCCESS);
        }
    }

    // Wait until the child processes finish their search
    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    // Print the number of occurrences determined by each child
    for (int i = 0; i < NUM_CHILDREN; i++) {
        printf("Child %d found %d occurrences of \"%s\" in \"%s\"\n", i, info[i].count, info[i].word, info[i].path);
    }

    // Cleanup shared memory area
    if (munmap(info, SHM_SIZE) == -1) {
        perror("Error unmapping shared memory area");
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

    return 0;
}

/*
Child 0 found 2 occurrences of "hello" in "/hello.txt"
Child 1 found 1 occurrences of "world" in "/world.txt"
Child 2 found 4 occurrences of "we" in "/we.txt"
Child 3 found 2 occurrences of "are" in "/are.txt"
Child 4 found 2 occurrences of "vitor" in "/vitor.txt"
Child 5 found 3 occurrences of "nuno" in "/nuno.txt"
Child 6 found 3 occurrences of "joao" in "/joao.txt"
Child 7 found 2 occurrences of "matos" in "/matos.txt"
Child 8 found 4 occurrences of "and" in "/and.txt"
Child 9 found 1 occurrences of "maia" in "/maia.txt"
*/