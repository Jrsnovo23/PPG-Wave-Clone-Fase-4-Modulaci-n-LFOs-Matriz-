#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

/**
    Sonido "comodín": el motor polifónico de JUCE (juce::Synthesiser) exige un
    par Sound/Voice. Como este instrumento no distingue sonidos por rango de
    nota o velocidad (toda la síntesis vive dentro de la voz), un único
    SynthSound que acepta cualquier nota/canal es suficiente.
*/
class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override        { return true; }
    bool appliesToChannel (int) override      { return true; }
};
