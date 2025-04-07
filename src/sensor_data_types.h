#ifndef SENSOR_DATA_TYPES_H
#define SENSOR_DATA_TYPES_H

/*-----------------------------------------------------
 * Sensor Data Types Definitions
 *-----------------------------------------------------*/

// the purpose of this enum is so that the server can know the data type
typedef enum {
    SENSOR_TYPE_1,
    SENSOR_TYPE_2,
	SENSOR_TYPE_3,
	SENSOR_TYPE_4,
    // Add more sensor types as needed
} SensorType;

// example sensor data structures
typedef struct {
    float depth;
    float temperature;
    float pressure;
} SensorDataType1;

typedef struct {
	double x_vel;
    double y_vel;
    double z_vel;
} SensorDataType2;

typedef struct {
	double x_accel;
    double y_accel;
    double z_accel;
} SensorDataType3;

typedef struct {
	double x_angle;
	double y_angle;
	double z_angle;
} SensorDataType4;


#endif // SENSOR_DATA_TYPES_H
