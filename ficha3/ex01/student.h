#ifndef STUDENT_H
#define STUDENT_H

/* Student structure containing the required information and a flag to signal when data is available. */
typedef struct
{
    int number;        // Student number
    char name[100];    // Student name
    char address[100]; // Student address
    int has_data;      // Flag to signal when data is available
} Student;

#endif
