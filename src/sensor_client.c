#include <time.h>
#include "comm_module.h"
#include "AUV_server.h"

int send_msg(int coid, int iSensorType, void* pSensorData, size_t uSensorDataSize);
void sensor_wrapper_one();
void sensor_wrapper_two();
void sensor_wrapper_three();
void sensor_wrapper_four();
double add_noise(double min, double max);

int main(void) {
	// Seeding random
	srand(time(NULL));

    /* 1. Spin up the server in background threads. */
    if (auv_server_start() != 0) {
        fprintf(stderr, "[Client] Failed to start AUV server!\n");
        return EXIT_FAILURE;
    }

    // Set up client-side threads for sending messages per each sensor
    // TODO: Can refactor this in some for loop.
	pthread_t sensor_client_1;
	pthread_t sensor_client_2;
	pthread_t sensor_client_3;
	pthread_t sensor_client_4;
	if (pthread_create(&sensor_client_1, NULL, (void*)&sensor_wrapper_one, NULL) != 0) {
		perror("pthread_create");
		return -1;
	}
	if (pthread_create(&sensor_client_2, NULL, (void*)&sensor_wrapper_two, NULL) != 0) {
		perror("pthread_create");
		return -1;
	}
	if (pthread_create(&sensor_client_3, NULL, (void*)&sensor_wrapper_three, NULL) != 0) {
		perror("pthread_create");
		return -1;
	}
	if (pthread_create(&sensor_client_4, NULL, (void*)&sensor_wrapper_four, NULL) != 0) {
		perror("pthread_create");
		return -1;
	}

    while(1){

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

void sensor_wrapper_one() {
    /* 2. Open a connection to "SensorServer_X" (the channel for sensor type X). */
	int coid = name_open("SensorServer_1", 0);
	if (coid == -1) {
		perror("name_open");
		return;
	}

	SensorDataType1 sensorMsg = { 0 };
	while (1) {
		sensorMsg.depth = add_noise(100, 101);
		sensorMsg.temperature = add_noise(19.5, 20);
		sensorMsg.pressure += add_noise(19.8, 20);
		if (send_msg(coid, SENSOR_TYPE_1, (void*)(&sensorMsg), sizeof(SensorDataType1)) != EXIT_SUCCESS){
			perror("[sensor_wrapper_one] Could not send message.");
		}
		sleep(SENSOR_FREQUNCY);
	}
}
void sensor_wrapper_two() {
	int coid = name_open("SensorServer_2", 0);
	if (coid == -1) {
		perror("name_open");
		return;
	}

	SensorDataType2 sensorMsg = { 0 };
	while (1) {
		sensorMsg.x_vel += add_noise(0, 1);
		sensorMsg.y_vel += add_noise(-1, 1);
		sensorMsg.z_vel += add_noise(-0.1, 0.1);
		if (send_msg(coid, SENSOR_TYPE_2, (void*)(&sensorMsg), sizeof(SensorDataType2)) != EXIT_SUCCESS){
			perror("[sensor_wrapper_two] Could not send message.");
		}
		sleep(SENSOR_FREQUNCY);
	}
}
void sensor_wrapper_three() {
	int coid = name_open("SensorServer_3", 0);
	if (coid == -1) {
		perror("name_open");
		return;
	}

	SensorDataType3 sensorMsg = { 0 };
	while (1) {
		sensorMsg.x_accel += add_noise(-0.1, 0.1);
		sensorMsg.y_accel += add_noise(-0.1, 0.1);
		sensorMsg.z_accel += add_noise(-0.01, 0.01);
		if (send_msg(coid, SENSOR_TYPE_3, (void*)(&sensorMsg), sizeof(SensorDataType3)) != EXIT_SUCCESS){
			perror("[sensor_wrapper_three] Could not send message.");
		}
		sleep(SENSOR_FREQUNCY);
	}
}
void sensor_wrapper_four() {
	int coid = name_open("SensorServer_4", 0);
	if (coid == -1) {
		perror("name_open");
		return;
	}

	SensorDataType4 sensorMsg = { 0 };
	while (1) {
		sensorMsg.x_angle += add_noise(-0.1, 0.1);
		sensorMsg.y_angle += add_noise(-0.1, 0.1);
		sensorMsg.z_angle += add_noise(-0.1, 0.1);
		if (send_msg(coid, SENSOR_TYPE_4, (void*)(&sensorMsg), sizeof(SensorDataType4)) != EXIT_SUCCESS){
			perror("[sensor_wrapper_four] Could not send message.");
		}
		sleep(SENSOR_FREQUNCY);
	}
}

double add_noise(double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}
