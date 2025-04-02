#ifndef COMM_MODULE_H
#define COMM_MODULE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <sys/types.h>

#include "sensor_data_types.h"

typedef struct {
    SensorType sensor_type;  // the sensor type included in the message header
    size_t     payload_size;
} SensorMessageHeader;

typedef struct {
    int status;       // 0 for OK, non-zero for errors, or anything you want
} AUVResponseThread;

#endif // COMM_MODULE_H
