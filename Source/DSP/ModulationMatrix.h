#pragma once

#include <juce_core/juce_core.h>

/**
    Fuentes de modulación disponibles en esta fase. "MIDI CC" genérico
    (cualquier número de controlador, no solo Mod Wheel) y "Key Tracking"
    como fuente separada de Note Number quedan pendientes — ver README,
    "Problemas conocidos" de la Fase 4.
*/
namespace ModulationSources
{
    enum Type
    {
        none = 0, lfo1, lfo2, envelope1, envelope2,
        velocity, modWheel, aftertouch, noteNumber, random
    };

    /** Nombres en el mismo orden que el enum Type. */
    const juce::StringArray& getNames();
}

/**
    Destinos de modulación disponibles en esta fase. Fine Pitch (aparte de
    OSC1/OSC2 Pitch), Filter Resonance, Pan y LFO Rate como destinos quedan
    pendientes — ver README, "Problemas conocidos" de la Fase 4.
*/
namespace ModulationDestinations
{
    enum Type
    {
        none = 0, osc1Pitch, osc2Pitch, osc1WavePosition, osc2WavePosition,
        filterCutoff, amplifier
    };

    /** Nombres en el mismo orden que el enum Type. */
    const juce::StringArray& getNames();
}

/** Valor actual (crudo, aproximadamente -1..1 o 0..1 según la fuente) de
    cada fuente de modulación, calculado una vez por muestra en SynthVoice. */
struct ModulationSourceValues
{
    float lfo1 = 0.0f, lfo2 = 0.0f;
    float envelope1 = 0.0f, envelope2 = 0.0f;
    float velocity = 0.0f;
    float modWheel = 0.0f, aftertouch = 0.0f;
    float noteNumber = 0.0f;
    float random = 0.0f;

    float get (ModulationSources::Type source) const noexcept;
};

/** Acumulador de modulación cruda (sin escalar a unidades físicas) por
    destino — SynthVoice aplica la escala física correspondiente a cada
    uno (semitonos, octavas, etc.) al consumir este resultado. */
struct ModulationResult
{
    float osc1PitchSemitones = 0.0f;
    float osc2PitchSemitones = 0.0f;
    float osc1WavePositionDelta = 0.0f;
    float osc2WavePositionDelta = 0.0f;
    float filterCutoffOctaves = 0.0f;
    float amplifierMultiplierDelta = 0.0f;

    void accumulate (ModulationDestinations::Type destination, float value) noexcept;
};

/** Una sola ruta fuente -> destino -> cantidad. La matriz completa de esta
    fase son 4 instancias de esto (ver ParameterLayout / SynthVoiceParameters). */
struct ModulationSlot
{
    ModulationSources::Type source = ModulationSources::none;
    ModulationDestinations::Type destination = ModulationDestinations::none;
    float amount = 0.0f; // -1..1

    void apply (const ModulationSourceValues& sources, ModulationResult& result) const noexcept;
};
