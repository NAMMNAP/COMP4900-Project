#ifndef SENSOR_HANDLERS_H
#define SENSOR_HANDLERS_H

#include "sensor_data_types.h"
#include "AUV_state.h"

typedef void (*SensorHandler)(const void *data);

typedef struct {
    name_attach_t   *attach;            // QNX name-attach handle
    int             sensor_id;          // used for logging or identification
    SensorType      sensor_type;        // sensor type
    SensorHandler   handler;            // function pointer to process the data
} ChannelContextThread;

ChannelContextThread *get_sensor_contexts(void);

// Handler function for SENSOR_TYPE_1
void handle_sensor_1(const SensorDataType1 *data, AuvState *global_state);

// Handler function for SENSOR_TYPE_2
void handle_sensor_2(const SensorDataType2 *data, AuvState *global_state);

#endif // SENSOR_HANDLERS_H
