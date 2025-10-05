
#ifndef UI_H
#define UI_H
#include "common.h"

void *th_ui(void *arg);   // 500 ms
void *th_load(void *arg); // opcional: carga de CPU

typedef struct {
    struct timespec t0;
    bool with_load;
} MainArgs;

#endif
