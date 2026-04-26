// server/file_ops.h

#ifndef FILE_OPS_H
#define FILE_OPS_H

int read_grid_state(int *used, int *total);
void write_grid_state(int used, int total);

#endif