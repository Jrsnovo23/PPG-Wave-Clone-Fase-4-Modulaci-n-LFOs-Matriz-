#pragma once

#include <juce_core/juce_core.h>

/**
    Envolvente ADSR con segmentos lineales (attack 0->1, decay 1->sustain,
    sustain constante, release nivel_actual->0). Suficiente para sonidos
    musicales rápidos (percusivos) y evoluciones lentas según los tiempos
    configurados — curvas exponenciales quedan como posible refinamiento
    futuro, no son necesarias para que suene musical.

    Si noteOn() llega mientras la envolvente sigue activa (retrigger antes
    de llegar a idle), continúa desde el nivel actual en vez de saltar a 0,
    evitando clicks.
*/
class Envelope
{
public:
    void setSampleRate (double newSampleRate) noexcept;

    void setAttackSeconds (float seconds) noexcept  { attackTime = juce::jmax (0.0001f, seconds); recalculate(); }
    void setDecaySeconds (float seconds) noexcept   { decayTime = juce::jmax (0.0001f, seconds); recalculate(); }
    void setSustainLevel (float level01) noexcept   { sustainLevel = juce::jlimit (0.0f, 1.0f, level01); }
    void setReleaseSeconds (float seconds) noexcept { releaseTime = juce::jmax (0.0001f, seconds); recalculate(); }

    void noteOn() noexcept;
    void noteOff() noexcept;

    /** false solo cuando terminó por completo el release (o nunca se disparó). */
    bool isActive() const noexcept { return stage != Stage::idle; }

    float getNextSample() noexcept;

private:
    enum class Stage { idle, attack, decay, sustain, release };

    Stage stage = Stage::idle;
    double sampleRate = 44100.0;

    float attackTime = 0.01f, decayTime = 0.1f, sustainLevel = 0.8f, releaseTime = 0.2f;
    float attackRate = 0.0f, decayRate = 0.0f, releaseRate = 0.0f;

    float currentLevel = 0.0f;

    void recalculate() noexcept;
};
