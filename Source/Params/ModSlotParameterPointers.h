#pragma once

#include <atomic>

struct ModSlotParameterPointers
{
    std::atomic<float>* source      = nullptr;
    std::atomic<float>* destination = nullptr;
    std::atomic<float>* amount      = nullptr;
};
