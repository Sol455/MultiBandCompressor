/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "GlobalControls.h"
#include "CompressorBandControls.h"
#include "UtilityComponents.h"


//==============================================================================


//==============================================================================


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
    
    Placeholder controlBar, analyzer/*globalControls bandControls;*/;
    GlobalControls globalcontrols{audioProcessor.apvts};
    CompressorBandControls bandControls{audioProcessor.apvts};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultibandCompressorAudioProcessorEditor)
};
