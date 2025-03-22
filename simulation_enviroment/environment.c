#include "environment.h"

// Forward-declare the function the thread will call
static void* envThreadFunc(void *arg);

// Forward-declare the function that processes the position update
static void envUpdatePosition(struct position pos);

// Helper to push a position update into the command queue
static void envPushCommand(struct position pos);

// Helper to pop a position update from the command queue
static int popUpdateCommand(struct position *outPos);

/**
 *  environmentStart()
 *  Spawns the environment thread and returns 0 on success,
 *  non-zero otherwise.
 */
int environmentStart(void)
{
    int ret = pthread_create(&envThread, NULL, envThreadFunc, NULL);
    if (ret != 0) {
        fprintf(stderr, "Failed to create env thread.\n");
        return ret;
    }
    return 0;
}


/**
 *  environmentStop()
 *  Signals the environment thread to stop and joins it.
 */
int environmentStop(void)
{
    running = 0;
    pthread_mutex_lock(&queueMutex);
    pthread_cond_broadcast(&queueCond);  // wake up any waiting thread
    pthread_mutex_unlock(&queueMutex);

    pthread_join(envThread, NULL);
    return 0;
}


/**
 *  environmentUpdatePosition(float posX, float posY)
 *  PUBLIC function - call this with your new coordinates.
 *  Internally, we push them to the queue for the environment thread.
 */
void environmentUpdatePosition(float posX, float posY)
{
    struct position newPos;
    newPos.posX = posX;
    newPos.posY = posY;

    envPushCommand(newPos);
}

/**
 * environmentGetPosition(void)
 * Thread‐safe retrieval of the current mainPosition.
 */
struct position environmentGetPosition(void)
{
    pthread_mutex_lock(&queueMutex);
    struct position currentPos = mainPosition;
    pthread_mutex_unlock(&queueMutex);

    return currentPos;
}




/* ------------------------------------------------------------------
   STATIC / INTERNAL FUNCTIONS
   ------------------------------------------------------------------ */

/**
 *  envPushCommand(struct position pos)
 *  Queues a new position command in a thread-safe manner.
 */
static void envPushCommand(struct position pos)
{
    pthread_mutex_lock(&queueMutex);

    int nextTail = (queueTail + 1) % MAX_CMDS;
    if (nextTail == queueHead) {
        // Queue is full, dropping this command
        fprintf(stderr, "Dropping position update: (%.2f, %.2f)\n",
                pos.posX, pos.posY);
    } else {
        commandQueue[queueTail] = pos;
        queueTail = nextTail;
        pthread_cond_signal(&queueCond);
    }

    pthread_mutex_unlock(&queueMutex);
}


/**
 *  popUpdateCommand(struct position *outPos)
 *  Pops a position command from the queue, if available.
 *  Returns 1 on success, 0 if queue is empty.
 */
static int popUpdateCommand(struct position *outPos)
{
    pthread_mutex_lock(&queueMutex);

    if (queueHead == queueTail) {
        // queue is empty
        pthread_mutex_unlock(&queueMutex);
        return 0;
    }

    *outPos = commandQueue[queueHead];
    queueHead = (queueHead + 1) % MAX_CMDS;

    pthread_mutex_unlock(&queueMutex);
    return 1;
}


/**
 *  envThreadFunc(void *arg)
 *  This is the main thread for the environment.
 *  It loops while 'running' is true, pops commands off the queue,
 *  processes them via envUpdatePosition, and sleeps a bit.
 */
static void* envThreadFunc(void *arg)
{
    (void)arg;
    running = 1;

    // Ensure our starting position is marked in the grid
    // (This sets environmentGrid[ mainPosition.posY ][ mainPosition.posX ] = 1)
    envUpdatePosition(mainPosition);

    while (running) {
        struct position pos;
        // Process all queued commands
        while (popUpdateCommand(&pos)) {
            envUpdatePosition(pos);
        }

        // Example: show that we're still "running" and the last known position
        printf("Environment running. Current position: (%.2f, %.2f)\n",
               mainPosition.posX, mainPosition.posY);
        fflush(stdout);

        // Sleep to avoid using 100% CPU
        usleep(200000); // 0.2 seconds
    }

    printf("Environment thread stopped.\n");
    return NULL;
}


/**
 *  envUpdatePosition(struct position pos)
 *  INTERNAL function that updates the environment. Here, it:
 *    1. Clears the old position in the 2D grid.
 *    2. Updates mainPosition.
 *    3. Sets the new position in the 2D grid (if in range).
 */
static void envUpdatePosition(struct position pos)
{
    // First clear the old position in the grid, if in bounds
    int oldX = (int)mainPosition.posX;
    int oldY = (int)mainPosition.posY;
    if (oldX >= 0 && oldX < ENV_WIDTH &&
        oldY >= 0 && oldY < ENV_HEIGHT) {
        environmentGrid[oldY][oldX] = 0;
    }

    // Update the global mainPosition
    mainPosition.posX = pos.posX;
    mainPosition.posY = pos.posY;

    // Mark the new position in the grid, if in bounds
    int newX = (int)pos.posX;
    int newY = (int)pos.posY;
    if (newX >= 0 && newX < ENV_WIDTH &&
        newY >= 0 && newY < ENV_HEIGHT) {
        environmentGrid[newY][newX] = 1;
    }
}
