/*
  ==============================================================================

    UtilityComponents.h
    Created: 5 Nov 2022 7:06:04pm
    Author:  Sol Harter

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct Placeholder : juce::Component
{
    Placeholder();
    void paint (juce::Graphics& g);
    juce::Colour customColour;
};

//==============================================================================

struct RotarySlider : juce::Slider
{
    RotarySlider();
 
};
