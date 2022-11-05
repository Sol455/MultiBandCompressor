/*
  ==============================================================================

    CustomButtons.h
    Created: 5 Nov 2022 7:00:02pm
    Author:  Sol Harter

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"

struct PowerButton : juce::ToggleButton { };
//==============================================================================

struct AnalyzerButton : juce::ToggleButton
{
    void resized() override;
    juce::Path randomPath;
};
