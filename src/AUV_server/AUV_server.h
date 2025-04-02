#ifndef AUV_SERVER_H
#define AUV_SERVER_H

#define NUM_SENSORS 2

#include "comm_module.h"
#include "sensor_handlers.h"
#include "sensor_map.h"


int auv_server_start(void);

int init_mutex(void);
void print_channel_startup(int sensor_id, int channel_id);

int handler_wrapper(ChannelContextThread *ctx, const void *data, size_t data_size);

#endif // AUV_SERVER_H
