#ifndef COMM_MODULE_H
#define COMM_MODULE_H

#include <sys/types.h>

/* Example sensor data message structure. */
typedef enum {
    SENSOR_TYPE_1,
    SENSOR_TYPE_2,
    // Add more sensor types as needed
} SensorType;

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

// this to be type safe. It may look redundant, but it is not
typedef struct {
    SensorType sensor_type;  // the sensor type included in the message header
} SensorMessageHeader;

/* Example AUV response message. */
typedef struct {
    int status;       // 0 for OK, non-zero for errors, or anything you want
} AUVResponseThread;

#endif // COMM_MODULE_H
