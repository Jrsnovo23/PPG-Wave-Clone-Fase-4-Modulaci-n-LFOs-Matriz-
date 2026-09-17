#pragma once

#include <juce_dsp/juce_dsp.h>

/**
    Filtro multimodo (Low Pass / High Pass / Band Pass). Internamente usa
    juce::dsp::StateVariableTPTFilter — una implementación de filtro de
    variable de estado ya probada y estable en tiempo real, en vez de
    reinventar el álgebra del filtro desde cero. El comportamiento musical
    (cutoff, resonancia, mapeo a Q) es control propio de esta clase.
*/
class Filter
{
public:
    enum class Type { lowPass, highPass, bandPass };

    void setSampleRate (double sampleRate) noexcept;
    void setType (Type newType) noexcept;
    void setCutoffHz (float hz) noexcept;
    void setResonance (float amount01) noexcept; // 0..1 -> Q

    void reset() noexcept;
    float processSample (float input) noexcept;

private:
    juce::dsp::StateVariableTPTFilter<float> filter;
    Type type = Type::lowPass;

    void applyType() noexcept;
};

/** Nombres de los tipos de filtro, en el mismo orden que Filter::Type —
    usados tanto por ParameterLayout como por el editor, para que nunca se
    desincronicen (mismo patrón que WavetableFactory::getDefaultWavetableNames). */
namespace FilterTypeChoices
{
    const juce::StringArray& getNames();
}
