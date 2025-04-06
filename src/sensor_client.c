#include "comm_module.h"
#include "AUV_server.h"

int send_msg(int coid, int iSensorType, void* pSensorData, size_t uSensorDataSize);

int main(void) {
    /* 1. Spin up the server in background threads. */
    if (auv_server_start() != 0) {
        fprintf(stderr, "[Client] Failed to start AUV server!\n");
        return EXIT_FAILURE;
    }

    /* 2. Open a connection to "SensorServer_X" (the channel for sensor type X). */
	int coidOne   = name_open("SensorServer_1", 0);
	int coidTwo   = name_open("SensorServer_2", 0);
	int coidThree = name_open("SensorServer_3", 0);
	int coidFour  = name_open("SensorServer_4", 0);
	if (coidOne == -1) {
		perror("name_open");
		return EXIT_FAILURE;
	}
	if (coidTwo == -1) {
		perror("name_open");
		return EXIT_FAILURE;
	}
	if (coidThree == -1) {
		perror("name_open");
		return EXIT_FAILURE;
	}
	if (coidFour == -1) {
		perror("name_open");
		return EXIT_FAILURE;
	}

	// TODO: This only sends a message once, with all values set to zero.
	//       Messages need to be sent continuously, with new readings from sensors.
	//
	//		 How to define the readings? We don't have a dataset, so through some random
	//       number generation probably, though it must make sense.
    SensorDataType1 sensorMsgOne = { 0 };
    SensorDataType2 sensorMsgTwo = { 0 };
    SensorDataType3 sensorMsgThree = { 0 };
    SensorDataType3 sensorMsgFour = { 0 };
    if (send_msg(coidOne, SENSOR_TYPE_1, (void*)(&sensorMsgOne), sizeof(SensorDataType1)) != EXIT_SUCCESS){
    	return EXIT_FAILURE;
    }
    if (send_msg(coidTwo, SENSOR_TYPE_2, (void*)(&sensorMsgTwo), sizeof(SensorDataType2)) != EXIT_SUCCESS){
    	return EXIT_FAILURE;
    }
    if (send_msg(coidThree, SENSOR_TYPE_3, (void*)(&sensorMsgThree), sizeof(SensorDataType3)) != EXIT_SUCCESS){
    	return EXIT_FAILURE;
    }
    if (send_msg(coidFour, SENSOR_TYPE_4, (void*)(&sensorMsgFour), sizeof(SensorDataType4)) != EXIT_SUCCESS){
    	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int send_msg(int coid, int iSensorType, void* pSensorData, size_t uSensorDataSize) {

    /* 3. Prepare the message which includes a header and sensor data. */
    char msg_buffer[sizeof(SensorMessageHeader) + uSensorDataSize];
    memset(msg_buffer, 0, sizeof(msg_buffer));

    /* Set up the header. */
    SensorMessageHeader header;
    header.sensor_type = iSensorType;
    header.payload_size  = uSensorDataSize;
    memcpy(msg_buffer, &header, sizeof(header));

    memcpy(msg_buffer + sizeof(header), pSensorData, uSensorDataSize);

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

	return EXIT_SUCCESS;
}
