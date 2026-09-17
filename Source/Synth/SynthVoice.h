#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include "../DSP/Oscillator.h"
#include "../DSP/WavetableSet.h"
#include "../DSP/Filter.h"
#include "../DSP/Envelope.h"
#include "../DSP/LFO.h"
#include "../DSP/ModulationMatrix.h"
#include "../Params/SynthVoiceParameters.h"

/**
    Voz polifónica: OSC1+OSC2 (wavetable) -> filtro multimodo -> salida, con
    Envelope 1 (amplitud), Envelope 2 (filtro), 2 LFOs, y una matriz de 4
    rutas fuente->destino->cantidad.

    Fuentes: LFO1, LFO2, Envelope1, Envelope2, Velocity, Mod Wheel (CC1),
    Aftertouch (channel pressure), Note Number, Random (fijo por nota).
    Destinos: OSC1/OSC2 Pitch, OSC1/OSC2 Wave Position, Filter Cutoff,
    Amplifier. Ver DSP/ModulationMatrix.h y el README de esta fase para lo
    que queda pendiente (MIDI CC genérico, Fine Pitch, Filter Resonance,
    Pan, LFO Rate como destino).
*/
class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice (const WavetableSet& wavetableSetIn, SynthVoiceParameters& paramsIn);

    bool canPlaySound (juce::SynthesiserSound*) override { return true; }

    void startNote (int midiNoteNumber, float velocity,
                     juce::SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int) override {}
    void controllerMoved (int controllerNumber, int newControllerValue) override;
    void aftertouchChanged (int newAftertouchValue) override;
    void channelPressureChanged (int newChannelPressureValue) override;

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                           int startSample, int numSamples) override;

    /** Llamado una vez por bloque desde el PluginProcessor con el BPM
        actual del host, para los LFOs sincronizados al tempo. */
    void setHostBpm (double bpm) noexcept { hostBpm = bpm; }

private:
    Oscillator osc1, osc2;
    Filter filter;
    Envelope envelope1, envelope2;
    LFO lfo1, lfo2;

    SynthVoiceParameters& params;

    double baseFrequencyHz = 440.0;
    int currentMidiNote = 60;
    float velocityGain = 1.0f;
    float randomValue = 0.0f;
    float modWheelValue = 0.0f;
    float aftertouchValue = 0.0f;
    double hostBpm = 120.0;
    juce::Random randomGen;

    float baseOsc1FreqHz = 440.0f, baseOsc2FreqHz = 440.0f;
    float baseOsc1Position = 0.0f, baseOsc2Position = 0.0f;

    void refreshOscillatorBaseSettings();
    void refreshEnvelopeSettings();
    void refreshFilterStaticSettings();
    void refreshLFOSettings();
    std::array<ModulationSlot, 4> readModulationSlots() const;
};
