#pragma once

#include <juce_core/juce_core.h>

/**
    IDs de parámetros del plugin.

    IMPORTANTE: una vez publicado un preset o proyecto de Ableton Live que use
    un ID, ese ID no debe cambiar de nombre — Ableton Live guarda automatizaciones
    y estado del plugin referenciando estos strings. Nuevos parámetros de fases
    futuras se agregan aquí, nunca se renombran los existentes.
*/
namespace ParamIDs
{
    static const juce::String masterVolume { "masterVolume" };

    // Osciladores (Fase 2)
    inline juce::String oscWavetableIndex (int oscNumber) { return "osc" + juce::String (oscNumber) + "WavetableIndex"; }
    inline juce::String oscWavePosition   (int oscNumber) { return "osc" + juce::String (oscNumber) + "WavePosition"; }
    inline juce::String oscOctave         (int oscNumber) { return "osc" + juce::String (oscNumber) + "Octave"; }
    inline juce::String oscCoarseTune     (int oscNumber) { return "osc" + juce::String (oscNumber) + "CoarseTune"; }
    inline juce::String oscFineTune       (int oscNumber) { return "osc" + juce::String (oscNumber) + "FineTune"; }
    inline juce::String oscLevel          (int oscNumber) { return "osc" + juce::String (oscNumber) + "Level"; }

    // Filtro (Fase 3)
    static const juce::String filterType      { "filterType" };
    static const juce::String filterCutoff    { "filterCutoff" };
    static const juce::String filterResonance { "filterResonance" };
    static const juce::String filterKeyTrack  { "filterKeyTrack" };
    static const juce::String filterEnvAmount { "filterEnvAmount" };

    // Envolventes (Fase 3): env 1 = amplificador, env 2 = filtro
    inline juce::String envAttack  (int envNumber) { return "env" + juce::String (envNumber) + "Attack"; }
    inline juce::String envDecay   (int envNumber) { return "env" + juce::String (envNumber) + "Decay"; }
    inline juce::String envSustain (int envNumber) { return "env" + juce::String (envNumber) + "Sustain"; }
    inline juce::String envRelease (int envNumber) { return "env" + juce::String (envNumber) + "Release"; }

    // LFOs (Fase 4)
    inline juce::String lfoWaveform     (int lfoNumber) { return "lfo" + juce::String (lfoNumber) + "Waveform"; }
    inline juce::String lfoRate         (int lfoNumber) { return "lfo" + juce::String (lfoNumber) + "Rate"; }
    inline juce::String lfoSynced       (int lfoNumber) { return "lfo" + juce::String (lfoNumber) + "Synced"; }
    inline juce::String lfoSyncDivision (int lfoNumber) { return "lfo" + juce::String (lfoNumber) + "SyncDivision"; }
    inline juce::String lfoRetrigger    (int lfoNumber) { return "lfo" + juce::String (lfoNumber) + "Retrigger"; }
    inline juce::String lfoPhase        (int lfoNumber) { return "lfo" + juce::String (lfoNumber) + "Phase"; }
    inline juce::String lfoDepth        (int lfoNumber) { return "lfo" + juce::String (lfoNumber) + "Depth"; }

    // Matriz de modulación (Fase 4): 4 rutas fijas
    inline juce::String modSlotSource      (int slotNumber) { return "mod" + juce::String (slotNumber) + "Source"; }
    inline juce::String modSlotDestination (int slotNumber) { return "mod" + juce::String (slotNumber) + "Destination"; }
    inline juce::String modSlotAmount      (int slotNumber) { return "mod" + juce::String (slotNumber) + "Amount"; }

    // --- Fases futuras --------------------------------------------------
    // Fase 5: Chorus, Delay, Reverb, Drive
    // Fase 8: Vintage Character
}
