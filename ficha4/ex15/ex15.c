#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CAPACITY 10
#define NUM_CLIENTS 30

typedef enum
{
    VIP,
    SPECIAL,
    NORMAL
} ClientType;

typedef struct
{
    int id;
    ClientType type;
    int stay_time;
} Client;

sem_t vip_sem, special_sem, normal_sem, club_sem;

void *client_thread(void *arg)
{
    Client *client = (Client *)arg;

    switch (client->type)
    {
    case VIP:
        sem_wait(&vip_sem);
        break;
    case SPECIAL:
        sem_wait(&special_sem);
        break;
    case NORMAL:
        sem_wait(&normal_sem);
        break;
    }

    sem_wait(&club_sem);
    printf("Client %d of type %d has entered the club.\n", client->id, client->type);

    sleep(client->stay_time);

    printf("Client %d of type %d has left the club.\n", client->id, client->type);
    sem_post(&club_sem);

    switch (client->type)
    {
    case VIP:
        sem_post(&vip_sem);
        break;
    case SPECIAL:
        sem_post(&special_sem);
        break;
    case NORMAL:
        sem_post(&normal_sem);
        break;
    }

    return NULL;
}

int main()
{
    pthread_t clients[NUM_CLIENTS];
    Client client_data[NUM_CLIENTS];

    sem_init(&vip_sem, 0, 1);
    sem_init(&special_sem, 0, 1);
    sem_init(&normal_sem, 0, 1);
    sem_init(&club_sem, 0, MAX_CAPACITY);

    for (int i = 0; i < NUM_CLIENTS; i++)
    {
        client_data[i].id = i;
        client_data[i].type = rand() % 3;
        client_data[i].stay_time = rand() % 5 + 1;

        pthread_create(&clients[i], NULL, client_thread, (void *)&client_data[i]);
    }

    for (int i = 0; i < NUM_CLIENTS; i++)
    {
        pthread_join(clients[i], NULL);
    }

    sem_destroy(&vip_sem);
    sem_destroy(&special_sem);
    sem_destroy(&normal_sem);
    sem_destroy(&club_sem);

    return 0;
}
