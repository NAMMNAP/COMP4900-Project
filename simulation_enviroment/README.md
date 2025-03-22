This directory will contain eveything related to the simulation environment.

The simulation environmnet consists of objects which we desire to simulate.

## Environment Overview

The simulation environment provides a simple 2D grid system (10x10) for tracking and manipulating object positions. It's designed to run as a separate thread that processes position updates asynchronously.

### Key Features:

-   Thread-safe position updates and retrieval
-   Command queue for position changes
-   2D grid representation of object locations

## Mission Scripts

Mission scripts are programs that interact with the simulation environment. They demonstrate how to:

1. Initialize the environment
2. Send position updates to objects
3. Retrieve current object positions
4. Properly shut down the environment

The provided `mission_script_example.c` shows a basic workflow:

-   Starting the environment
-   Moving an object to specific coordinates
-   Reading the object's position
-   Stopping the environment properly

You can create your own mission scripts following this pattern to test different navigation scenarios.
