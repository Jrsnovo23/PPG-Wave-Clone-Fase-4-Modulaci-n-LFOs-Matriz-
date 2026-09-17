#pragma once

#include <atomic>

/**
    Puntero directo (no-owning) a los parámetros de UN oscilador dentro del
    APVTS. Se construye una vez en el PluginProcessor y se pasa por
    referencia a cada SynthVoice — todas las voces comparten los mismos
    parámetros de oscilador (son ajustes del patch, no por nota); solo la
    frecuencia base difiere por voz según la nota tocada.
*/
struct OscillatorParameterPointers
{
    std::atomic<float>* wavetableIndex = nullptr;
    std::atomic<float>* wavePosition   = nullptr;
    std::atomic<float>* octave         = nullptr;
    std::atomic<float>* coarseTune     = nullptr;
    std::atomic<float>* fineTune       = nullptr;
    std::atomic<float>* level          = nullptr;
};
