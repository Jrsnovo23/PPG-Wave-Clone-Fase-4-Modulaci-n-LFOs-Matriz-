#include "ParameterLayout.h"
#include "ParameterIDs.h"
#include "../DSP/WavetableFactory.h"
#include "../DSP/Filter.h"
#include "../DSP/LFO.h"
#include "../DSP/ModulationMatrix.h"

namespace
{
    void addOscillatorParams (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params,
                               int oscNumber, float defaultLevel, int defaultWavetableIndex)
    {
        using namespace ParamIDs;
        const juce::String prefix = "OSC" + juce::String (oscNumber) + " ";

        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            juce::ParameterID { oscWavetableIndex (oscNumber), 1 },
            prefix + "Wavetable",
            WavetableFactory::getDefaultWavetableNames(),
            defaultWavetableIndex));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { oscWavePosition (oscNumber), 1 },
            prefix + "Wave Position",
            juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f },
            0.0f));

        params.push_back (std::make_unique<juce::AudioParameterInt> (
            juce::ParameterID { oscOctave (oscNumber), 1 },
            prefix + "Octave",
            -4, 4, 0));

        params.push_back (std::make_unique<juce::AudioParameterInt> (
            juce::ParameterID { oscCoarseTune (oscNumber), 1 },
            prefix + "Coarse Tune",
            -24, 24, 0));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { oscFineTune (oscNumber), 1 },
            prefix + "Fine Tune",
            juce::NormalisableRange<float> { -100.0f, 100.0f, 0.1f },
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel ("cents")));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { oscLevel (oscNumber), 1 },
            prefix + "Level",
            juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f },
            defaultLevel));
    }

    void addEnvelopeParams (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params,
                             int envNumber, const juce::String& label,
                             float defaultAttack, float defaultDecay, float defaultSustain, float defaultRelease)
    {
        using namespace ParamIDs;
        const juce::String prefix = label + " ";

        auto timeRange = juce::NormalisableRange<float> { 0.001f, 5.0f, 0.001f };
        timeRange.setSkewForCentre (0.3f);

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { envAttack (envNumber), 1 }, prefix + "Attack", timeRange, defaultAttack,
            juce::AudioParameterFloatAttributes().withLabel ("s")));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { envDecay (envNumber), 1 }, prefix + "Decay", timeRange, defaultDecay,
            juce::AudioParameterFloatAttributes().withLabel ("s")));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { envSustain (envNumber), 1 }, prefix + "Sustain",
            juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, defaultSustain));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { envRelease (envNumber), 1 }, prefix + "Release", timeRange, defaultRelease,
            juce::AudioParameterFloatAttributes().withLabel ("s")));
    }

    void addLFOParams (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params, int lfoNumber)
    {
        using namespace ParamIDs;
        const juce::String prefix = "LFO" + juce::String (lfoNumber) + " ";

        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            juce::ParameterID { lfoWaveform (lfoNumber), 1 }, prefix + "Waveform",
            LFOWaveformChoices::getNames(), 0));

        {
            auto rateRange = juce::NormalisableRange<float> { 0.02f, 20.0f, 0.001f };
            rateRange.setSkewForCentre (2.0f);
            params.push_back (std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID { lfoRate (lfoNumber), 1 }, prefix + "Rate", rateRange, 2.0f,
                juce::AudioParameterFloatAttributes().withLabel ("Hz")));
        }

        params.push_back (std::make_unique<juce::AudioParameterBool> (
            juce::ParameterID { lfoSynced (lfoNumber), 1 }, prefix + "Sync", false));

        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            juce::ParameterID { lfoSyncDivision (lfoNumber), 1 }, prefix + "Sync Division",
            LFOSyncDivisionChoices::getNames(), 2));

        params.push_back (std::make_unique<juce::AudioParameterBool> (
            juce::ParameterID { lfoRetrigger (lfoNumber), 1 }, prefix + "Retrigger", true));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { lfoPhase (lfoNumber), 1 }, prefix + "Phase",
            juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 0.0f));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { lfoDepth (lfoNumber), 1 }, prefix + "Depth",
            juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 1.0f));
    }

    void addModSlotParams (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params, int slotNumber,
                            int defaultSource, int defaultDestination, float defaultAmount)
    {
        using namespace ParamIDs;
        const juce::String prefix = "Mod " + juce::String (slotNumber) + " ";

        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            juce::ParameterID { modSlotSource (slotNumber), 1 }, prefix + "Source",
            ModulationSources::getNames(), defaultSource));

        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            juce::ParameterID { modSlotDestination (slotNumber), 1 }, prefix + "Destination",
            ModulationDestinations::getNames(), defaultDestination));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { modSlotAmount (slotNumber), 1 }, prefix + "Amount",
            juce::NormalisableRange<float> { -1.0f, 1.0f, 0.001f }, defaultAmount));
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout ParameterLayout::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // --- Master ------------------------------------------------------------
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::masterVolume, 1 },
        "Master Volume",
        juce::NormalisableRange<float> { -60.0f, 6.0f, 0.01f },
        -6.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("dB")
            .withStringFromValueFunction ([] (float v, int) { return juce::String (v, 1) + " dB"; })
    ));

    // --- Osciladores (Fase 2) -----------------------------------------------
    addOscillatorParams (params, 1, 0.8f, 0);
    addOscillatorParams (params, 2, 0.5f, 2);

    // --- Filtro (Fase 3) -----------------------------------------------------
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParamIDs::filterType, 1 }, "Filter Type",
        FilterTypeChoices::getNames(), 0));

    {
        auto cutoffRange = juce::NormalisableRange<float> { 20.0f, 20000.0f, 1.0f };
        cutoffRange.setSkewForCentre (1000.0f);
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { ParamIDs::filterCutoff, 1 }, "Filter Cutoff", cutoffRange, 8000.0f,
            juce::AudioParameterFloatAttributes().withLabel ("Hz")));
    }

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::filterResonance, 1 }, "Filter Resonance",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 0.15f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::filterKeyTrack, 1 }, "Filter Key Track",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::filterEnvAmount, 1 }, "Filter Env Amount",
        juce::NormalisableRange<float> { -1.0f, 1.0f, 0.001f }, 0.5f));

    // --- Envolventes (Fase 3) -------------------------------------------------
    addEnvelopeParams (params, 1, "Env1 Amp",    0.005f, 0.3f, 0.8f, 0.4f);
    addEnvelopeParams (params, 2, "Env2 Filter", 0.005f, 0.5f, 0.3f, 0.5f);

    // --- LFOs (Fase 4) -----------------------------------------------------
    addLFOParams (params, 1);
    addLFOParams (params, 2);

    // --- Matriz de modulación (Fase 4): 4 rutas, una activa por defecto ------
    addModSlotParams (params, 1, ModulationSources::lfo1, ModulationDestinations::filterCutoff, 0.2f);
    addModSlotParams (params, 2, ModulationSources::none, ModulationDestinations::none, 0.0f);
    addModSlotParams (params, 3, ModulationSources::none, ModulationDestinations::none, 0.0f);
    addModSlotParams (params, 4, ModulationSources::none, ModulationDestinations::none, 0.0f);

    // --- Fases futuras -----------------------------------------------------
    // Fase 5: Chorus, Delay, Reverb, Drive
    // Fase 8: Vintage Character

    return { params.begin(), params.end() };
}
