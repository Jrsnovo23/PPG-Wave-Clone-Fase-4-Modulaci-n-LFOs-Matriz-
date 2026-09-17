#pragma once

#include <atomic>

struct FilterParameterPointers
{
    std::atomic<float>* type       = nullptr; // índice de choice: 0=LP, 1=HP, 2=BP
    std::atomic<float>* cutoffHz   = nullptr;
    std::atomic<float>* resonance  = nullptr;
    std::atomic<float>* keyTrack   = nullptr;
    std::atomic<float>* envAmount  = nullptr; // bipolar, -1..1
};
