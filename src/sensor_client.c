#include "comm_module.h"
#include "AUV_server.h"

int main(void) {
    /* 1. Spin up the server in background threads. */
    if (auv_server_start() != 0) {
        fprintf(stderr, "[Client] Failed to start AUV server!\n");
        return EXIT_FAILURE;
    }

    /* 2. Open a connection to "SensorServer_1" (the channel for sensor type 1). */
    int coid = name_open("SensorServer_1", 0);
    if (coid == -1) {
        perror("name_open");
        return EXIT_FAILURE;
    }

    /* 3. Prepare the message which includes a header and sensor data. */
    char msg_buffer[sizeof(SensorMessageHeader) + sizeof(SensorDataType1)];
    memset(msg_buffer, 0, sizeof(msg_buffer));

    /* Set up the header. */
    SensorMessageHeader header;
    header.sensor_type = SENSOR_TYPE_1;
    header.payload_size  = sizeof(SensorDataType1);
    memcpy(msg_buffer, &header, sizeof(header));

    /* Set up the sensor data for SENSOR_TYPE_1. */
    SensorDataType1 sensorMsg = {0};
    sensorMsg.depth       = 100.5;          /* Example depth */
    sensorMsg.temperature = 20.0;           /* Example temperature */
    sensorMsg.pressure    = 1013.25;        /* Example pressure */
    memcpy(msg_buffer + sizeof(header), &sensorMsg, sizeof(sensorMsg));

    /* 4. Prepare a place to receive the server response. */
    AUVResponseThread auvResp;
    memset(&auvResp, 0, sizeof(auvResp));

    /* 5. Send the message to the server and wait for a reply (blocking call). */
    if (MsgSend(coid, msg_buffer, sizeof(msg_buffer), &auvResp, sizeof(auvResp)) == -1) {
        perror("MsgSend");
        name_close(coid);
        return EXIT_FAILURE;
    }

    /* 6. Print out the server’s response. */
    printf("[Client] Received AUV response: status = %d\n",
           auvResp.status);

    /* 7. Close the connection. */
    name_close(coid);
    return EXIT_SUCCESS;
}
