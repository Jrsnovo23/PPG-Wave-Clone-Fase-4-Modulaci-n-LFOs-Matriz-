#include "Filter.h"

namespace
{
    constexpr float minQ = 0.7f;
    constexpr float maxQ = 18.0f;
}

const juce::StringArray& FilterTypeChoices::getNames()
{
    static const juce::StringArray names { "Low Pass", "High Pass", "Band Pass" };
    return names;
}

void Filter::setSampleRate (double sampleRate) noexcept
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = 1; // se procesa muestra por muestra dentro de SynthVoice
    spec.numChannels = 1;
    filter.prepare (spec);
    applyType();
}

void Filter::applyType() noexcept
{
    switch (type)
    {
        case Type::lowPass:  filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);  break;
        case Type::highPass: filter.setType (juce::dsp::StateVariableTPTFilterType::highpass); break;
        case Type::bandPass: filter.setType (juce::dsp::StateVariableTPTFilterType::bandpass); break;
    }
}

void Filter::setType (Type newType) noexcept
{
    type = newType;
    applyType();
}

void Filter::setCutoffHz (float hz) noexcept
{
    filter.setCutoffFrequency (juce::jlimit (20.0f, 20000.0f, hz));
}

void Filter::setResonance (float amount01) noexcept
{
    const float q = juce::jmap (juce::jlimit (0.0f, 1.0f, amount01), 0.0f, 1.0f, minQ, maxQ);
    filter.setResonance (q);
}

void Filter::reset() noexcept
{
    filter.reset();
}

float Filter::processSample (float input) noexcept
{
    return filter.processSample (0, input);
}
