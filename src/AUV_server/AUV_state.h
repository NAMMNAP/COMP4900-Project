#ifndef AUV_STATE_H
#define AUV_STATE_H

#include "comm_module.h"

typedef struct {
    float depth;
    float temperature;
    float pressure;

    float latitude;
    float longitude;
    float altitude;

    // Mutex to synchronize access if multiple threads modify this.
    pthread_mutex_t mutex;
} AuvState;

// Declare the global state instance.
extern AuvState *global_state;

#endif // AUV_STATE_H
