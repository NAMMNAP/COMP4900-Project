// AUV_server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include "comm_module.h"


/*-----------------------------------------------------
 * Handler function forward calls
 * These functions handle the logic for new incoming
 * sensor data.
 *----------------------------------------------------*/
void handle_sensor_1(const SensorDataType1 *data);
void handle_sensor_2(const SensorDataType2 *data);
/* ... You could add more: handle_sensor_3, etc. */

typedef void (*SensorHandler)(const void *data);

typedef struct {
    name_attach_t   *attach;            // QNX name-attach handle
    int             sensor_id;          // used for logging or identification
    SensorType      sensor_type;        // sensor type
    SensorHandler   handler;            // function pointer to process the data
} ChannelContextThread;

typedef union AUV_server_data {
    SensorDataType1 sensor_data_1;       // Sensor data type 1
    SensorDataType2 sensor_data_2;       // Sensor data type 2
    // Add more sensor data types as needed
} AUV_server_data;


/*-----------------------------------------------------
 * Thread function for each channel
 *----------------------------------------------------*/
static void* channel_thread(void *arg)
{
    ChannelContextThread *ctx = (ChannelContextThread*)arg;
    name_attach_t *attach = ctx->attach;

    printf("[Sensor %d] Listening on channel %d...\n",
            ctx->sensor_id, attach->chid);

    while (1)
    {
        char msg_buffer[256];  // needs to be big enough for the sensor msg
        int rcvid = MsgReceive(attach->chid, msg_buffer, sizeof(msg_buffer), NULL);

        if (rcvid == -1) {
            perror("MsgReceive");
            continue;
        }

        if (rcvid == 0) {
            // A pulse – we’ll ignore in this example
            continue;
        }

        AUVResponseThread response;
        memset(&response, 0, sizeof(response));

        /* Check the message type and process accordingly. */
        SensorMessageHeader header;
        memcpy(&header, msg_buffer, sizeof(header));

        if (header.sensor_type != ctx->sensor_type) {
            fprintf(stderr, "[Sensor %d] Received mismatched sensor type: %d. Expected: %d\n",
                    ctx->sensor_id, header.sensor_type, ctx->sensor_type);
            response.status = -1;
            response.confidence = 0.0f;
            if (MsgReply(rcvid, EOK, &response, sizeof(response)) == -1) {
                perror("MsgReply");
            }
            continue;
        }

        AUV_server_data data;
        /* Copy the data from the message buffer to the appropriate structure. */
        /* Assuming the message is structured as: header + data */
        memcpy(&data, msg_buffer + sizeof(header), sizeof(data));
        /* Call the sensor handler (if defined) to process the data. */
        if (ctx->handler) {
            ctx->handler(&data);
            response.status = 0;
        } else {
            fprintf(stderr, "[Sensor %d] No handler defined.\n", ctx->sensor_id);
            response.status = -1;
        }

        /* Reply to unblock the client. */
        if (MsgReply(rcvid, EOK, &response, sizeof(response)) == -1) {
            perror("MsgReply");
        }
    }

    /* If we ever break from the loop, detach name. Not shown here. */
    name_detach(attach, 0);
    free(ctx);
    return NULL;
}


/*-----------------------------------------------------
 * Main program: create multiple channels/threads
 *----------------------------------------------------*/
int main(void)
{
    // Define the list of sensor contexts with sensor type and handler
    ChannelContextThread sensor_contexts[] = {
        {
            .attach             = NULL,
            .sensor_id          = 1,
            .sensor_type        = SENSOR_TYPE_1,
            .handler            = (SensorHandler) handle_sensor_1
        },
        {
            .attach             = NULL,
            .sensor_id          = 2,
            .sensor_type        = SENSOR_TYPE_2,
            .handler            = (SensorHandler) handle_sensor_2
        }
    };


    // Total number of sensors
    int num_sensors = sizeof(sensor_contexts) / sizeof(sensor_contexts[0]);

    // Each sensor has its own thread
    pthread_t threads[num_sensors];

    /* For each sensor, attach a name and spawn a thread. */
    for (int i = 0; i < num_sensors; i++) {
        char server_name[64];
        snprintf(server_name, sizeof(server_name), "SensorServer_%d", sensor_contexts[i].sensor_id);

        name_attach_t *attach = name_attach(NULL, server_name, 0);
        if (attach == NULL) {
            perror("name_attach");
            return EXIT_FAILURE;
        }

        // Assign the attach pointer to the context
        sensor_contexts[i].attach = attach;

        /* Launch the channel's thread */
        if (pthread_create(&threads[i], NULL, channel_thread, (void *)&sensor_contexts[i]) != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

/*-----------------------------------------------------
 * handler implementation
 *----------------------------------------------------*/
void handle_sensor_1(const SensorDataType1 *data)
{
    /* In real code, process data->depth, data->temperature, etc. */
    printf("[handle_sensor_1] Depth=%.2f, Temp=%.2f, Pressure=%.2f\n",
           data->depth, data->temperature, data->pressure);
}

void handle_sensor_2(const SensorDataType2 *data)
{
    /* In real code, process data->latitude, data->longitude, etc. */
    printf("[handle_sensor_2] Lat=%.4f, Long=%.4f, Alt=%.2f\n",
           data->latitude, data->longitude, data->altitude);
}
