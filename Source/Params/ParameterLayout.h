#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

/**
    Punto único de creación del árbol de parámetros (AudioProcessorValueTreeState).

    Cada fase del proyecto añade sus propios parámetros aquí dentro de la misma
    función, agrupados por sección (Oscillators, Filter, Envelopes, LFO, Mod
    Matrix, Effects, Vintage Character...). Mantener esto centralizado evita
    que el sistema de parámetros quede disperso por la base de código.
*/
namespace ParameterLayout
{
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
