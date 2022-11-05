/*
  ==============================================================================

    UtilityComponents.cpp
    Created: 5 Nov 2022 7:06:04pm
    Author:  Sol Harter

  ==============================================================================
*/

#include "UtilityComponents.h"

Placeholder::Placeholder()
{
    juce::Random r;
    customColour = juce::Colour(r.nextInt(255),r.nextInt(255),r.nextInt(255));
}

void Placeholder::paint (juce::Graphics& g)
{
    g.fillAll(customColour);
}

RotarySlider::RotarySlider() :
juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
             juce::Slider::TextEntryBoxPosition::NoTextBox)
{}
