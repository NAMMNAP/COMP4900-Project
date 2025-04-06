#ifndef AUV_STATE_H
#define AUV_STATE_H

#include "comm_module.h"

typedef struct {
    float depth;
    float temperature;
    float pressure;

    // Vector measurements
    // x, y, z axis
    float velocity[3];
    float accel[3];
    float angle[3]; // Yaw, pitch, roll

    float x;
    float y;
    float z;

    // Mutex to synchronize access if multiple threads modify this.
    pthread_mutex_t mutex;
} AuvState;

// Declare the global state instance.
extern AuvState *global_state;

#endif // AUV_STATE_H
