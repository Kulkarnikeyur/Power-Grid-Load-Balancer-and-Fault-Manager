#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include "grid_state.h"
#include "file_ops.h"
#include "fault.h"

static int total_capacity = 0;
static int used_capacity = 0;
static pthread_mutex_t grid_mutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t capacity_sem;
static int grid_initialized = 0;

void init_grid(int total)
{
    if (total <= 0)
        total = 1;

    pthread_mutex_lock(&grid_mutex);
    total_capacity = total;
    used_capacity = 0;
    pthread_mutex_unlock(&grid_mutex);

    if (sem_init(&capacity_sem, 0, total_capacity) != 0)
    {
        perror("sem_init failed");
        return;
    }

    grid_initialized = 1;
    printf("Grid initialized with capacity: %d\n", total_capacity);
}
int get_available_capacity()
{
    pthread_mutex_lock(&grid_mutex);
    int available = total_capacity - used_capacity;
    pthread_mutex_unlock(&grid_mutex);
    return available;
}
int request_capacity(int amount)
{
    if (!grid_initialized || amount <= 0)
        return -1;

    pthread_mutex_lock(&grid_mutex);
    if (used_capacity + amount > total_capacity)
    {
        pthread_mutex_unlock(&grid_mutex);
        return total_capacity - used_capacity; // return available capacity
    }
    pthread_mutex_unlock(&grid_mutex);

    int i;
    for (i = 0; i < amount; i++)
    {
        if (sem_trywait(&capacity_sem) != 0)
        {
            for (int j = 0; j < i; j++)
                sem_post(&capacity_sem);
            return -1;
        }
    }

    pthread_mutex_lock(&grid_mutex);

    used_capacity += amount;

    int is_full = (used_capacity == total_capacity);

    pthread_mutex_unlock(&grid_mutex);

    if (is_full)
    {
        send_fault_message("WARNING: Grid capacity reached!");
    }
    // printf("Allocated %d units, Used: %d / %d\n", amount, used_capacity, total_capacity);

    return -2;
}

void release_capacity(int amount)
{
    if (amount <= 0 || !grid_initialized)
        return;

    pthread_mutex_lock(&grid_mutex);
    if (amount > used_capacity)
        amount = used_capacity;

    used_capacity -= amount;
    pthread_mutex_unlock(&grid_mutex);

    for (int i = 0; i < amount; i++)
        sem_post(&capacity_sem);

    // printf("Released %d units, Used: %d / %d\n", amount, used_capacity, total_capacity);
}

int modify_capacity(int new_total_capacity)
{
    if (!grid_initialized || new_total_capacity <= 0)
        return -1;

    pthread_mutex_lock(&grid_mutex);
    if (new_total_capacity < used_capacity)
    {
        pthread_mutex_unlock(&grid_mutex);
        return -1;
    }

    int delta = new_total_capacity - total_capacity;
    if (delta > 0)
    {
        for (int i = 0; i < delta; i++)
            sem_post(&capacity_sem);
    }
    else if (delta < 0)
    {
        int reduce = -delta;
        for (int i = 0; i < reduce; i++)
        {
            if (sem_trywait(&capacity_sem) != 0)
            {
                for (int j = 0; j < i; j++)
                    sem_post(&capacity_sem);
                pthread_mutex_unlock(&grid_mutex);
                return -1;
            }
        }
    }

    total_capacity = new_total_capacity;
    pthread_mutex_unlock(&grid_mutex);

    // printf("Modified grid capacity to %d, Used: %d\n",
    //        total_capacity, used_capacity);
    return 0;
}

int get_used_capacity()
{
    int result;
    pthread_mutex_lock(&grid_mutex);
    result = used_capacity;
    pthread_mutex_unlock(&grid_mutex);
    return result;
}

int get_total_capacity()
{
    int result;
    pthread_mutex_lock(&grid_mutex);
    result = total_capacity;
    pthread_mutex_unlock(&grid_mutex);
    return result;
}
