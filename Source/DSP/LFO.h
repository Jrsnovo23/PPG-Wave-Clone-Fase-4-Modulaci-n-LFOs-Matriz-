#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

enum class LFOWaveform { sine, triangle, square, sawUp, sawDown, random, sampleAndHold };

/** Nombres en el mismo orden que LFOWaveform. */
namespace LFOWaveformChoices
{
    const juce::StringArray& getNames();
}

/** Divisiones de tempo disponibles para el modo Sync, y su duración en
    negras (quarter notes) — usado para calcular el rate real en Hz a
    partir del BPM del host. */
namespace LFOSyncDivisionChoices
{
    const juce::StringArray& getNames();
    float getQuarterNoteMultiplier (int index);
}

/**
    LFO con 7 formas de onda, rate libre en Hz o sincronizado al tempo del
    host, retrigger opcional (reinicia fase en noteOn), fase inicial y
    depth. "Random" interpola suavemente entre valores aleatorios cada
    ciclo (LFO "wandering"); "Sample & Hold" mantiene el valor fijo durante
    todo el ciclo y salta al siguiente.
*/
class LFO
{
public:
    void setSampleRate (double sr) noexcept { sampleRate = sr; }
    void setWaveform (LFOWaveform w) noexcept { waveform = w; }
    void setRateHz (float hz) noexcept { freeRateHz = juce::jmax (0.01f, hz); }
    void setTempoSynced (bool shouldSync) noexcept { synced = shouldSync; }
    void setSyncDivisionIndex (int index) noexcept { syncDivisionIndex = index; }
    void setHostBpm (double bpm) noexcept { hostBpm = bpm; }
    void setDepth (float depth01) noexcept { depth = juce::jlimit (0.0f, 1.0f, depth01); }
    void setPhaseOffset (float phase01) noexcept { phaseOffset = juce::jlimit (0.0f, 1.0f, phase01); }
    void setRetrigger (bool shouldRetrigger) noexcept { retrigger = shouldRetrigger; }

    void noteOn() noexcept
    {
        if (! retrigger)
            return;

        phase = phaseOffset;
        if (waveform == LFOWaveform::random || waveform == LFOWaveform::sampleAndHold)
        {
            heldValue = randomBipolar();
            nextHeldValue = randomBipolar();
        }
    }

    float renderSample() noexcept
    {
        const float currentRateHz = synced
            ? (float) (hostBpm / 60.0 / (double) LFOSyncDivisionChoices::getQuarterNoteMultiplier (syncDivisionIndex))
            : freeRateHz;

        const double increment = sampleRate > 0.0 ? (double) currentRateHz / sampleRate : 0.0;

        float value = 0.0f;
        switch (waveform)
        {
            case LFOWaveform::sine:     value = (float) std::sin (juce::MathConstants<double>::twoPi * phase); break;
            case LFOWaveform::triangle: value = triangleFromPhase (phase); break;
            case LFOWaveform::square:   value = phase < 0.5 ? 1.0f : -1.0f; break;
            case LFOWaveform::sawUp:    value = (float) (2.0 * phase - 1.0); break;
            case LFOWaveform::sawDown:  value = (float) (1.0 - 2.0 * phase); break;
            case LFOWaveform::random:        value = juce::jmap ((float) phase, 0.0f, 1.0f, heldValue, nextHeldValue); break;
            case LFOWaveform::sampleAndHold: value = heldValue; break;
        }

        phase += increment;
        if (phase >= 1.0)
        {
            phase -= 1.0;
            if (waveform == LFOWaveform::random || waveform == LFOWaveform::sampleAndHold)
            {
                heldValue = nextHeldValue;
                nextHeldValue = randomBipolar();
            }
        }

        return value * depth;
    }

private:
    static float triangleFromPhase (double phase) noexcept
    {
        return (float) (phase < 0.5 ? (4.0 * phase - 1.0) : (3.0 - 4.0 * phase));
    }

    float randomBipolar() noexcept { return random.nextFloat() * 2.0f - 1.0f; }

    LFOWaveform waveform = LFOWaveform::sine;
    double sampleRate = 44100.0;
    double phase = 0.0;
    float phaseOffset = 0.0f;
    float freeRateHz = 2.0f;
    bool synced = false;
    int syncDivisionIndex = 2; // por defecto "1/4"
    double hostBpm = 120.0;
    float depth = 1.0f;
    bool retrigger = true;

    float heldValue = 0.0f, nextHeldValue = 0.0f;
    juce::Random random;
};
