#pragma once

struct QueueFamilyIndices
{
    int graphicsFamily = -1; // location of graphics queue family
    // check if queuefamily is valid
    bool IsValid() {
        return graphicsFamily >= 0;
    }
};
