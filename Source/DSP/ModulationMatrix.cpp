#include "ModulationMatrix.h"

const juce::StringArray& ModulationSources::getNames()
{
    static const juce::StringArray names {
        "None", "LFO 1", "LFO 2", "Envelope 1", "Envelope 2",
        "Velocity", "Mod Wheel", "Aftertouch", "Note Number", "Random"
    };
    return names;
}

const juce::StringArray& ModulationDestinations::getNames()
{
    static const juce::StringArray names {
        "None", "OSC1 Pitch", "OSC2 Pitch", "OSC1 Wave Pos", "OSC2 Wave Pos",
        "Filter Cutoff", "Amplifier"
    };
    return names;
}

float ModulationSourceValues::get (ModulationSources::Type source) const noexcept
{
    switch (source)
    {
        case ModulationSources::lfo1:       return lfo1;
        case ModulationSources::lfo2:       return lfo2;
        case ModulationSources::envelope1:  return envelope1;
        case ModulationSources::envelope2:  return envelope2;
        case ModulationSources::velocity:   return velocity;
        case ModulationSources::modWheel:   return modWheel;
        case ModulationSources::aftertouch: return aftertouch;
        case ModulationSources::noteNumber: return noteNumber;
        case ModulationSources::random:     return random;
        default: return 0.0f;
    }
}

void ModulationResult::accumulate (ModulationDestinations::Type destination, float value) noexcept
{
    switch (destination)
    {
        case ModulationDestinations::osc1Pitch:        osc1PitchSemitones += value; break;
        case ModulationDestinations::osc2Pitch:        osc2PitchSemitones += value; break;
        case ModulationDestinations::osc1WavePosition: osc1WavePositionDelta += value; break;
        case ModulationDestinations::osc2WavePosition: osc2WavePositionDelta += value; break;
        case ModulationDestinations::filterCutoff:     filterCutoffOctaves += value; break;
        case ModulationDestinations::amplifier:        amplifierMultiplierDelta += value; break;
        default: break;
    }
}

void ModulationSlot::apply (const ModulationSourceValues& sources, ModulationResult& result) const noexcept
{
    if (source == ModulationSources::none || destination == ModulationDestinations::none)
        return;

    result.accumulate (destination, sources.get (source) * amount);
}
