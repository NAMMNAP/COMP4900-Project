#ifndef SENSOR_MAP_H
#define SENSOR_MAP_H

#include "comm_module.h"       // For SensorType and AUVResponseThread, etc.
#include "sensor_data_types.h" // For SensorDataType1, SensorDataType2, etc.

typedef struct {
    SensorType sensor_type;
    size_t     payload_size;
} SensorTypeMap;

static const SensorTypeMap sensor_type_map[] = {
    { SENSOR_TYPE_1, sizeof(SensorDataType1) },
    { SENSOR_TYPE_2, sizeof(SensorDataType2) }
};

#define NUM_SENSOR_TYPES (sizeof(sensor_type_map) / sizeof(sensor_type_map[0]))

// Lookup function: returns payload size for a given sensor type, or 0 if not found.
static inline size_t get_sensor_payload_size(SensorType type) {
    for (size_t i = 0; i < NUM_SENSOR_TYPES; i++) {
        if (sensor_type_map[i].sensor_type == type) {
            return sensor_type_map[i].payload_size;
        }
    }
    return 0; // Not found.
}

#endif // SENSOR_MAP_H
