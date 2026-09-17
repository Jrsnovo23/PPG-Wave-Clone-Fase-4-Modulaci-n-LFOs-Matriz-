#include "SynthVoice.h"
#include <cmath>

SynthVoice::SynthVoice (const WavetableSet& wavetableSetIn, SynthVoiceParameters& paramsIn)
    : params (paramsIn)
{
    osc1.setWavetableSet (&wavetableSetIn);
    osc2.setWavetableSet (&wavetableSetIn);
}

void SynthVoice::refreshOscillatorBaseSettings()
{
    osc1.setWavetableIndex ((int) params.osc1.wavetableIndex->load());
    osc1.setLevel (params.osc1.level->load());
    osc2.setWavetableIndex ((int) params.osc2.wavetableIndex->load());
    osc2.setLevel (params.osc2.level->load());

    osc1.setSampleRate (getSampleRate());
    osc2.setSampleRate (getSampleRate());

    const auto computeFreq = [this] (const OscillatorParameterPointers& p) -> float
    {
        const float octave = p.octave->load();
        const float coarse = p.coarseTune->load();
        const float fine   = p.fineTune->load();
        const float semitoneOffset = (octave * 12.0f) + coarse + (fine / 100.0f);
        return (float) (baseFrequencyHz * std::pow (2.0, (double) semitoneOffset / 12.0));
    };

    baseOsc1FreqHz = computeFreq (params.osc1);
    baseOsc2FreqHz = computeFreq (params.osc2);
    baseOsc1Position = params.osc1.wavePosition->load();
    baseOsc2Position = params.osc2.wavePosition->load();
}

void SynthVoice::refreshEnvelopeSettings()
{
    envelope1.setSampleRate (getSampleRate());
    envelope1.setAttackSeconds (params.env1.attack->load());
    envelope1.setDecaySeconds (params.env1.decay->load());
    envelope1.setSustainLevel (params.env1.sustain->load());
    envelope1.setReleaseSeconds (params.env1.release->load());

    envelope2.setSampleRate (getSampleRate());
    envelope2.setAttackSeconds (params.env2.attack->load());
    envelope2.setDecaySeconds (params.env2.decay->load());
    envelope2.setSustainLevel (params.env2.sustain->load());
    envelope2.setReleaseSeconds (params.env2.release->load());
}

void SynthVoice::refreshFilterStaticSettings()
{
    filter.setSampleRate (getSampleRate());

    const int typeIndex = (int) params.filter.type->load();
    filter.setType (typeIndex == 1 ? Filter::Type::highPass
                   : typeIndex == 2 ? Filter::Type::bandPass
                                    : Filter::Type::lowPass);

    filter.setResonance (params.filter.resonance->load());
}

void SynthVoice::refreshLFOSettings()
{
    auto applyTo = [this] (LFO& lfo, LFOParameterPointers& p)
    {
        lfo.setSampleRate (getSampleRate());
        lfo.setWaveform ((LFOWaveform) (int) p.waveform->load());
        lfo.setRateHz (p.rateHz->load());
        lfo.setTempoSynced (p.synced->load() > 0.5f);
        lfo.setSyncDivisionIndex ((int) p.syncDivision->load());
        lfo.setRetrigger (p.retrigger->load() > 0.5f);
        lfo.setPhaseOffset (p.phase->load());
        lfo.setDepth (p.depth->load());
        lfo.setHostBpm (hostBpm);
    };

    applyTo (lfo1, params.lfo1);
    applyTo (lfo2, params.lfo2);
}

std::array<ModulationSlot, 4> SynthVoice::readModulationSlots() const
{
    std::array<ModulationSlot, 4> slots;
    for (size_t i = 0; i < slots.size(); ++i)
    {
        auto& p = params.modSlots[i];
        slots[i].source      = (ModulationSources::Type) (int) p.source->load();
        slots[i].destination = (ModulationDestinations::Type) (int) p.destination->load();
        slots[i].amount      = p.amount->load();
    }
    return slots;
}

void SynthVoice::startNote (int midiNoteNumber, float velocity,
                             juce::SynthesiserSound*, int /*pitchWheel*/)
{
    currentMidiNote = midiNoteNumber;
    baseFrequencyHz = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    velocityGain = velocity;
    randomValue = randomGen.nextFloat() * 2.0f - 1.0f;

    osc1.resetPhase();
    osc2.resetPhase();
    refreshOscillatorBaseSettings();

    refreshFilterStaticSettings();
    filter.reset();

    refreshEnvelopeSettings();
    envelope1.noteOn();
    envelope2.noteOn();

    refreshLFOSettings();
    lfo1.noteOn();
    lfo2.noteOn();
}

void SynthVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (! allowTailOff)
        clearCurrentNote();

    envelope1.noteOff();
    envelope2.noteOff();
}

void SynthVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
    if (controllerNumber == 1) // Mod Wheel (CC1)
        modWheelValue = (float) newControllerValue / 127.0f;
}

void SynthVoice::aftertouchChanged (int newAftertouchValue)
{
    aftertouchValue = (float) newAftertouchValue / 127.0f;
}

void SynthVoice::channelPressureChanged (int newChannelPressureValue)
{
    aftertouchValue = (float) newChannelPressureValue / 127.0f;
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                   int startSample, int numSamples)
{
    // Ajustes que no necesitan resolución por muestra: una vez por bloque.
    refreshOscillatorBaseSettings();
    refreshEnvelopeSettings();
    refreshFilterStaticSettings();
    refreshLFOSettings();

    const auto modSlots = readModulationSlots();

    const float baseCutoff = params.filter.cutoffHz->load();
    const float keyTrack   = params.filter.keyTrack->load();
    const float envAmount  = params.filter.envAmount->load();

    const float keyTrackOctaves = keyTrack * (float) (currentMidiNote - 60) / 12.0f;
    const float keyTrackMultiplier = std::pow (2.0f, keyTrackOctaves);
    const float noteNumberNormalized = juce::jlimit (-1.0f, 1.0f, (float) (currentMidiNote - 60) / 60.0f);

    while (--numSamples >= 0)
    {
        const float ampEnv    = envelope1.getNextSample();
        const float filterEnv = envelope2.getNextSample();
        const float lfo1Value = lfo1.renderSample();
        const float lfo2Value = lfo2.renderSample();

        ModulationSourceValues sourceValues;
        sourceValues.lfo1 = lfo1Value;
        sourceValues.lfo2 = lfo2Value;
        sourceValues.envelope1 = ampEnv;
        sourceValues.envelope2 = filterEnv;
        sourceValues.velocity = velocityGain;
        sourceValues.modWheel = modWheelValue;
        sourceValues.aftertouch = aftertouchValue;
        sourceValues.noteNumber = noteNumberNormalized;
        sourceValues.random = randomValue;

        ModulationResult modResult;
        for (auto& slot : modSlots)
            slot.apply (sourceValues, modResult);

        // Osciladores: pitch en un rango util de +/-24 semitonos, wave
        // position sumada directamente sobre la base (0..1, con clamp).
        osc1.setFrequency (baseOsc1FreqHz * std::pow (2.0f, (modResult.osc1PitchSemitones * 24.0f) / 12.0f));
        osc2.setFrequency (baseOsc2FreqHz * std::pow (2.0f, (modResult.osc2PitchSemitones * 24.0f) / 12.0f));
        osc1.setWavePosition (juce::jlimit (0.0f, 1.0f, baseOsc1Position + modResult.osc1WavePositionDelta));
        osc2.setWavePosition (juce::jlimit (0.0f, 1.0f, baseOsc2Position + modResult.osc2WavePositionDelta));

        // Filtro: cutoff base * key tracking, desplazado por Envelope 2 y
        // por la matriz (+/-4 octavas cada uno en su extremo).
        const float envOctaves = envAmount * filterEnv * 4.0f;
        const float modOctaves = modResult.filterCutoffOctaves * 4.0f;
        const float finalCutoff = juce::jlimit (20.0f, 20000.0f,
            baseCutoff * keyTrackMultiplier * std::pow (2.0f, envOctaves + modOctaves));
        filter.setCutoffHz (finalCutoff);

        const float oscMix   = (osc1.renderSample() + osc2.renderSample()) * 0.5f;
        const float filtered = filter.processSample (oscMix);

        const float ampMultiplier = juce::jlimit (0.0f, 2.0f, 1.0f + modResult.amplifierMultiplierDelta);
        const float sample = filtered * ampEnv * velocityGain * ampMultiplier;

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample (ch, startSample, sample);

        ++startSample;

        if (! envelope1.isActive())
        {
            clearCurrentNote();
            break;
        }
    }
}
