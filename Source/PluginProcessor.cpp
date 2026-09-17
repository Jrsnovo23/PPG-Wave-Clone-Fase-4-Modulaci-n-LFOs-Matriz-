#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Params/ParameterLayout.h"
#include "Params/ParameterIDs.h"
#include "DSP/WavetableFactory.h"

namespace
{
    constexpr int numVoicesPhase4 = 8; // polifonía configurable real llega en Fase 10

    OscillatorParameterPointers makeOscParams (juce::AudioProcessorValueTreeState& apvts, int oscNumber)
    {
        OscillatorParameterPointers p;
        p.wavetableIndex = apvts.getRawParameterValue (ParamIDs::oscWavetableIndex (oscNumber));
        p.wavePosition   = apvts.getRawParameterValue (ParamIDs::oscWavePosition (oscNumber));
        p.octave         = apvts.getRawParameterValue (ParamIDs::oscOctave (oscNumber));
        p.coarseTune     = apvts.getRawParameterValue (ParamIDs::oscCoarseTune (oscNumber));
        p.fineTune       = apvts.getRawParameterValue (ParamIDs::oscFineTune (oscNumber));
        p.level          = apvts.getRawParameterValue (ParamIDs::oscLevel (oscNumber));
        return p;
    }

    FilterParameterPointers makeFilterParams (juce::AudioProcessorValueTreeState& apvts)
    {
        FilterParameterPointers p;
        p.type      = apvts.getRawParameterValue (ParamIDs::filterType);
        p.cutoffHz  = apvts.getRawParameterValue (ParamIDs::filterCutoff);
        p.resonance = apvts.getRawParameterValue (ParamIDs::filterResonance);
        p.keyTrack  = apvts.getRawParameterValue (ParamIDs::filterKeyTrack);
        p.envAmount = apvts.getRawParameterValue (ParamIDs::filterEnvAmount);
        return p;
    }

    EnvelopeParameterPointers makeEnvParams (juce::AudioProcessorValueTreeState& apvts, int envNumber)
    {
        EnvelopeParameterPointers p;
        p.attack  = apvts.getRawParameterValue (ParamIDs::envAttack (envNumber));
        p.decay   = apvts.getRawParameterValue (ParamIDs::envDecay (envNumber));
        p.sustain = apvts.getRawParameterValue (ParamIDs::envSustain (envNumber));
        p.release = apvts.getRawParameterValue (ParamIDs::envRelease (envNumber));
        return p;
    }

    LFOParameterPointers makeLFOParams (juce::AudioProcessorValueTreeState& apvts, int lfoNumber)
    {
        LFOParameterPointers p;
        p.waveform     = apvts.getRawParameterValue (ParamIDs::lfoWaveform (lfoNumber));
        p.rateHz       = apvts.getRawParameterValue (ParamIDs::lfoRate (lfoNumber));
        p.synced       = apvts.getRawParameterValue (ParamIDs::lfoSynced (lfoNumber));
        p.syncDivision = apvts.getRawParameterValue (ParamIDs::lfoSyncDivision (lfoNumber));
        p.retrigger    = apvts.getRawParameterValue (ParamIDs::lfoRetrigger (lfoNumber));
        p.phase        = apvts.getRawParameterValue (ParamIDs::lfoPhase (lfoNumber));
        p.depth        = apvts.getRawParameterValue (ParamIDs::lfoDepth (lfoNumber));
        return p;
    }

    ModSlotParameterPointers makeModSlotParams (juce::AudioProcessorValueTreeState& apvts, int slotNumber)
    {
        ModSlotParameterPointers p;
        p.source      = apvts.getRawParameterValue (ParamIDs::modSlotSource (slotNumber));
        p.destination = apvts.getRawParameterValue (ParamIDs::modSlotDestination (slotNumber));
        p.amount      = apvts.getRawParameterValue (ParamIDs::modSlotAmount (slotNumber));
        return p;
    }

    SynthVoiceParameters makeSynthVoiceParameters (juce::AudioProcessorValueTreeState& apvts)
    {
        SynthVoiceParameters v;
        v.osc1 = makeOscParams (apvts, 1);
        v.osc2 = makeOscParams (apvts, 2);
        v.filter = makeFilterParams (apvts);
        v.env1 = makeEnvParams (apvts, 1);
        v.env2 = makeEnvParams (apvts, 2);
        v.lfo1 = makeLFOParams (apvts, 1);
        v.lfo2 = makeLFOParams (apvts, 2);
        for (int i = 0; i < 4; ++i)
            v.modSlots[(size_t) i] = makeModSlotParams (apvts, i + 1);
        return v;
    }
}

PPGWaveCloneAudioProcessor::PPGWaveCloneAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", ParameterLayout::createParameterLayout()),
      wavetableSet (WavetableFactory::createDefaultSet()),
      voiceParams (makeSynthVoiceParameters (apvts))
{
    for (int i = 0; i < numVoicesPhase4; ++i)
        synth.addVoice (new SynthVoice (*wavetableSet, voiceParams));

    synth.addSound (new SynthSound());
}

void PPGWaveCloneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    juce::ignoreUnused (samplesPerBlock);
}

bool PPGWaveCloneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PPGWaveCloneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Tempo del host, para los LFOs sincronizados. 120 BPM si el host no
    // reporta posición (p.ej. algunos modos standalone).
    double bpm = 120.0;
    if (auto* playHead = getPlayHead())
        if (auto position = playHead->getPosition())
            bpm = position->getBpm().orFallback (120.0);

    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
            voice->setHostBpm (bpm);

    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    const float volumeDb = apvts.getRawParameterValue (ParamIDs::masterVolume)->load();
    buffer.applyGain (juce::Decibels::decibelsToGain (volumeDb));
}

juce::AudioProcessorEditor* PPGWaveCloneAudioProcessor::createEditor()
{
    return new PPGWaveCloneAudioProcessorEditor (*this);
}

void PPGWaveCloneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PPGWaveCloneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PPGWaveCloneAudioProcessor();
}
