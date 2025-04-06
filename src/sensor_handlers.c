#include <stdio.h>
#include "sensor_handlers.h"


AuvState *global_state;

void ins_algorithm()
{
	// TODO: create an algorithm that updates global_state.x, global_state.y, global_state.z
}

void handle_sensor_1(const SensorDataType1 *data)
{
	global_state->depth       = data->depth;
	global_state->temperature = data->temperature;
	global_state->pressure    = data->pressure;

	printf("[handle_sensor_1] Depth=%.2f, Temp=%.2f, Pressure=%.2f\n",
		   global_state->depth, global_state->temperature, global_state->pressure);
}

void handle_sensor_2(const SensorDataType2 *data)
{
	global_state->velocity[0] = data->x_vel;
	global_state->velocity[1] = data->y_vel;
	global_state->velocity[2] = data->z_vel;

	printf("[handle_sensor_2] X-Velocity=%.2f, Y-Velocity=%.2f, Z-Velocity=%.2f\n",
		   global_state->velocity[0], global_state->velocity[1], global_state->velocity[2]);

	// Calling algorithm
	ins_algorithm();
}

void handle_sensor_3(const SensorDataType3 *data)
{
	global_state->accel[0] = data->x_accel;
	global_state->accel[1] = data->y_accel;
	global_state->accel[2] = data->z_accel;

	printf("[handle_sensor_3] X-Acceleration=%.2f, Y-Acceleration=%.2f, Z-Acceleration=%.2f\n",
		   global_state->accel[0], global_state->accel[1], global_state->accel[2]);

	// Calling algorithm
	ins_algorithm();
}

void handle_sensor_4(const SensorDataType4 *data)
{
	global_state->angle[0] = data->x_angle;
	global_state->angle[1] = data->y_angle;
	global_state->angle[2] = data->z_angle;

	printf("[handle_sensor_3] X-Angle=%.2f, Y-Angle=%.2f, Z-Angle=%.2f\n",
		   global_state->angle[0], global_state->angle[1], global_state->angle[2]);

	// Calling algorithm
	ins_algorithm();
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

    sensor_contexts[2].attach      = NULL;
	sensor_contexts[2].sensor_id   = 3;
	sensor_contexts[2].sensor_type = SENSOR_TYPE_3;
	sensor_contexts[2].handler     = (SensorHandler) handle_sensor_3;

	sensor_contexts[3].attach      = NULL;
	sensor_contexts[3].sensor_id   = 4;
	sensor_contexts[3].sensor_type = SENSOR_TYPE_4;
	sensor_contexts[3].handler     = (SensorHandler) handle_sensor_4;

    return sensor_contexts;
}
