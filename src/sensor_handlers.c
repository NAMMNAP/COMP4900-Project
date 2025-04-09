#include <stdio.h>
#include <math.h>
#include "sensor_handlers.h"


AuvState *global_state;

void three_by_three_matrix_mult(double mul[][3], double a[][3], double b[][3]);

void ins_caller()
{
	while (1) {
		sleep(INS_FREQUENCY);
		printf("[INS] Calling INS algorithm...\n");

		pthread_mutex_lock(&global_state->mutex);
		ins_algorithm(INS_FREQUENCY);
		pthread_mutex_unlock(&global_state->mutex);
	}
}
void ins_algorithm(double delta_t)
{
	// Vector/matrix variables
	double rotation_matrix[3][3] = { 0 };
	double vel_world[3] = { 0 };
	double vel_body[3]  = { 0 };

	// Step 1: Creating rotation matrix
	double psi   = global_state->angle[2];	// Yaw
	double theta = global_state->angle[1];	// Pitch
	double phi   = global_state->angle[0];	// Roll
	double rot_z[3][3] = { {cos(psi), -sin(psi), 0}, {sin(psi), cos(psi), 0}, {0, 0, 1} };
	double rot_y[3][3] = { {cos(theta), 0, sin(theta)}, {0, 1, 0}, {-sin(theta), 0, cos(theta)} };
	double rot_x[3][3] = { {1, 0, 0}, {0, cos(phi), -sin(phi)}, {0, sin(phi), cos(phi)} };

	double z_y_mult[3][3] = { 0 };
	three_by_three_matrix_mult(z_y_mult, rot_z, rot_y);
	three_by_three_matrix_mult(rotation_matrix, z_y_mult, rot_x);

	// Step 2: Calculate body velocities
	vel_body[0] = global_state->velocity[0] + global_state->accel[0] * delta_t;
	vel_body[1] = global_state->velocity[1] + global_state->accel[1] * delta_t;
	vel_body[2] = global_state->velocity[2] + global_state->accel[2] * delta_t;

	// Step 3: Calculate world velocities
	vel_world[0] = vel_body[0] * rotation_matrix[0][0] + vel_body[1] * rotation_matrix[0][1] +
			       vel_body[2] * rotation_matrix[0][2];
	vel_world[1] = vel_body[0] * rotation_matrix[1][0] + vel_body[1] * rotation_matrix[1][1] +
			       vel_body[2] * rotation_matrix[1][2];
	vel_world[2] = vel_body[0] * rotation_matrix[2][0] + vel_body[1] * rotation_matrix[2][1] +
			       vel_body[2] * rotation_matrix[2][2];

	// Step 4: Update positions
	global_state->position[0] = global_state->position[0] + vel_world[0] * delta_t;
	global_state->position[1] = global_state->position[1] + vel_world[1] * delta_t;

	// Step 5: Convert to GPS and adjust drift
	global_state->position[2] = global_state->depth;
	global_state->latitude = (global_state->position[0] / 1000) / 111.32; // 1 degree of lattiude = 111.32 km
	global_state->longitude = 40075 * (cos(global_state->latitude) / 360);

	printf("[INS] Updated positions: Latitude=%lf, Longitude=%lf, Depth=%.2f m\n",
			global_state->latitude, global_state->longitude, global_state->position[2]);
}

void handle_sensor_1(const SensorDataType1 *data)
{
	global_state->depth       = data->depth;
	global_state->temperature = data->temperature;
	global_state->pressure    = data->pressure;

	printf("[handle_sensor_1] Depth=%.2f m, Temp=%.2f C, Pressure=%.2f bar\n",
		   global_state->depth, global_state->temperature, global_state->pressure);
}

void handle_sensor_2(const SensorDataType2 *data)
{
	global_state->velocity[0] = data->x_vel;
	global_state->velocity[1] = data->y_vel;
	global_state->velocity[2] = data->z_vel;

	printf("[handle_sensor_2] X-Velocity=%.2f m/s, Y-Velocity=%.2f m/s, Z-Velocity=%.2f m/s\n",
		   global_state->velocity[0], global_state->velocity[1], global_state->velocity[2]);
}

void handle_sensor_3(const SensorDataType3 *data)
{
	global_state->accel[0] = data->x_accel;
	global_state->accel[1] = data->y_accel;
	global_state->accel[2] = data->z_accel;

	printf("[handle_sensor_3] X-Acceleration=%.2f m/s^2, Y-Acceleration=%.2f m/s^2, Z-Acceleration=%.2f m/s^2\n",
		   global_state->accel[0], global_state->accel[1], global_state->accel[2]);
}

void handle_sensor_4(const SensorDataType4 *data)
{
	global_state->angle[0] = data->x_angle;
	global_state->angle[1] = data->y_angle;
	global_state->angle[2] = data->z_angle;

	// Convert to degrees for printing
	double x_angle = global_state->angle[0] * (180.0 / M_PI);
	double y_angle = global_state->angle[1] * (180.0 / M_PI);
	double z_angle = global_state->angle[2] * (180.0 / M_PI);

	printf("[handle_sensor_4] X-Angle=%.2f degrees, Y-Angle=%.2f degrees, Z-Angle=%.2f degrees\n",
			x_angle, y_angle, z_angle);
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

// Helper function
void three_by_three_matrix_mult(double mul[][3], double a[][3], double b[][3]) {
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			for(int k = 0; k < 3; k++) {
				mul[i][j] = mul[i][j] + a[i][k] * b[k][j];
			}
		}
	}
}
