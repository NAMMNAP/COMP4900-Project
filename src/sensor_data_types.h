#ifndef SENSOR_DATA_TYPES_H
#define SENSOR_DATA_TYPES_H

/*-----------------------------------------------------
 * Sensor Data Types Definitions
 *-----------------------------------------------------*/

// the purpose of this enum is so that the server can know the data type
typedef enum {
    SENSOR_TYPE_1,
    SENSOR_TYPE_2,
    // Add more sensor types as needed
} SensorType;

// example sensor data structures
typedef struct {
    double depth;
    double temperature;
    double pressure;
} SensorDataType1;

typedef struct {
    double latitude;
    double longitude;
    double altitude;
} SensorDataType2;


#endif // SENSOR_DATA_TYPES_H
