#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // for sleep/usleep

#define MAX_CMDS 64

// The environment is a simple 2D grid for demonstration.
// Adjust these sizes as needed.
#define ENV_WIDTH  10
#define ENV_HEIGHT 10

// Track if the environment thread is running
static int running = 0;

// A simple struct for the position
struct position {
    float posX;
    float posY;
};

// Global for the "main" tracked position
static struct position mainPosition = {5.0f, 5.0f};

// 2D Grid: 1 means the object is located there; 0 means empty.
static int environmentGrid[ENV_HEIGHT][ENV_WIDTH] = {0};

// Command queue for positions
static struct position commandQueue[MAX_CMDS];
static int  queueHead = 0;
static int  queueTail = 0;

// Thread synchronization
static pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  queueCond  = PTHREAD_COND_INITIALIZER;
static pthread_t       envThread;


// Start/Stop environment
int environmentStart(void);
int environmentStop(void);

// Public function to update position (pushes new position into the queue)
void environmentUpdatePosition(float posX, float posY);

// Get the current position in a thread‐safe manner.
struct position environmentGetPosition(void);

#endif
