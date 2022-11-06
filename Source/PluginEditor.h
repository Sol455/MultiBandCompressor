/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/LookAndFeel.h"
#include "GUI/GlobalControls.h"
#include "GUI/CompressorBandControls.h"
#include "GUI/UtilityComponents.h"
#include "GUI/SpectrumAnalyzer.h"


/**
*/
class MultibandCompressorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MultibandCompressorAudioProcessorEditor (MultibandCompressorAudioProcessor&);
    ~MultibandCompressorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LookAndFeel lnf;
    MultibandCompressorAudioProcessor& audioProcessor;
    
    Placeholder controlBar/*, analyzer globalControls bandControls;*/;
    GlobalControls globalcontrols{audioProcessor.apvts};
    CompressorBandControls bandControls{audioProcessor.apvts};
    SpectrumAnalyzer analyzer{audioProcessor};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultibandCompressorAudioProcessorEditor)
};
