#pragma once

#include <atomic>

struct LFOParameterPointers
{
    std::atomic<float>* waveform     = nullptr;
    std::atomic<float>* rateHz       = nullptr;
    std::atomic<float>* synced       = nullptr; // bool como float (0/1)
    std::atomic<float>* syncDivision = nullptr;
    std::atomic<float>* retrigger    = nullptr; // bool como float (0/1)
    std::atomic<float>* phase        = nullptr;
    std::atomic<float>* depth        = nullptr;
};
