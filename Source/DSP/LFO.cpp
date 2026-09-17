#include "LFO.h"
#include <array>

const juce::StringArray& LFOWaveformChoices::getNames()
{
    static const juce::StringArray names {
        "Sine", "Triangle", "Square", "Saw Up", "Saw Down", "Random", "Sample & Hold"
    };
    return names;
}

const juce::StringArray& LFOSyncDivisionChoices::getNames()
{
    static const juce::StringArray names { "1/16", "1/8", "1/4", "1/2", "1/1", "2/1", "4/1" };
    return names;
}

float LFOSyncDivisionChoices::getQuarterNoteMultiplier (int index)
{
    static const std::array<float, 7> multipliers { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f };
    return multipliers[(size_t) juce::jlimit (0, 6, index)];
}
