#include <stdio.h>
#include "sensor_handlers.h"

// Note: You can either pass the global state as an argument or use an external/global instance.
extern AuvState *global_state;

void handle_sensor_1(const SensorDataType1 *data, AuvState *global_state)
{
    global_state->depth       = data->depth;
    global_state->temperature = data->temperature;
    global_state->pressure    = data->pressure;

    printf("[handle_sensor_1] Depth=%.2f, Temp=%.2f, Pressure=%.2f\n",
           global_state->depth, global_state->temperature, global_state->pressure);
}

void handle_sensor_2(const SensorDataType2 *data, AuvState *global_state)
{
    global_state->latitude  = data->latitude;
    global_state->longitude = data->longitude;
    global_state->altitude  = data->altitude;

    printf("[handle_sensor_2] Lat=%.4f, Long=%.4f, Alt=%.2f\n",
           global_state->latitude, global_state->longitude, global_state->altitude);
}

ChannelContextThread *get_sensor_contexts(int num_sensors)
{
    ChannelContextThread *sensor_contexts = malloc(sizeof(ChannelContextThread) * num_sensors);
    if (sensor_contexts == NULL) {
        perror("malloc");
        return NULL;
    }

    // Note: if you’re passing the global state to handlers, you may need to ensure that is set.
    sensor_contexts[0].attach      = NULL;
    sensor_contexts[0].sensor_id   = 1;
    sensor_contexts[0].sensor_type = SENSOR_TYPE_1;
    sensor_contexts[0].handler     = (SensorHandler) handle_sensor_1;

    sensor_contexts[1].attach      = NULL;
    sensor_contexts[1].sensor_id   = 2;
    sensor_contexts[1].sensor_type = SENSOR_TYPE_2;
    sensor_contexts[1].handler     = (SensorHandler) handle_sensor_2;

    return sensor_contexts;
}
