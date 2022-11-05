/*
  ==============================================================================

    GlobalControls.h
    Created: 5 Nov 2022 7:31:11pm
    Author:  Sol Harter

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"


struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);
    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    using RSWL = RotarySliderWithLabels;
    std::unique_ptr<RSWL> inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;
    
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> lowMidXoverSliderAttachment,
                                midHighXoverSliderAttachment,
                                inGainSliderAttachment,
                                outGainSliderAttachment;
    
    
};
