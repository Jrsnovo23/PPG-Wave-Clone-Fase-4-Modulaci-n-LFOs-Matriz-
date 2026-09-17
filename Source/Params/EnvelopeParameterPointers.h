#pragma once

#include <atomic>

struct EnvelopeParameterPointers
{
    std::atomic<float>* attack  = nullptr;
    std::atomic<float>* decay   = nullptr;
    std::atomic<float>* sustain = nullptr;
    std::atomic<float>* release = nullptr;
};
