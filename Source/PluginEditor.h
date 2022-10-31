/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
struct Placeholder : juce::Component
{
    Placeholder();
    void paint (juce::Graphics& g) override
    {
        g.fillAll(customColour);
    }
    juce::Colour customColour;
};

struct GlobalControls : juce::Component
{
    void paint (juce::Graphics& g) override;
};
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
    MultibandCompressorAudioProcessor& audioProcessor;
    
    Placeholder controlBar, analyzer, /*globalControls*/ bandControls;
    GlobalControls globalcontrols;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultibandCompressorAudioProcessorEditor)
};
