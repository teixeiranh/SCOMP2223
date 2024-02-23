#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

// Structure for student
typedef struct
{
    int number;
    char name[100];
    char grade[5];
} student;

// Code to be executed by each thread
void *print_student(void *arg)
{
    student *s = (student *)arg;
    printf("Student number: %d\n", s->number);
    printf("Student name: %s\n", s->name);
    printf("Student number: %s\n", s->grade);
    return NULL;
}

/*
    Solution to ex02 of PL5
*/
int main(int args, char *argv[])
{
    // Array of students - serving as argument for each of the thread.
    student students[5] = {
        {1, "Joao", "A"},
        {2, "Vitor", "B"},
        {3, "Maia", "C"},
        {4, "Matos", "D"},
        {5, "Nuno", "D"},
    };

    pthread_t threads[5]; // Array of threads

    for (int i = 0; i < 5; i++) // Create and start the threads
    {
        if (pthread_create(&threads[i], NULL, print_student, &students[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    for (int i = 0; i < 5; i++) // Wait for the threads to finish
    {
        if (pthread_join(threads[i], NULL))
        {
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }
    }

    return 0;
}