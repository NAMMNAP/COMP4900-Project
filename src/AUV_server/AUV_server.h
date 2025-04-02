#ifndef AUV_SERVER_H
#define AUV_SERVER_H

#define NUM_SENSORS 2

#include "comm_module.h"
#include "sensor_handlers.h"
#include "sensor_map.h"


int auv_server_start(void);

#endif // AUV_SERVER_H
