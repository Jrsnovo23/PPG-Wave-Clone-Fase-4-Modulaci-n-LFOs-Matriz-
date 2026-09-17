#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include "PluginProcessor.h"

/**
    Editor de la Fase 4: agrega paneles de LFO1/LFO2 y la matriz de
    modulación (4 rutas) sobre la base de la Fase 3. Sigue siendo una
    interfaz genérica de JUCE, no la estética final — eso es la Fase 7.

    Nota: Sync (on/off) y Retrigger de cada LFO son parámetros reales y
    automatizables, pero esta interfaz provisional no les da un control
    dedicado (ya hay 7 controles por LFO contando eso — no entra cómodo).
    Se pueden automatizar igual desde la lista de parámetros de Ableton
    Live; la Fase 7 les da su lugar en la GUI final.
*/
class PPGWaveCloneAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit PPGWaveCloneAudioProcessorEditor (PPGWaveCloneAudioProcessor&);
    ~PPGWaveCloneAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct OscillatorControls
    {
        juce::Label title;
        juce::ComboBox wavetableBox;
        juce::Slider positionSlider, octaveSlider, coarseSlider, fineSlider, levelSlider;
        juce::Label positionLabel, octaveLabel, coarseLabel, fineLabel, levelLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> wavetableAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
            positionAttachment, octaveAttachment, coarseAttachment, fineAttachment, levelAttachment;
    };

    struct FilterControls
    {
        juce::Label title;
        juce::ComboBox typeBox;
        juce::Slider cutoffSlider, resonanceSlider, keyTrackSlider, envAmountSlider;
        juce::Label cutoffLabel, resonanceLabel, keyTrackLabel, envAmountLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
            cutoffAttachment, resonanceAttachment, keyTrackAttachment, envAmountAttachment;
    };

    struct EnvelopeControls
    {
        juce::Label title;
        juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
        juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
            attackAttachment, decayAttachment, sustainAttachment, releaseAttachment;
    };

    struct LFOControls
    {
        juce::Label title;
        juce::ComboBox waveformBox;
        juce::Slider rateSlider, phaseSlider, depthSlider;
        juce::Label rateLabel, phaseLabel, depthLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
            rateAttachment, phaseAttachment, depthAttachment;
    };

    struct ModSlotControls
    {
        juce::ComboBox sourceBox, destinationBox;
        juce::Slider amountSlider;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sourceAttachment, destinationAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountAttachment;
    };

    void setupOscillatorControls (OscillatorControls& controls, int oscNumber);
    void setupFilterControls();
    void setupEnvelopeControls (EnvelopeControls& controls, int envNumber, const juce::String& titleText);
    void setupLFOControls (LFOControls& controls, int lfoNumber);
    void setupModSlotControls (ModSlotControls& controls, int slotNumber);

    void layoutOscillatorControls (OscillatorControls& controls, juce::Rectangle<int> area);
    void layoutFilterControls (juce::Rectangle<int> area);
    void layoutEnvelopeControls (EnvelopeControls& controls, juce::Rectangle<int> area);
    void layoutLFOControls (LFOControls& controls, juce::Rectangle<int> area);
    void layoutModSlotControls (ModSlotControls& controls, juce::Rectangle<int> area);

    PPGWaveCloneAudioProcessor& processorRef;

    juce::Label titleLabel;
    juce::Slider masterVolumeSlider;
    juce::Label  masterVolumeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolumeAttachment;

    OscillatorControls osc1Controls, osc2Controls;
    FilterControls filterControls;
    EnvelopeControls env1Controls, env2Controls;
    LFOControls lfo1Controls, lfo2Controls;
    juce::Label modMatrixTitle;
    std::array<ModSlotControls, 4> modSlotControls;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PPGWaveCloneAudioProcessorEditor)
};
