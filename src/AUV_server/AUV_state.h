#ifndef AUV_STATE_H
#define AUV_STATE_H

#include <pthread.h>

typedef struct {
    double depth;
    double temperature;
    double pressure;

    double latitude;
    double longitude;
    double altitude;

    // Mutex to synchronize access if multiple threads modify this.
    pthread_mutex_t mutex;
} AuvState;

#endif // AUV_STATE_H
