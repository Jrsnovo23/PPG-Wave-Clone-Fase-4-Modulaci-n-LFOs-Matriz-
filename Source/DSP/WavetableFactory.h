#pragma once

#include "WavetableSet.h"
#include <memory>

namespace WavetableFactory
{
    /** Crea la biblioteca "Básicas" de la Fase 2: Sine, Triangle, Saw, Square,
        PWM (multi-frame) y Harmonic Morph (multi-frame, demuestra el escaneo
        de wavetable). Todas generadas por síntesis aditiva, sin muestras ni
        datos de terceros. */
    std::unique_ptr<WavetableSet> createDefaultSet();

    /** Nombres en el MISMO orden que createDefaultSet() los agrega — usado
        también por ParameterLayout para construir el AudioParameterChoice,
        así ambos lados nunca pueden desincronizarse. */
    const juce::StringArray& getDefaultWavetableNames();
}
