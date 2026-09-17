#include "PluginEditor.h"
#include "Params/ParameterIDs.h"
#include "DSP/WavetableFactory.h"
#include "DSP/Filter.h"
#include "DSP/LFO.h"
#include "DSP/ModulationMatrix.h"

namespace
{
    void setupRotarySlider (juce::Slider& slider, juce::Label& label, const juce::String& labelText,
                             juce::Component& parent)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 18);
        parent.addAndMakeVisible (slider);

        label.setText (labelText, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
        label.setFont (juce::Font (12.0f));
        parent.addAndMakeVisible (label);
    }

    void layoutKnobRow (juce::Rectangle<int> row, const std::vector<std::pair<juce::Slider*, juce::Label*>>& knobs)
    {
        if (knobs.empty())
            return;

        const int knobWidth = row.getWidth() / (int) knobs.size();
        for (auto& knob : knobs)
        {
            auto slot = row.removeFromLeft (knobWidth);
            knob.first->setBounds (slot.removeFromTop (80).reduced (6, 0));
            knob.second->setBounds (slot);
        }
    }
}

PPGWaveCloneAudioProcessorEditor::PPGWaveCloneAudioProcessorEditor (PPGWaveCloneAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    titleLabel.setText ("PPG WAVE CLONE - Fase 4", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setFont (juce::Font (18.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel);

    setupRotarySlider (masterVolumeSlider, masterVolumeLabel, "Master Volume", *this);
    masterVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::masterVolume, masterVolumeSlider);

    setupOscillatorControls (osc1Controls, 1);
    setupOscillatorControls (osc2Controls, 2);

    setupFilterControls();
    setupEnvelopeControls (env1Controls, 1, "ENV 1 (AMP)");
    setupEnvelopeControls (env2Controls, 2, "ENV 2 (FILTER)");

    setupLFOControls (lfo1Controls, 1);
    setupLFOControls (lfo2Controls, 2);

    modMatrixTitle.setText ("MODULATION MATRIX", juce::dontSendNotification);
    modMatrixTitle.setJustificationType (juce::Justification::centred);
    modMatrixTitle.setFont (juce::Font (14.0f, juce::Font::bold));
    addAndMakeVisible (modMatrixTitle);

    for (int i = 0; i < 4; ++i)
        setupModSlotControls (modSlotControls[(size_t) i], i + 1);

    setSize (800, 900);
}

void PPGWaveCloneAudioProcessorEditor::setupOscillatorControls (OscillatorControls& controls, int oscNumber)
{
    controls.title.setText ("OSCILLATOR " + juce::String (oscNumber), juce::dontSendNotification);
    controls.title.setJustificationType (juce::Justification::centred);
    controls.title.setFont (juce::Font (14.0f, juce::Font::bold));
    addAndMakeVisible (controls.title);

    controls.wavetableBox.addItemList (WavetableFactory::getDefaultWavetableNames(), 1);
    addAndMakeVisible (controls.wavetableBox);
    controls.wavetableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processorRef.apvts, ParamIDs::oscWavetableIndex (oscNumber), controls.wavetableBox);

    setupRotarySlider (controls.positionSlider, controls.positionLabel, "Position", *this);
    controls.positionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::oscWavePosition (oscNumber), controls.positionSlider);

    setupRotarySlider (controls.octaveSlider, controls.octaveLabel, "Octave", *this);
    controls.octaveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::oscOctave (oscNumber), controls.octaveSlider);

    setupRotarySlider (controls.coarseSlider, controls.coarseLabel, "Coarse", *this);
    controls.coarseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::oscCoarseTune (oscNumber), controls.coarseSlider);

    setupRotarySlider (controls.fineSlider, controls.fineLabel, "Fine", *this);
    controls.fineAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::oscFineTune (oscNumber), controls.fineSlider);

    setupRotarySlider (controls.levelSlider, controls.levelLabel, "Level", *this);
    controls.levelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::oscLevel (oscNumber), controls.levelSlider);
}

void PPGWaveCloneAudioProcessorEditor::setupFilterControls()
{
    filterControls.title.setText ("FILTER", juce::dontSendNotification);
    filterControls.title.setJustificationType (juce::Justification::centred);
    filterControls.title.setFont (juce::Font (14.0f, juce::Font::bold));
    addAndMakeVisible (filterControls.title);

    filterControls.typeBox.addItemList (FilterTypeChoices::getNames(), 1);
    addAndMakeVisible (filterControls.typeBox);
    filterControls.typeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processorRef.apvts, ParamIDs::filterType, filterControls.typeBox);

    setupRotarySlider (filterControls.cutoffSlider, filterControls.cutoffLabel, "Cutoff", *this);
    filterControls.cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::filterCutoff, filterControls.cutoffSlider);

    setupRotarySlider (filterControls.resonanceSlider, filterControls.resonanceLabel, "Resonance", *this);
    filterControls.resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::filterResonance, filterControls.resonanceSlider);

    setupRotarySlider (filterControls.keyTrackSlider, filterControls.keyTrackLabel, "Key Track", *this);
    filterControls.keyTrackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::filterKeyTrack, filterControls.keyTrackSlider);

    setupRotarySlider (filterControls.envAmountSlider, filterControls.envAmountLabel, "Env Amount", *this);
    filterControls.envAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::filterEnvAmount, filterControls.envAmountSlider);
}

void PPGWaveCloneAudioProcessorEditor::setupEnvelopeControls (EnvelopeControls& controls, int envNumber,
                                                                const juce::String& titleText)
{
    controls.title.setText (titleText, juce::dontSendNotification);
    controls.title.setJustificationType (juce::Justification::centred);
    controls.title.setFont (juce::Font (14.0f, juce::Font::bold));
    addAndMakeVisible (controls.title);

    setupRotarySlider (controls.attackSlider, controls.attackLabel, "Attack", *this);
    controls.attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::envAttack (envNumber), controls.attackSlider);

    setupRotarySlider (controls.decaySlider, controls.decayLabel, "Decay", *this);
    controls.decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::envDecay (envNumber), controls.decaySlider);

    setupRotarySlider (controls.sustainSlider, controls.sustainLabel, "Sustain", *this);
    controls.sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::envSustain (envNumber), controls.sustainSlider);

    setupRotarySlider (controls.releaseSlider, controls.releaseLabel, "Release", *this);
    controls.releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::envRelease (envNumber), controls.releaseSlider);
}

void PPGWaveCloneAudioProcessorEditor::setupLFOControls (LFOControls& controls, int lfoNumber)
{
    controls.title.setText ("LFO " + juce::String (lfoNumber), juce::dontSendNotification);
    controls.title.setJustificationType (juce::Justification::centred);
    controls.title.setFont (juce::Font (14.0f, juce::Font::bold));
    addAndMakeVisible (controls.title);

    controls.waveformBox.addItemList (LFOWaveformChoices::getNames(), 1);
    addAndMakeVisible (controls.waveformBox);
    controls.waveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processorRef.apvts, ParamIDs::lfoWaveform (lfoNumber), controls.waveformBox);

    setupRotarySlider (controls.rateSlider, controls.rateLabel, "Rate", *this);
    controls.rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::lfoRate (lfoNumber), controls.rateSlider);

    setupRotarySlider (controls.phaseSlider, controls.phaseLabel, "Phase", *this);
    controls.phaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::lfoPhase (lfoNumber), controls.phaseSlider);

    setupRotarySlider (controls.depthSlider, controls.depthLabel, "Depth", *this);
    controls.depthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::lfoDepth (lfoNumber), controls.depthSlider);
}

void PPGWaveCloneAudioProcessorEditor::setupModSlotControls (ModSlotControls& controls, int slotNumber)
{
    controls.sourceBox.addItemList (ModulationSources::getNames(), 1);
    addAndMakeVisible (controls.sourceBox);
    controls.sourceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processorRef.apvts, ParamIDs::modSlotSource (slotNumber), controls.sourceBox);

    controls.destinationBox.addItemList (ModulationDestinations::getNames(), 1);
    addAndMakeVisible (controls.destinationBox);
    controls.destinationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processorRef.apvts, ParamIDs::modSlotDestination (slotNumber), controls.destinationBox);

    controls.amountSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    controls.amountSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    addAndMakeVisible (controls.amountSlider);
    controls.amountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, ParamIDs::modSlotAmount (slotNumber), controls.amountSlider);
}

void PPGWaveCloneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a1f));
}

void PPGWaveCloneAudioProcessorEditor::layoutOscillatorControls (OscillatorControls& controls, juce::Rectangle<int> area)
{
    controls.title.setBounds (area.removeFromTop (20));
    controls.wavetableBox.setBounds (area.removeFromTop (24).reduced (20, 0));
    area.removeFromTop (8);

    auto knobRow = area.removeFromTop (110);
    layoutKnobRow (knobRow, {
        { &controls.positionSlider, &controls.positionLabel },
        { &controls.octaveSlider,   &controls.octaveLabel },
        { &controls.coarseSlider,   &controls.coarseLabel },
        { &controls.fineSlider,     &controls.fineLabel },
        { &controls.levelSlider,    &controls.levelLabel },
    });
}

void PPGWaveCloneAudioProcessorEditor::layoutFilterControls (juce::Rectangle<int> area)
{
    filterControls.title.setBounds (area.removeFromTop (20));
    filterControls.typeBox.setBounds (area.removeFromTop (24).reduced (10, 0));
    area.removeFromTop (8);

    auto knobRow = area.removeFromTop (110);
    layoutKnobRow (knobRow, {
        { &filterControls.cutoffSlider,    &filterControls.cutoffLabel },
        { &filterControls.resonanceSlider, &filterControls.resonanceLabel },
        { &filterControls.keyTrackSlider,  &filterControls.keyTrackLabel },
        { &filterControls.envAmountSlider, &filterControls.envAmountLabel },
    });
}

void PPGWaveCloneAudioProcessorEditor::layoutEnvelopeControls (EnvelopeControls& controls, juce::Rectangle<int> area)
{
    controls.title.setBounds (area.removeFromTop (20));
    area.removeFromTop (32); // alinea con paneles que sí tienen combo box

    auto knobRow = area.removeFromTop (110);
    layoutKnobRow (knobRow, {
        { &controls.attackSlider,  &controls.attackLabel },
        { &controls.decaySlider,   &controls.decayLabel },
        { &controls.sustainSlider, &controls.sustainLabel },
        { &controls.releaseSlider, &controls.releaseLabel },
    });
}

void PPGWaveCloneAudioProcessorEditor::layoutLFOControls (LFOControls& controls, juce::Rectangle<int> area)
{
    controls.title.setBounds (area.removeFromTop (20));
    controls.waveformBox.setBounds (area.removeFromTop (24).reduced (20, 0));
    area.removeFromTop (8);

    auto knobRow = area.removeFromTop (110);
    layoutKnobRow (knobRow, {
        { &controls.rateSlider,  &controls.rateLabel },
        { &controls.phaseSlider, &controls.phaseLabel },
        { &controls.depthSlider, &controls.depthLabel },
    });
}

void PPGWaveCloneAudioProcessorEditor::layoutModSlotControls (ModSlotControls& controls, juce::Rectangle<int> area)
{
    auto row = area.reduced (0, 2);
    const int comboWidth = row.getWidth() / 3;
    controls.sourceBox.setBounds (row.removeFromLeft (comboWidth).reduced (4, 0));
    controls.destinationBox.setBounds (row.removeFromLeft (comboWidth).reduced (4, 0));
    controls.amountSlider.setBounds (row.reduced (4, 0));
}

void PPGWaveCloneAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (16);

    titleLabel.setBounds (area.removeFromTop (28));
    area.removeFromTop (8);

    auto masterArea = area.removeFromTop (110);
    masterVolumeSlider.setBounds (masterArea.withSizeKeepingCentre (90, 80).translated (0, -10));
    masterVolumeLabel.setBounds (masterArea.removeFromBottom (18));

    area.removeFromTop (12);

    auto oscRow = area.removeFromTop (162);
    auto osc1Area = oscRow.removeFromLeft (oscRow.getWidth() / 2).reduced (8, 0);
    auto osc2Area = oscRow.reduced (8, 0);
    layoutOscillatorControls (osc1Controls, osc1Area);
    layoutOscillatorControls (osc2Controls, osc2Area);

    area.removeFromTop (12);

    auto lowerRow = area.removeFromTop (162);
    const int thirdWidth = lowerRow.getWidth() / 3;
    auto filterArea = lowerRow.removeFromLeft (thirdWidth).reduced (8, 0);
    auto env1Area   = lowerRow.removeFromLeft (thirdWidth).reduced (8, 0);
    auto env2Area   = lowerRow.reduced (8, 0);
    layoutFilterControls (filterArea);
    layoutEnvelopeControls (env1Controls, env1Area);
    layoutEnvelopeControls (env2Controls, env2Area);

    area.removeFromTop (12);

    auto lfoRow = area.removeFromTop (162);
    auto lfo1Area = lfoRow.removeFromLeft (lfoRow.getWidth() / 2).reduced (8, 0);
    auto lfo2Area = lfoRow.reduced (8, 0);
    layoutLFOControls (lfo1Controls, lfo1Area);
    layoutLFOControls (lfo2Controls, lfo2Area);

    area.removeFromTop (12);

    modMatrixTitle.setBounds (area.removeFromTop (20));
    area.removeFromTop (4);
    for (auto& slot : modSlotControls)
    {
        layoutModSlotControls (slot, area.removeFromTop (28));
        area.removeFromTop (4);
    }
}
