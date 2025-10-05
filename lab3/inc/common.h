
#ifndef COMMON_H
#define COMMON_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define PI 3.14159265358979323846
#define TWO_PI (2.0 * PI)
#define R 0.30            // raio frontal (D = 0.6 m)

#define PERIOD_ROBOT_NS        30000000L   // 30 ms
#define PERIOD_LIN_NS          40000000L   // 40 ms
#define PERIOD_CTRL_NS         50000000L   // 50 ms
#define PERIOD_MODEL_NS        50000000L   // 50 ms (X e Y)
#define PERIOD_REF_NS         120000000L   // 120 ms
#define PERIOD_UI_NS          500000000L   // 500 ms

#define SIM_SECONDS 20.0                  // horizonte de simulação

// Nomes das tarefas (para logs)
#define TH_ROBOT   "robot"
#define TH_LIN     "lin"
#define TH_CTRL    "ctrl"
#define TH_MX      "model_x"
#define TH_MY      "model_y"
#define TH_REF     "ref"
#define TH_UI      "ui"
#define TH_LOAD    "load"

#endif
