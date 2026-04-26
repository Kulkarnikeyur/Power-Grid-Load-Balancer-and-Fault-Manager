#ifndef GRID_STATE_H
#define GRID_STATE_H

#include <semaphore.h>

// Initialize grid
void init_grid(int total_capacity);

// Request and release capacity
int request_capacity(int amount);
void release_capacity(int amount);
int modify_capacity(int new_total_capacity);

// Get current usage
int get_used_capacity();
int get_total_capacity();

#endif
