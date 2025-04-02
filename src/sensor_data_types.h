#ifndef SENSOR_DATA_TYPES_H
#define SENSOR_DATA_TYPES_H

/*-----------------------------------------------------
 * Sensor Data Types Definitions
 *-----------------------------------------------------*/

typedef enum {
    SENSOR_TYPE_1,
    SENSOR_TYPE_2,
    // Add more sensor types as needed
} SensorType;

// example sensor data structures
typedef struct {
    SensorType type; // Type of sensor
    double depth;
    double temperature;
    double pressure;
} SensorDataType1;

typedef struct {
    SensorType type; // Type of sensor
    double latitude;
    double longitude;
    double altitude;
} SensorDataType2;


#endif // SENSOR_DATA_TYPES_H
