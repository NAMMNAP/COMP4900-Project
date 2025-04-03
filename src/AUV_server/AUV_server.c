#include "AUV_server.h"

int init_mutex(void)
{
    // Initialize global state before creating threads.
    global_state = malloc(sizeof(AuvState));
    if (global_state == NULL) {
        perror("malloc");
        return -1;
    }

    // Initialize the mutex
    if (pthread_mutex_init(&global_state->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        free(global_state);
        return -1;
    }
    return 0;
}

void print_channel_startup(int sensor_id, int channel_id)
{
    printf("[Sensor %d] Listening on channel %d...\n", sensor_id, channel_id);
}

/*-----------------------------------------------------
 * Thread function for each channel
 *----------------------------------------------------*/

static void* channel_thread(void *arg)
{
    ChannelContextThread *ctx = (ChannelContextThread*)arg;
    print_channel_startup(ctx->sensor_id, ctx->attach->chid);

    while (1)
    {
        char msg_buffer[256];  // needs to be big enough for the sensor msg
        int rcvid = MsgReceive(ctx->attach->chid, msg_buffer, sizeof(msg_buffer), NULL);

        if (rcvid == -1) {
            perror("MsgReceive");
            continue;
        }

        if (rcvid == 0) {
            // A pulse – we’ll ignore in this example
            continue;
        }
        printf("[Sensor %d] Received message on channel %d\n", ctx->sensor_id, ctx->attach->chid);

        AUVResponseThread response;
        memset(&response, 0, sizeof(response));

        /* Check the message type and process accordingly. */
        SensorMessageHeader header;
        memcpy(&header, msg_buffer, sizeof(header));

        if (header.sensor_type != ctx->sensor_type) {
            fprintf(stderr, "[Sensor %d] Received mismatched sensor type: %d. Expected: %d\n",
                    ctx->sensor_id, header.sensor_type, ctx->sensor_type);
            response.status = -1;
            if (MsgReply(rcvid, EOK, &response, sizeof(response)) == -1) {
                perror("MsgReply");
            }
            continue;
        }

        response.status = handler_wrapper(ctx, msg_buffer + sizeof(header), header.payload_size);

        /* Reply to unblock the client. */
        if (MsgReply(rcvid, EOK, &response, sizeof(response)) == -1) {
            perror("MsgReply");
        }
    }

    /* If we ever break from the loop, detach name. Not shown here. */
    name_detach(ctx->attach, 0);
    free(ctx);
    return NULL;
}

/*-------------------------------------------------------
 * starts the sever and creates multiple channels/threads
 *-------------------------------------------------------*/
int auv_server_start(void)
{
    if (init_mutex() != 0) {
        printf("Failed to initialize mutex\n");
        return -1; // mutex initialization failed
    }

    ChannelContextThread *sensor_contexts = get_sensor_contexts(NUM_SENSORS);
    if (sensor_contexts == NULL) {
        printf("Failed to get sensor contexts\n");
        return -1; // malloc failure already reported in get_sensor_contexts
    }

    // Each sensor has its own thread
    pthread_t threads[NUM_SENSORS];

    /* For each sensor, attach a name and spawn a thread. */
    for (int i = 0; i < NUM_SENSORS; i++) {
        char server_name[64];
        snprintf(server_name, sizeof(server_name), "SensorServer_%d", sensor_contexts[i].sensor_id);

        name_attach_t *attach = name_attach(NULL, server_name, 0);
        if (attach == NULL) {
            perror("name_attach");
            free(sensor_contexts);
            printf("Failed to attach name for sensor %d\n", sensor_contexts[i].sensor_id);
            return -1;
        }

        // Assign the attach pointer to the context
        sensor_contexts[i].attach = attach;


        /* Launch the channel's thread */
        if (pthread_create(&threads[i], NULL, channel_thread, (void *)&sensor_contexts[i]) != 0) {
            perror("pthread_create");
            free(sensor_contexts);
            printf("Failed to create thread for sensor %d\n", sensor_contexts[i].sensor_id);
            return -1;
        }
    }

    free(sensor_contexts);
    return 0;
}

int handler_wrapper(ChannelContextThread *ctx, const void *data, size_t data_size)
{
    print("[Sensor %d] Handler wrapper called.\n", ctx->sensor_id);
    // For example, update shared state before calling the handler.
    extern AuvState *global_state;
    pthread_mutex_lock(&global_state->mutex);

    // convert the data to a sensor type using the sensor_map.h
    size_t expected_size = get_sensor_payload_size(ctx->sensor_type);
    if (expected_size == 0) {
        fprintf(stderr, "[Sensor %d] Unknown sensor type.\n", ctx->sensor_id);
        pthread_mutex_unlock(&global_state->mutex);
        return -1;
    }
    if (data_size != expected_size) {
        fprintf(stderr, "[Sensor %d] Payload size mismatch: got %zu, expected %zu\n",
                ctx->sensor_id, data_size, expected_size);
        pthread_mutex_unlock(&global_state->mutex);
        return -1;
    }

    // Now call the specific handler for this sensor.
    if (ctx->handler) {
        ctx->handler(data);
    } else {
        fprintf(stderr, "[Sensor %d] No handler defined.\n", ctx->sensor_id);
        pthread_mutex_unlock(&global_state->mutex);
        return -1;
    }

    pthread_mutex_unlock(&global_state->mutex);
    return 0;
}
