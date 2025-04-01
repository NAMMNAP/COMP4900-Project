#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include "comm_module.h"

int main(void) {
    /* 1. Open a connection to "SensorServer_1" (the channel for sensor type 1). */
    int coid = name_open("SensorServer_1", 0);
    if (coid == -1) {
        perror("name_open");
        return EXIT_FAILURE;
    }

    /* 2. Prepare the message which includes a header and sensor data. */
    char msg_buffer[sizeof(SensorMessageHeader) + sizeof(SensorDataType1)];
    memset(msg_buffer, 0, sizeof(msg_buffer));

    /* Set up the header. */
    SensorMessageHeader header;
    header.sensor_type = SENSOR_TYPE_1;  // Specify the sensor type.
    memcpy(msg_buffer, &header, sizeof(header));

    /* Set up the sensor data for SENSOR_TYPE_1. */
    SensorDataType1 sensorMsg;
    memset(&sensorMsg, 0, sizeof(sensorMsg));
    sensorMsg.type        = SENSOR_TYPE_1;  // Optional: include type in sensor data.
    sensorMsg.depth       = 100.5;          /* Example depth */
    sensorMsg.temperature = 20.0;           /* Example temperature */
    sensorMsg.pressure    = 1013.25;        /* Example pressure */
    memcpy(msg_buffer + sizeof(header), &sensorMsg, sizeof(sensorMsg));

    /* 3. Prepare a place to receive the server response. */
    AUVResponseThread auvResp;
    memset(&auvResp, 0, sizeof(auvResp));

    /* 4. Send the message to the server and wait for a reply (blocking call). */
    if (MsgSend(coid, &sensorMsg, sizeof(sensorMsg),
                &auvResp, sizeof(auvResp)) == -1) {
        perror("MsgSend");
        name_close(coid);
        return EXIT_FAILURE;
    }

    /* 5. Print out the server’s response. */
    printf("[Client] Received AUV response: status = %d, confidence = %.2f\n",
           auvResp.status, auvResp.confidence);

    /* 6. Close the connection. */
    name_close(coid);
    return EXIT_SUCCESS;
}
