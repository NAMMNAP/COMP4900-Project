#include "environment.h"
#include <stdio.h>
#include <unistd.h>

int main(void) {
    if (environmentStart() != 0) {
        return 1;
    }

    // Move the object around
    environmentUpdatePosition(6.0f, 6.0f);
    sleep(1);

    // Read current position with get function
    struct position pos = environmentGetPosition();
    printf("Got position: (%.2f, %.2f)\n", pos.posX, pos.posY);

    environmentUpdatePosition(2.0f, 1.0f);
    sleep(1);

    // Stop environment
    environmentStop();

    return 0;
}
